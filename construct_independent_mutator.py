#Todo: Create way to save variants and keep files

#Todo: Maybe be able to add tests to existing files if the name coincides and there is space?
#Todo: Allow for full file text editing and bypass test file generation step. Would reload test object from string


"""
This is a demonstration file for what I call a mutator.  These can be built
for the purpose of adding depth testing to various aspects of existing tests.
In order to add a mutator, please see the given config file to see the
appriate flag to add to the infrastructure script.

The file must contain the function mutate, and the function will be passed a
test object as defined in the infrastructure script.  Lower, you will see a
more detailed description of the attributes of the test object.  In order to
mutate a test, at least the test contents should be changed, and then the
mutated test returned.  If the test does not meet any of the conditions of
the mutation, the function can return None and no test will be added to the
runtime

Parameter test is a test object as it can be found in the infrastructure.py file
The variable has the following sub-attributes:

    test.path : type(string)
        This variable will contain the full path of the original test file

    test.name : type(string)
        This variable will contain the name of the test name.  This value
        should be updated if you don't want the results of the mutated test
        to be lumped together with the non-mutated form of the test.

    test.tags : type(dict[string] -> list[string])
        This variable containts for each test number (given as '1', '2', ...)
        returns a list of the tags that are listed for the test. For instance
        if you accessed test.tags['1'][0], it would give you the first tag on
        test '1'.  You can assign new tags to this test and use them in the
        filters if you would only like to run a mutated form of the test

    test.versions : type(dict[string] -> list[string])
        This variable contains a dictionary that can be accessed with each
        test number (given as '1', '2', ...) and gives a list of OpenACC
        versions with which they are compatible.  This can be updated, but
        I would recommend only updating it as appropriate such as removing
        versions if your mutation changes the compatibility with a version

    test.contents : type(dict[string] -> list[string])
        This variable contains the test contents for each test number (given
        as '1', '2', ...) and gives a list of strings which are the lines of
        the contents of the test.  These should be updated to represent the
        new code that you want tested.

    test.tests : type(list[string])
        This variable contains a list of the test number keys.  This can be
        used to iterate through test.tags, test.versions, and test.contents.


The following functions are provided to assist in the process:
    isFortran() -> Boolean:
        returns whether the test is Fortran or not
    isC() -> Boolean
        returns whether the test is C or not
    isCPP() -> Boolean
        returns wheter the test is C++ or not
    isACC(line) -> Boolean
        returns whether the given string is an ACC pragma
    limitVersions(test_section, [Optional] starting_version, [Optional] ending_version) -> List(string)
        returns a list of OpenACC versions which is the subset of the overlap of the existing versions
        and the given range from starting_version to ending_version.  (If not given, starting version is
        1.0 and ending version is the most recent listed in OpenACC_Versions)
    clauseInLine(clause, line) -> Boolean
        returns whether the given clause is in the string given as line.  This should probably be wrapped
        with isACC(line) but that can be done at the user's discretion
    replaceClauseInLine(clause, line, replacement) -> string
        returns the line with the given clause replaced with the replacement
"""

g_test = None

OpenACC_Versions = ["1.0", "2.0", "2.5", "2.6", "2.7", "3.0"]

def isFortran():
    testname = g_test.name
    if testname[-4:] == ".F90":
        return True
    elif testname[-4:] == ".f90":
        return True
    elif testname[-4:] == ".for":
        return True
    elif testname[-4:] == ".f95":
        return True
    elif testname[-4:] == ".F90":
        return True
    elif testname[-4:] == ".f03":
        return True
    elif testname[-4:] == ".F03":
        return True
    else:
        return False


def isC():
    testname = g_test.name
    if testname[-2:] == ".c":
        return True
    else:
        return False


def isCPP():
    testname = g_test.name
    if testname[-3:] == ".cc":
        return True
    elif testname[-2:] == ".C":
        return True
    elif testname[-4:] == ".cxx":
        return True
    elif testname[-4:] == ".c++":
        return True
    elif testname[-4:] == ".cpp":
        return True
    elif testname[-4:] == ".CPP":
        return True
    elif testname[-3:] == ".cp":
        return True
    else:
        return False

def isACC(line):
    if isFortran():
        if line.strip().startswith("!$acc "):
            return True
        else:
            return False
    else:
        if line.strip().startswith("#pragma acc "):
            return True
        else:
            return False

def limitVersions(test_num, start=None, end=None):
    if start is None:
        start = OpenACC_Versions[0]
    if end is None:
        end = OpenACC_Versions[-1]
    new_versions = []
    for version in OpenACC_Versions:
        if version in g_test.versions[test_num]:
            if start == version or float(start) < float(version):
                if end == version or float(end) > float(version):
                    new_versions.append(version)
    return new_versions

def clauseInLine(clause, line):
    #This should be improved to determine which have parameters and if they can appear at the end and such
    for x in [' ', '\n', '(']:
        if " " + clause + x in line:
            return True
    return False

def replaceClauseInLine(clause, line, replacement):
    #Same as above
    for x in [' ', '\n', '(']:
        if " " + clause + x in line:
            line = line.replace(" " + clause + x, " " + replacement + x)
    return line

"""
This mutation is going to take a given test and create variants that only use parallel, kernels, or serial as
the construct.  It will check if construct-independent is used as a tag in any of the tests, and if so, it
will create additional variants for testing and return them.
"""

def mutate(test):
    global g_test
    g_test = test
    returned = []
    occurance = [0, 0, 0]
    #First, we need to check the original usage of the constructs.
    for test_num in test.tests:
        if 'construct-independent' in test.tags[test_num]:
            for line in list(range(len(test.contents[test_num]))):
                if isACC(test.contents[test_num][line]):
                    if clauseInLine("parallel", test.contents[test_num][line]):
                        occurance[0] += 1
                    if clauseInLine("kernels", test.contents[test_num][line]):
                        occurance[1] += 1
                    if clauseInLine("serial", test.contents[test_num][line]):
                        occurance[2] += 1
    total = occurance[0] + occurance[1] + occurance[2]
    maximum = max(occurance)
    if total == 0:
        #If there are no occurances of parallel, kernels, or serial in the set of construct-independent tests,
        #then there is nothing to change about the base test.
        print("Test " + test.name + " exited due to no constructs")
        return None
    if total == maximum:
        #If each usage of a construct is the same type, then we don't need to create a variant that would be an
        #implementation of the original test code
        variants = []
        if occurance[0] == 0:
            variants.append('parallel')
        if occurance[1] == 0:
            variants.append('kernels')
        if occurance[2] == 0:
            variants.append('serial')
    else:
        #If there is a mix of the constructs used, we will create a variant for each case with that construct being
        #used universally
        variants = ['parallel', 'kernels', 'serial']
    for target in variants:
        #We create a new copy of the test and we replace lines with constructs in tests which are construct-independent
        current_version = test.copy()
        for test_num in current_version.tests:
            if "construct-independent" in test.tags[test_num]:
                for line in list(range(len(current_version.contents[test_num]))):
                    if isACC(current_version.contents[test_num][line]):
                        current_version.contents[test_num][line] = replaceClauseInLine("parallel", current_version.contents[test_num][line], target)
                        current_version.contents[test_num][line] = replaceClauseInLine("kernels", current_version.contents[test_num][line], target)
                        current_version.contents[test_num][line] = replaceClauseInLine("serial", current_version.contents[test_num][line], target)
                current_version.tags[test_num].append(target.strip())
                current_version.tags[test_num].remove("construct-independent")
                #You could add more dynamic version checking based on more conditions, but the construct-independent
                #tag implies that no version breaking features are used.
                if target == "serial":
                    #Since serial was added in 2.6, we need to limit the test versions to ones that are after 2.6
                    current_version.versions[test_num] = limitVersions(test_num, start="2.6")
        current_version.name = current_version.name.split(".")[0] + "_" + target.strip() + "." + current_version.name.split(".")[-1]
        returned.append(current_version)
    return returned
    
