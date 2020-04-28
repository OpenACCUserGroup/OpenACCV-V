from os import listdir, sep, environ, rmdir, remove, mkdir
from os.path import isfile, join, dirname, realpath, exists, isdir
import re
import subprocess
import sys
import json
import datetime
import platform
import traceback
from timeit import default_timer as time
import threading
import multiprocessing
import random

# Get explicit information from user about accelerator information
# System Name
# Define Iteration count inside the infrastructure
# TODO: Support neg-tests
# TODO: Compare two output files
# TODO: Support html json output as input for merge
# TODO: Improve html report.  Maybe even try to build into it original source with highlighting showing what
# sections failed for what reasons
# TODO: Support construct-independent builds for testing all equivalent constructs in any situation
# TODO: Add Named Systems | Dynamic System Attributes | Target, SEED, MPI, Versioning
# TODO: Find bug that is causing duplicate entry into results metadata tables (comparison issue?)



# TODO: Parallel testing

class shellInterface:
    def __init__(self):
        self.env = None
        if g_config is not None:
            if g_config.env is not None:
                self.env = g_config.env
        if self.env is None:
            self.env = environ.copy()

    def runCommand(self, args, cwd=None):
        global g_subprocess_runtime
        if isinstance(args, list):
            args = ' '.join(args)
        if g_verbose['commands']:
            print("Executing: " + args)
        start = time()
        if cwd is None:
            command = subprocess.Popen(args, env=self.env, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        else:
            command = subprocess.Popen(args, env=self.env, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)
        result = None
        completed = True
        if sys.version_info[0] == 2 or (sys.version_info[0] == 3 and sys.version_info[1] < 3):
            result = command.wait()
            runtime = time() - start
        else:
            try:
                result = command.wait(g_config.timeout)
                runtime = time() - start
            except subprocess.TimeoutExpired:
                runtime = -1
                command.kill()
                completed = False
        g_subprocess_runtime += runtime
        if completed:
            [out, err] = command.communicate()
            if g_verbose['output']:
                if out.decode('utf-8') != '':
                    print(out.decode('utf-8'))
            if g_verbose['errors']:
                if err.decode('utf-8') != '':
                    print(err.decode('utf-8'))
            if g_verbose['results']:
                print("Process completed with returncode: " + str(result))
            try:
                out = out.decode('utf-8')
                err = err.decode('utf-8')
            except UnicodeDecodeError:
                print("Error converting output to utf-8")
                g_results.log("Could not convert output to utf-8 string")
                return [result, "Could not convert output to utf-8 string", "Could not convert output to utf-8 string", runtime]
            if g_results is not None:
                g_results.log(' '.join(args) + '\n' + out + '\n' + err)
            return [result, out, err, runtime]
        else:
            if g_verbose['errors']:
                print("Task did not complete within timeout")
            if g_results is not None:
                g_results.log("Command: " + args + " failed to complete within the timeout")
            return [-1, "", "Failed to complete within timeout", runtime]


class results:
    def __init__(self):
        self.export_format = g_config.export_format
        self.data = {'runs': {}, 'testsuites': [], 'systems': [], 'configs': []}
        self.fullLog = ""

    def run_init(self):
        if g_config.partial:
            self.load_results_for_config()
            g_testsuite.filter_test_list_with_partial_results(self)
        else:
            self.clean_partial_results()
        self.submit_config()
        self.submit_system()
        self.submit_testsuite()

    def load_results_for_config(self):
        temp_files = listdir(g_config.partial_results_dir)
        if len(temp_files) == 0:
            return
        self.load_partial_results_metadata()
        [g_config_id, g_system_id, g_testsuite_id] = self.get_id_set_for_partial_results()
        # If any part of the current environment is different, the partial results
        # are not going to be representative
        if g_config_id == -1 or g_system_id == -1 or g_testsuite_id == -1:
            return
        for filename in temp_files:
            if filename in ["systems.json", "configs.json", "testsuites.json"]:
                continue
            file_object = open(join(g_config.partial_results_dir, filename), 'r')
            test_run_info = jsonLoadWrapper(file_object)
            file_object.close()
            for run in test_run_info:
                if run['testsuite_id'] == g_testsuite_id:
                    if run['compilation']['config'] == g_config_id:
                        if run['runtime']['system'] == g_system_id:
                            if filename not in self.data['runs'].keys():
                                self.data['runs'][filename] = []
                            self.data['runs'][filename].append(run)

    def load_partial_results_metadata(self):
        temp_files = listdir(g_config.partial_results_dir)
        if len(temp_files) == 0:
            return
        for filename in temp_files:
            if filename in ["systems.json", "configs.json", "testsuites.json"]:
                file_object = open(join(g_config.partial_results_dir, filename), 'r')
                self.data[filename.split(".")[0]] = jsonLoadWrapper(file_object)
                file_object.close()

    def get_id_set_for_partial_results(self):
        g_config_id = -1
        g_system_id = -1
        g_testsuite_id = -1
        for conf in self.data['configs']:
            if g_config == conf:
                g_config_id = conf['id']
                g_config.config_id = g_config_id
        for syst in self.data['systems']:
            if g_system == syst:
                g_system_id = syst['id']
                g_system.id = g_system_id
        for ts in self.data['testsuites']:
            is_same = True
            for testname in ts.keys():
                if testname == "id":
                    continue
                test_object = g_testsuite.get_test(testname)
                if test_object is not None:
                    for testnum in ts[testname]["tests"].keys():
                        if testnum in test_object.contents.keys():
                            if ts[testname]["tests"][testnum]['content'] != '\n'.join(test_object.contents[testnum]):
                                is_same = False
                                break
                        else:
                            is_same = False
                            break
                else:
                    is_same = False
                    break
            if is_same:
                g_testsuite_id = ts["id"]
                g_testsuite.id = g_testsuite_id
                break
        return [g_config_id, g_system_id, g_testsuite_id]

    def clean_partial_results(self):
        temp_files = listdir(g_config.partial_results_dir)
        if len(temp_files) == 0:
            return
        self.load_partial_results_metadata()
        [g_config_id, g_system_id, g_testsuite_id] = self.get_id_set_for_partial_results()
        if g_config_id == -1 or g_system_id == -1 or g_testsuite_id == -1:
            return
        for filename in temp_files:
            if filename in ["systems.json", "configs.json", "testsuites.json"]:
                continue
            file_object = open(join(g_config.partial_results_dir, filename), 'r')
            test_run_info = jsonLoadWrapper(file_object)
            file_object.close()
            runs_to_remove = []
            for run in test_run_info:
                if run['testsuite_id'] == g_testsuite_id:
                    if run['compilation']['config'] == g_config_id:
                        if run['runtime']['system'] == g_system_id:
                            runs_to_remove.append(run)
            for run in runs_to_remove:
                test_run_info.remove(run)
            if len(test_run_info) == 0:
                try:
                    remove(join(g_config.partial_results_dir, filename))
                except OSError:
                    if g_verbose['oserrors']:
                        print("Couldn't delete old results in file: " + join(g_config.partial_results_dir, filename))
            else:
                try:
                    file_object = open(join(g_config.partial_results_dir, filename), 'w')
                    json.dump(test_run_info, file_object, indent=4, sort_keys=True)
                    file_object.close()
                except OSError:
                    if g_verbose['oserrors']:
                        print("Could not edit file: " + join(g_config.partial_results_dir, filename) + ". Continuing...")

    def temp_dump(self, test_obj):
        """
        :param Optional[str] test_obj:
        :rtype: None
        """
        assert_created_directory(g_config.build_dir)
        assert_created_directory(g_config.partial_results_dir)
        if isinstance(test_obj, str):
            testname = test_obj
        else:
            testname = test_obj.name
        if isfile(join(g_config.partial_results_dir, testname)):
            file_object = open(join(g_config.partial_results_dir, testname), 'r')
            try:
                existing_test_data = jsonLoadWrapper(file_object)
            except json.decoder.JSONDecodeError:
                existing_test_data = []
            file_object.close()
        else:
            existing_test_data = []
        for run in self.data['runs'][testname]:
            existing_test_data.append(run)
        file_object = open(join(g_config.partial_results_dir, testname), 'w')
        json.dump(existing_test_data, file_object, indent=4, sort_keys=True)
        file_object.close()

    def log(self, text):
        """
        :param str text:
        :rtype: None
        """
        if len(self.fullLog) > 0:
            if self.fullLog[-1] != '\n':
                self.fullLog = self.fullLog + '\n'
        self.fullLog = self.fullLog + text
        return

    def has_complete_entry(self, testname):
        """
        :param str testname:
        :rtype: bool
        """
        if testname in self.data['runs'].keys():
            for run in self.data['runs'][testname]:
                if 'runtime' in run.keys() and "compilation" in run.keys():
                    if run['runtime']['system'] == g_system.id:
                        if run['compilation']['config'] == g_config.config_id:
                            if run['testsuite_id'] == g_testsuite.id:
                                if run['runtime']['export']:
                                    if run['compilation']['export']:
                                        return True
        return False

    def enter_test(self, test_object):
        """
        :param test test_object:
        :rtype: None
        """
        assert ("testsuites" in self.data.keys())
        ts_reference = None
        for ts in self.data['testsuites']:
            if ts['id'] == g_testsuite.id:
                ts_reference = ts
        assert test_object.name in ts_reference.keys()
        assert len(test_object.tests) == ts_reference[test_object.name]['num tests']
        assert "tests" in ts_reference[test_object.name].keys()
        for test_no in test_object.tests:
            assert test_no in ts_reference[test_object.name]['tests'].keys()
            assert test_object.tags[test_no] == ts_reference[test_object.name]['tests'][test_no]['tags']
            assert test_object.versions[test_no] == ts_reference[test_object.name]['tests'][test_no]['versions']

    def submit_system(self, submitted_system=None):
        """
        :rtype: int
        """
        if submitted_system is not None:
            system_info = submitted_system
        else:
            system_info = g_system
        for system_dict in self.data['systems']:
            if system_info == system_dict:
                system_info.id = system_dict['id']
                return system_info.id
        unique = False
        id_num = len(self.data['systems']) + 1
        while not unique:
            unique = True
            for system_dict in self.data['systems']:
                if system_dict['id'] == id_num:
                    unique = False
            if not unique:
                id_num = int(random.random() * 1000000)
        system_info.id = id_num
        self.data['systems'].append(vars(system_info))
        self.data['systems'][-1]['id'] = id_num
        f = open(join(g_config.partial_results_dir, "systems.json"), 'w')
        json.dump(self.data['systems'], f, indent=4, sort_keys=True)
        f.close()
        return id_num

    def submit_config(self, submitted_config=None):
        """
        :rtype: int
        """
        #config_dict = vars(g_config)
        if submitted_config is not None:
            config_info = submitted_config
        else:
            config_info = g_config
        for conf in self.data['configs']:
            if config_info == conf:
                config_info.id = conf['id']
                return config_info.id
        unique = False
        id_num = len(self.data['configs']) + 1
        while not unique:
            unique = True
            for conf in self.data['configs']:
                if conf['id'] == id_num:
                    unique = False
            if not unique:
                id_num = int(random.random() * 1000000)
        config_info.id = id_num
        self.data['configs'].append(vars(config_info))
        f = open(join(g_config.partial_results_dir, "configs.json"), 'w')
        json.dump(self.data['configs'], f, indent=4, sort_keys=True)
        f.close()
        return id_num

    def submit_testsuite(self, submitted_testsuite=None):
        if submitted_testsuite is not None:
            testsuite_ref = submitted_testsuite
        else:
            testsuite_ref = g_testsuite
        for existing_suite in self.data['testsuites']:
            is_same = True
            for testname in existing_suite.keys():
                if testname == "id":
                    continue
                test_object = testsuite_ref.get_test(testname)
                if test_object is not None:
                    for testnum in existing_suite[testname]["tests"].keys():
                        if testnum in test_object.contents.keys():
                            if existing_suite[testname]["tests"][testnum]['content'] != ''.join(test_object.contents[testnum]):
                                is_same = False
                                break
                        else:
                            is_same = False
                            break
                else:
                    is_same = False
                    break
            if is_same:
                testsuite_ref.id = existing_suite['id']
                return testsuite_ref.id
        unique = False
        id_num = len(self.data['testsuites']) + 1
        while not unique:
            unique = True
            for ts in self.data['testsuites']:
                if ts['id'] == id_num:
                    unique = False
            if not unique:
                id_num = int(random.random() * 1000000)
        testsuite_ref.id = id_num
        self.data['testsuites'].append(testsuite_ref.get_dict_info())
        f = open(join(g_config.partial_results_dir, "testsuites.json"), 'w')
        json.dump(self.data['testsuites'], f, indent=4, sort_keys=True)
        f.close()
        return id_num


    def mark(self, testname):
        """
        :param str testname: 
        :rtype: None
        """
        self.data['runs'][testname][-1]['runtime']['export'] = True
        self.data['runs'][testname][-1]['compilation']['export'] = True
        self.temp_dump(testname)

    def submit_no_runtime(self, testname):
        """
        :param str testname:
        :rtype: None
        """
        self.data['runs'][testname][-1]['runtime'] = {}
        self.data['runs'][testname][-1]['runtime']['export'] = False
        self.data['runs'][testname][-1]['runtime']['system'] = g_system.id

    def submit_runtime(self, testname, result, output, error, runtime):
        """
        :param str testname:
        :param int result:
        :param str output:
        :param str error:
        :param float runtime:
        :rtype: None
        """
        self.fullLog = self.fullLog + '\n' + output + '\n' + error + '\n' + "Test completed with an exit code of " + str(result) + '\n'
        self.data['runs'][testname][-1]['runtime'] = {"result": result,
                                              "output": output,
                                              "errors": error,
                                              "system": g_system.id,
                                              "runtime": runtime,
                                              "export": False}

    def submit_compile(self, testname, args, result, output, err, runtime):
        """
        :param str testname:
        :param List[str] args:
        :param int result:
        :param str output:
        :param str err:
        :param float runtime:
        :rtype: None
        """
        self.fullLog = self.fullLog = '\n' + ' '.join(args) + '\n' + output + '\n' + err + '\n' + "Compilation completed with an exit code of " + str(results) + '\n'
        if len(self.data['runs'][testname]) == 0:
            self.data['runs'][testname].append({"testsuite_id": g_testsuite.id})
        elif 'compilation' in self.data['runs'][testname][-1].keys():  # Test already completed compilation for last run.  This must be the start of next run
            self.data['runs'][testname].append({"testsuite_id": g_testsuite.id})
        self.data['runs'][testname][-1]["compilation"]={"result": result,
                                                   "output": output,
                                                   "errors": err,
                                                   "command": ' '.join(args),
                                                   "runtime": runtime,
                                                   "config": g_config.id,
                                                   "export": False}

    def submit_command_set(self, command_set_type, testname, commands, processed_commands, return_codes, outputs, errs):
        """
        :param str command_set_type:
        :param str testname:
        :param List[str] commands:
        :param List[str] processed_commands:
        :param List[int] return_codes:
        :param List[str] outputs:
        :param List[str] errs:
        :rtype: None
        """
        if g_verbose['debug']:
            if len(commands) != len(return_codes) or len(return_codes) != len(outputs) or len(outputs) != len(errs) or len(errs) != len(processed_commands):
                print("Error with exporting command set results.  Different sized lists")
                sys.exit()
        if command_set_type == "pre-compile commands":
            if testname not in self.data['runs'].keys():
                self.data['runs'][testname] = []
            if len(self.data['runs'][testname]) == 0:
                self.data['runs'][testname].append({"testsuite_id":g_testsuite.id})
            elif command_set_type in self.data['runs'][testname][-1].keys():  #  Last run already has pre-compile command results, this must be the beginning of the next run
                self.data['runs'][testname].append({"testsuite_id":g_testsuite.id})
        self.data['runs'][testname][-1][command_set_type] = []
        for x in list(range(len(commands))):
            self.data['runs'][testname][-1][command_set_type].append({'command':commands[x],
                                                              'processed command': processed_commands[x],
                                                              'result': return_codes[x],
                                                              'output': outputs[x],
                                                              'err': errs[x]})

    def output(self, filepath):
        file_object = open(filepath, 'w')
        if self.export_format == "json":
            json.dump(self.data, file_object, indent=4, sort_keys=True)
        elif self.export_format == "html":
            file_object.write("var jsonResults = ")
            json.dump(self.data, file_object, indent=4, sort_keys=True)
            file_object.write(";")
        elif self.export_format == "txt":
            file_object.write(self.fullLog)
        else:
            print("Did not recognize export format.  Exporing results as json")
            json.dump(self.data, file_object, indent=4, sort_keys=True)
        file_object.close()
        self.clean_partial_results()

    def add_results_file(self, results_file_path):
        system_translation = {}
        config_translation = {}
        testsuite_translation = {}
        results_file_object = open(results_file_path, 'r')
        new_results = jsonLoadWrapper(results_file_object)
        results_file_object.close()
        for new_config in new_results['configs']:
            new_config_obj = config(new_config)
            for existing_config_dict in self.data['configs']:
                if new_config_obj == existing_config_dict: #Overloaded operator in config
                    config_translation[new_config['id']] = existing_config_dict['id']
            if new_config['id'] not in config_translation.keys():
                id = self.submit_config(new_config_obj)
                config_translation[new_config['id']] = id
        for new_system in new_results['systems']:
            new_system_obj = system(new_system)
            for existing_system_dict in self.data['systems']:
                if new_system_obj == existing_system_dict: #Overloaded operator in system
                    system_translation[new_system['id']] = existing_system_dict['id']
            if new_system['id'] not in system_translation.keys():
                id = self.submit_system(new_system_obj)
                system_translation[new_system['id']] = id
        for new_testsuite in new_results['testsuites']:
            for existing_testsuite_dict in self.data['testsuites']:
                if testsuite_compare(new_testsuite, existing_testsuite_dict):
                    testsuite_translation[new_testsuite['id']] = existing_testsuite_dict['id']
            if new_testsuite['id'] not in testsuite_translation.keys():
                id = self.submit_testsuite(new_testsuite_obj)
                testsuite_translation[new_testsuite['id']] = id
        for testname in new_results['runs'].keys():
            if testname not in self.data['runs'].keys():
                self.data['runs'][testname] = []
            for run_info in new_results['runs'][testname]:
                self.data['runs'][testname].append({})
                self.data['runs'][testname]['compilation'] = {}
                self.data['runs'][testname]['runtime'] = {}
                self.data['runs'][testname]['testsuite_id'] = testsuite_translation[run_info['testsuite_id']]
                for key in run_info['compilation']:
                    if key == "config":
                        self.data['runs'][testname]['compilation'][key] = config_translation[run_info['compilation']['config']]
                    else:
                        self.data['runs'][testname]['compilation'][key] = run_info['compilation'][key]
                for key in run_info['runtime']:
                    if key == "system":
                        self.data['runs'][testname]['runtime'][key] = system_translation[run_info['runtime']['system']]
                    else:
                        self.data['runs'][testname]['runtime'][key] = run_info['runtime'][key]
                for key in run_info.keys():
                    if key in ['compilation', 'runtime', 'testsuite_id']: #Handling the optional CommandSet results
                        continue
                    self.data[key] = []
                    for command in run_info[key]:
                        self.data[key].append({})
                        for attribute in command.keys():
                            self.data[key][-1][attribute] = command[attribute]

    def build_summary(self):
        self.data['summary'] = {}
        for config in self.data['configs']:
            self.data['summary'][config['id']] = {}
            for system in self.data['systems']:
                self.data['summary'][config['id']][system['id']] = {}
                for testsuite in self.data['testsuites']:
                    self.data['summary'][config['id']][system['id']][testsuite['id']] = {}
                    for testname in self.data['runs']:
                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname] = {}
                        run_list = []
                        run_list_inds = []
                        export_run = None
                        export_run_ind = None
                        for run in list(range(len(self.data['runs'][testname]))):
                            if self.data['runs'][testname][run]['testsuite_id'] == testsuite['id']:
                                if self.data['runs'][testname][run]['compilation']['config'] == config['id']:
                                    if self.data['runs'][testname][run]['runtime']['system'] == system['id']:
                                        run_list.append(self.data['runs'][testname][run])
                                        run_list_inds.append(run)
                                        if self.data['runs'][testname][run]['runtime']['export'] is True:
                                            export_run = self.data['runs'][testname][run]
                                            export_run_ind = run
                        if len(run_list) == 0:
                            continue
                        for testnum in testsuite[testname]['tests'].keys():
                            exclusive_run = None
                            exclusive_run_ind = None
                            bypass_flag = "-DT" + str(testnum)
                            exclusive_run_defs = []
                            has_run = False
                            for tnum in testsuite[testname]['tests'].keys():
                                if tnum != testnum:
                                    exclusive_run_defs.append("-DT" + str(tnum))
                            for run in list(range(len(run_list))):
                                found = True
                                if bypass_flag in run_list[run]['compilation']['command'].split(' '):
                                    continue
                                has_run = True
                                for defnum in exclusive_run_defs:
                                    if defnum not in run_list[run]['compilation']['command'].split(' '):
                                        found = False
                                        break
                                if found:
                                    exclusive_run = run_list[run]
                                    exclusive_run_ind = run_list_inds[run]
                            self.data["summary"][config['id']][system['id']][testsuite['id']][testname][testnum] = {}
                            if not has_run:
                                self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]["result"] = "Excluded From Run"
                                self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['run_index'] = -1
                            elif bypass_flag not in export_run['compilation']['command']:
                                self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['run_index'] = export_run_ind
                                if export_run['compilation']['result'] != 0:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]["result"] = "Compilation Failure"
                                elif export_run['runtime']['errors'] != "":
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Error"
                                elif export_run['runtime']['result'] == 0:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                else:
                                    if int(export_run['runtime']['result'] / 2 ** (int(testnum) - 1)) % 2 == 0:
                                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                    else:
                                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Failure"
                            elif exclusive_run is not None:
                                self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['run_index'] = exclusive_run_ind
                                if exclusive_run['compilation']['result'] != 0:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Compilation Failure"
                                elif exclusive_run['runtime']['errors'] != "":
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Error"
                                elif exclusive_run['runtime']['result'] == 0:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                else:
                                    if int(exclusive_run['runtime']['result'] / 2 ** (int(testnum) - 1)) % 2 == 0:
                                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                    else:
                                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Failure"
                            else:
                                compare_run = None
                                compare_run_ind = None
                                for run in list(range(len(run_list))):
                                    if list_compare(run_list[run]['compilation']['command'].split(' ') + [bypass_flag], export_run):
                                        compare_run = run_list[run]
                                        compare_run_ind = run
                                if compare_run is None:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Unknown Section Result"
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['run_index'] = -1
                                else:
                                    self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['run_index'] = compare_run_ind
                                    if compare_run['compilation']['result'] != 0:
                                        if export_run['compilation']['result'] == compare_run['compilation']['result']:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Unknown Section Result"
                                        else:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Compilation Failure"
                                    elif compare_run['runtime']['errors'] != "":
                                        if export_run['runtime']['errors'] == compare_run['runtime']['errors']:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Unknown Section Result"
                                        else:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Error"
                                    elif compare_run['runtime']['result'] == 0:
                                        self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                    else:
                                        if int(compare_run['runtime']['result'] / 2 ** (int(testnum) - 1)) % 2 == 0:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Pass"
                                        else:
                                            self.data['summary'][config['id']][system['id']][testsuite['id']][testname][testnum]['result'] = "Runtime Failure"
        for config in self.data['configs']:
            for system in self.data['systems']:
                for testsuite in self.data['testsuites']:
                    for testname in self.data['runs']:
                        if len(self.data['summary'][config['id']][system['id']][testsuite['id']][testname]) == 0:
                            del self.data['summary'][config['id']][system['id']][testsuite['id']][testname]
                    if len(self.data['summary'][config['id']][system['id']][testsuite['id']]) == 0:
                        del self.data['summary'][config['id']][system['id']][testsuite['id']]
                if len(self.data['summary'][config['id']][system['id']]) == 0:
                    del self.data['summary'][config['id']][system['id']]
            if len(self.data['summary'][config['id']]) == 0:
                del self.data['summary'][config['id']]

class test:
    def __init__(self, path):
        self.path = path
        self.name = path.split(sep)[-1]
        [self.tags, self.versions, self.contents] = self.build_tags()
        self.should_compile = True
        self.flags = {}
        # self.flags set in self.process_tags()
        self.config_report = self.process_tags()
        self.tests = self.tags.keys()
        self.compile_attempt_count = 0
        self.executable_path = ""
        self.current_excluded_tests = []
        self.build_dir = join(g_config.build_dir, self.name)

    def comp(self, skipped_tests):
        args = []
        if isFortran(self.path):
            args.append(g_config.FC)
            for x in g_config.FCFlags:
                args.append(x)
        elif isCPP(self.path):
            args.append(g_config.CPP)
            for x in g_config.CPPFlags:
                args.append(x)
        elif isC(self.path):
            args.append(g_config.CC)
            for x in g_config.CCFlags:
                args.append(x)
        else:
            print("unknown test type: " + self.path)
            return 1

        for x in self.tests:
            if not self.flags[x]:
                args.append("-DT" + x)
        for x in skipped_tests:
            args.append("-DT" + x)
        if g_config.seed is not None:
            args.append("-DSEED=" + str(g_config.seed))
        args.append('-o')
        self.executable_path = join(g_config.build_dir, self.name, self.name + str(self.compile_attempt_count))
        args.append(self.executable_path)
        self.compile_attempt_count += 1
        args.append(self.path)
        self.run_pre_compile_commands(' '.join(args))
        [result, out, err, runtime] = g_shell.runCommand(args, self.build_dir)
        g_results.submit_compile(self.path.split(sep)[-1], args, result, out, err, runtime)
        self.run_post_compile_commands(' '.join(args), result, out, err)
        return result

    def run(self):
        if g_verbose['info']:
            print("Running: " + self.name)
        sys.stdout.flush()
        total = 0
        valid_tests = []
        if isFortran(self.path) or isCPP(self.path) or isC(self.path):
            g_results.enter_test(self)
            if not exists(join(g_config.build_dir, self.name)):
                [result, out, err, runtime] = g_shell.runCommand(["mkdir", join(g_config.build_dir, self.name)])
                if len(err) > 0 or result != 0:
                    if g_verbose['debug']:
                        print("There was an issue creating the build directory:")
                        print('mkdir ' + join(g_config.build_dir, self.name))
                        print(out + err)
                        print(result)
                    sys.stdout.flush()
            for x in self.flags.keys():
                if self.flags[x]:
                    total += 1
                    valid_tests.append(x)
            if g_config.fast:
                self.current_excluded_tests = []
                result = self.comp([])
                if result == 0:
                    self.run_pre_run_commands(0)
                    [result, out, err, runtime] = g_shell.runCommand(g_config.runtime_prefix.split(" ") + [self.executable_path], self.build_dir)
                    g_results.submit_runtime(self.name, result, out, err, runtime)
                    if g_config.keep_policy == "off" or (g_config.keep_policy == "on-error" and (result == 0 and len(err) == 0)):
                        [result, out, err, runtime] = g_shell.runCommand(['rm', self.executable_path], self.build_dir)
                        if result != 0:
                            print("Warning: Could not remove executable: " + self.executable_path)
                    self.run_post_run_commands(0, result, out, err)
                    g_results.mark(self.name)
                else:
                    g_results.submit_no_runtime(self.name)
                    g_results.mark(self.name)
            elif total < 4:
                for x in list(range(2 ** total)):
                    flag_indicator = x
                    skipped_tests = []
                    test_index = 0
                    while flag_indicator > 0:
                        if flag_indicator % 2 == 1:
                            skipped_tests.append(valid_tests[test_index])
                        flag_indicator /= 2
                        test_index += 1
                    self.current_excluded_tests = skipped_tests
                    result = self.comp(skipped_tests)
                    if result == 0:
                        self.run_pre_run_commands(self.compile_attempt_count)
                        [result, out, err, runtime] = g_shell.runCommand(g_config.runtime_prefix.split(" ") + [self.executable_path], self.build_dir)
                        g_results.submit_runtime(self.name, result, out, err, runtime)
                        if g_config.keep_policy == 'off' or (g_config.keep_policy == 'on-error' and (result == 0 and len(err) == 0)):
                            [result, out, err, runtime] = g_shell.runCommand(['rm', self.executable_path], self.build_dir)
                            if result != 0:
                                print("Warning: could not remove executable: " + self.executable_path)
                        self.run_post_run_commands(self.compile_attempt_count, result, out, err)
                        if len(err) == 0:
                            g_results.mark(self.name)
                            return
                    else:
                        g_results.submit_no_runtime(self.name)
                g_results.mark(self.name)
            else:
                comp_failing = []
                run_failing = []
                for x in list(range(-1, total)):  # -1 starts before any tests are excluded
                    skipped_tests = []
                    for y in list(range(total)):
                        if y != x:
                            skipped_tests.append(valid_tests[y])
                    self.current_excluded_tests = skipped_tests
                    result = self.comp(skipped_tests)
                    if result == 0:
                        self.run_pre_run_commands(self.compile_attempt_count)
                        [result, out, err, runtime] = g_shell.runCommand(g_config.runtime_prefix.split(" ") + [self.executable_path], self.build_dir)
                        g_results.submit_runtime(self.name, result, out, err, runtime)
                        if g_config.keep_policy == "off" or (g_config.keep_policy == "on-error" and (result == 0 and len(err) == 0)):
                            [result, out, err, runtime] = g_shell.runCommand(['rm', self.executable_path], self.build_dir)
                            if result != 0:
                                print("Warning: could not remove executable:" + self.executable_path)
                        self.run_post_run_commands(self.compile_attempt_count, result, out, err)
                        if len(err) != 0:
                            if x == -1:
                                g_results.mark(self.name)
                                return
                            run_failing.append(x)
                    else:
                        comp_failing.append(x)
                        g_results.submit_no_runtime(self.name)
                skipped_tests = []
                for x in comp_failing:
                    skipped_tests.append(valid_tests[x])
                for x in run_failing:
                    skipped_tests.append(valid_tests[x])
                self.current_excluded_tests = skipped_tests
                result = self.comp(skipped_tests)
                if result == 0:
                    self.run_pre_run_commands(self.compile_attempt_count)
                    [result, out, err, runtime] = g_shell.runCommand(g_config.runtime_prefix.split(" ") + [self.executable_path], self.build_dir)
                    g_results.submit_runtime(self.name, result, out, err, runtime)
                    if g_config.keep_policy == "off" or (g_config.keep_policy == "on-error" and (result == 0 and len(err) == 0)):
                        [result, out, err, runtime] = g_shell.runCommand(['rm', self.executable_path], self.build_dir)
                        if result != 0:
                            print("Warning: could not remove executable: " + self.executable_path)
                    self.run_post_run_commands(self.compile_attempt_count, result, out, err)
                    g_results.mark(self.name)
                    return
                else:
                    g_results.submit_no_runtime(self.name)
                    g_results.mark(self.name)

    def build_tags(self):
        fil = open(self.path)
        data = fil.readlines()
        tags = {}
        versions = {}
        regular_expression = re.compile('^#ifndef T[0-9]*')
        start_inds = {}
        end_inds = {}
        last_test_number = None
        if isFortran(self.path):
            comment_expression = re.compile("!T[0-9]*:")
        else:
            comment_expression = re.compile("//T[0-9]*:")
        for x in list(range(len(data))):
            if regular_expression.match(data[x]):
                test_number = re.search("(?<=T)[0-9]*", data[x]).group(0)
                tags[test_number] = []
                versions[test_number] = []
                if test_number not in start_inds.keys():
                    start_inds[test_number] = []
                    end_inds[test_number] = []
                start_inds[test_number].append(x)
                last_test_number = test_number
            if data[x].upper().startswith("#ENDIF"):
                end_inds[last_test_number].append(x)
        for test_no in tags.keys():
            for x in data:
                if comment_expression.match(x):
                    if isFortran(self.path):
                        re_search = str("(?<=!T" + str(test_no) + ":)(\w+|\W+)*")
                    else:
                        re_search = str("(?<=//T" + str(test_no) + ":)(\w+|\W+)*")
                    pre_parse = re.search(re_search, x)
                    if not pre_parse is None:
                        tag_list = pre_parse.group(0).split(",")
                        for y in list(range(len(tag_list))):
                            tag_list[y] = tag_list[y].strip()
                            if tag_list[y].startswith("V:"):
                                if "-" in tag_list[y]:
                                    [start_version, end_version] = tag_list[y][2:].split("-")
                                    started = False
                                    for z in OpenACCVersions:
                                        if z == start_version:
                                            versions[test_no].append(z)
                                            started = True
                                        elif started:
                                            versions[test_no].append(z)
                                        if z == end_version:
                                            break
                                else:
                                    versions[test_no].append(tag_list[y][2:])
                            else:
                                tags[test_no].append(tag_list[y])
        content = {}
        for test_number in start_inds.keys():
            content[test_number] = []
            for range_index in list(range(len(start_inds[test_number]))):
                for line in list(range(start_inds[test_number][range_index], end_inds[test_number][range_index] + 1)):
                    content[test_number].append(data[line])
        return [tags, versions, content]

    def process_tags(self):
        config_report = ""
        for x in self.tags.keys():
            is_valid = g_config.include_by_default
            if g_config.tag_evaluation is None:
                if len(g_config.include_tags) > 0:
                    found = False
                    for y in self.tags[x]:
                        if y in g_config.include_tags:
                            found = True
                    if found:
                        is_valid = True
            else:
                is_valid = g_config.tag_evaluation.eval_fast(self.tags[x])
            if not g_config.get_acc_version(self.name) is None:
                if g_config.get_acc_version(self.name) not in self.versions[x]:
                    is_valid = False
            if g_config.tag_evaluation is None:
                if len(g_config.exclude_tags) > 0 and is_valid:
                    found = False
                    for y in self.tags[x]:
                        if y in g_config.exclude_tags:
                            found = True
                    if found:
                        is_valid = False
            if self.name in g_config.exclude_tests:
                is_valid = False
            if self.name in g_config.include_tests:
                is_valid = True
            self.flags[x] = is_valid
        count = 0
        for x in self.tags.keys():
            if not self.flags[x]:
                count += 1
        if count == 0:
            config_report = "The full test was valid for execution."
        if count == len(self.tags.keys()):
            config_report = "The full test was invalid for execution and will be skipped"
            self.should_compile = False
        if count == 1:
            config_report = "The following test failed to compile: "
            for x in self.flags.keys():
                if not self.flags[x]:
                    config_report = str(config_report + str(x))
        if count > 1:
            config_report = "The following tests failed to compile: "
            for x in self.flags.keys():
                if not self.flags[x]:
                    if count != 1:
                        config_report = str(config_report + str(x) + ", ")
                    else:
                        config_report = str(config_report + " and " + str(x))
        return config_report

    def run_pre_compile_commands(self, compilation_command):
        return_codes = [None] * len(g_config.PreCompileCommands)
        outs = [None] * len(g_config.PreCompileCommands)
        errs = [None] * len(g_config.PreCompileCommands)
        processed_commands = [None] * len(g_config.PreCompileCommands)
        for x in list(range(len(g_config.PreCompileCommands))):
            command = g_config.PreCompileCommands[x]
            command = self.replace_standard_values(command)
            command = command_replace(command, "$COMPILATION_COMMAND", compilation_command)
            processed_commands[x] = command.replace("$$", "$")
            return_codes[x], outs[x], errs[x], runtime = g_shell.runCommand(processed_commands[x], self.build_dir)
        g_results.submit_command_set("pre-compile commands", self.path.split(sep)[-1], g_config.PreCompileCommands, processed_commands, return_codes, outs, errs)

    def run_post_compile_commands(self, compilation_command, res, out, err):
        return_codes = [None] * len(g_config.PostCompileCommands)
        outs = [None] * len(g_config.PostCompileCommands)
        errs = [None] * len(g_config.PostCompileCommands)
        processed_commands = [None] * len(g_config.PostCompileCommands)
        for x in list(range(len(g_config.PostCompileCommands))):
            command = g_config.PostCompileCommands[x]
            command = process_conditionals(command, res, err)
            if command is False:
                continue
            command = self.replace_standard_values(command)
            command = command_replace(command, "$COMPILATION_COMMAND", compilation_command)
            command = command_replace(command, "$RETURNCODE", res)
            command = command_replace(command, "$OUTPUT", out)
            command = command_replace(command, "$ERRORS", err)
            processed_commands[x] = command.replace("$$", "$")
            return_codes[x], outs[x], errs[x], runtime = g_shell.runCommand(processed_commands[x], self.build_dir)
        g_results.submit_command_set("post-compile commands", self.path.split(sep)[-1], g_config.PostCompileCommands, processed_commands, return_codes, outs, errs)

    def run_pre_run_commands(self, run_attempt):
        return_codes = [None] * len(g_config.PreRunCommands)
        outs = [None] * len(g_config.PreRunCommands)
        errs = [None] * len(g_config.PreRunCommands)
        processed_commands = [None] * len(g_config.PreRunCommands)
        for x in list(range(len(g_config.PreRunCommands))):
            command = g_config.PreCompileCommands[x]
            command = self.replace_standard_values(command)
            command = command_replace(command, "$RUN_ATTEMPT", run_attempt)
            processed_commands[x] = command.replace("$$", "$")
            return_codes[x], outs[x], errs[x], runtime = g_shell.runCommand(processed_commands[x], self.build_dir)
        g_results.submit_command_set("pre-run commands", self.path.split(sep)[-1], g_config.PreRunCommands, processed_commands, return_codes, outs, errs)

    def run_post_run_commands(self, run_attempt, res, out, err):
        return_codes = [None] * len(g_config.PostRunCommands)
        outs = [None] * len(g_config.PostRunCommands)
        errs = [None] * len(g_config.PostRunCommands)
        processed_commands = [None] * len(g_config.PostRunCommands)
        for x in list(range(len(g_config.PostRunCommands))):
            command = g_config.PostCompileCommands[x]
            command = process_conditionals(command, res, err)
            if command is False:
                continue
            command = self.replace_standard_values(command)
            command = command_replace(command, "$RUN_ATTEMPT", run_attempt)
            command = command_replace(command, "$RETURNCODE", res)
            command = command_replace(command, "$OUTPUT", out)
            command = command_replace(command, "$ERRORS", err)
            processed_commands[x] = command.replace("$$", "$")
            return_codes[x], outs[x], errs[x], runtime = g_shell.runCommand(processed_commands[x], self.build_dir)
        g_results.submit_command_set("post-run commands", self.path.split(sep)[-1], g_config.PostRunCommands, processed_commands, return_codes, outs, errs)

    def replace_standard_values(self, command):
        command = command_replace(command, "$CC", g_config.CC)
        command = command_replace(command, "$CPP", g_config.CPP)
        command = command_replace(command, "$FC", g_config.FC)
        if isC(self.path):
            command = command_replace(command, "$COMPILER", g_config.CC)
            command = command_replace(command, "$FLAGS", g_config.CCFlags)
            command = command_replace(command, "$TEST_DIR", g_testsuite.CTestLocation)
        elif isCPP(self.path):
            command = command_replace(command, "$COMPILER", g_config.CPP)
            command = command_replace(command, "$FLAGS", g_config.CPPFlags)
            command = command_replace(command, "$TEST_DIR", g_testsuite.CPPTestLocation)
        elif isFortran(self.path):
            command = command_replace(command, "$COMPILER", g_config.FC)
            command = command_replace(command, "$FLAGS", g_config.FCFlags)
            command = command_replace(command, "$TEST_DIR", g_testsuite.FortranTestLocation)
        command = command_replace(command, "$BUILD_DIR", join(g_config.build_dir, self.name))
        command = command_replace(command, "$EXECUTABLE_PATH", self.executable_path)
        command = command_replace(command, "$SOURCE_PATH", self.path)
        command = command_replace(command, "$TEST_NAME", self.name)
        command = command_replace(command, "$EXCLUDED_TESTS", ','.join([str(x) for x in self.current_excluded_tests]))
        command = command_replace(command, "INCLUDED_TESTS", ','.join([str(x) for x in list(set(self.tests) - set(self.current_excluded_tests))]))
        return command

    def to_run(self):
        if len(self.flags.keys()) == 0:
            return g_config.include_by_default
        for x in self.flags.keys():
            if self.flags[x]:
                return True
        return False



class TestList:
    def __init__(self):
        self.id = -1
        self.CTestLocation = None
        self.CPPTestLocation = None
        self.FortranTestLocation = None
        self.CTests = []
        self.CPPTests = []
        self.FortranTests = []
        self.CTestsToRun = []
        self.CPPTestsToRun = []
        self.FortranTestsToRun = []
        if g_config.CC != "":
            temp = []
            try:
                temp = listdir(join(g_config.test_dir, "C"))
                self.CTestLocation = join(g_config.test_dir, "C")
            except OSError:
                temp = listdir(g_config.test_dir)
                self.CTestLocation = g_config.test_dir
            for x in temp:
                if isC(x):
                    t = test(join(self.CTestLocation, x))
                    self.CTests.append(t)
        else:
            self.CTests = []
            self.CTestLocation = g_config.test_dir
            print("No C compiler provided, skipping C tests")
        if g_config.CPP != "":
            temp = []
            try:
                temp = listdir(join(g_config.test_dir, "C++"))
                self.CPPTestLocation = join(g_config.test_dir, "C++")
            except OSError:
                temp = listdir(g_config.test_dir)
                self.CPPTestLocation = g_config.test_dir
            for x in temp:
                if isCPP(x):
                    t = test(join(g_config.test_dir, "C++", x))
                    self.CPPTests.append(t)
        else:
            self.CPPTests = []
            self.CPPTestLocation = g_config.test_dir
            print("No C++ compiler provided, skipping C++ tests")
        if g_config.FC != "":
            temp = []
            try:
                temp = listdir(join(g_config.test_dir, "Fortran"))
                self.FortranTestLocation = join(g_config.test_dir, "Fortran")
            except OSError:
                temp = listdir(g_conifg.test_dir)
                self.FortranTestLocation = g_config.test_dir
            for x in temp:
                if isFortran(x):
                    t = test(join(g_config.test_dir, 'Fortran', x))
                    self.FortranTests.append(t)
        else:
            self.FortranTests = []
            self.FortranTestLocation = g_config.test_dir
            print("No Fortran compiler provided, skipping Fortran tests")
        self.count = len(self.CTests) + len(self.CPPTests) + len(self.FortranTests)
        self.sortLists()
    def filter_test_list_with_partial_results(self, partial_results):
        for test_obj in self.CTests:
            if g_config.partial:
                if not partial_results.has_complete_entry(test_obj.name):
                    self.CTestsToRun.append(test_obj)
                elif g_verbose['info']:
                    print("Found results for test: " + test_obj.name)
            else:
                self.CTestsToRun.append(test_obj)
        for test_obj in self.CPPTests:
            if g_config.partial:
                if not partial_results.has_complete_entry(test_obj.name):
                    self.CPPTestsToRun.append(test_obj)
                elif g_verbose['info']:
                    print("Found results for test: " + test_obj.name)
            else:
                self.CPPTestsToRun.append(test_obj)
        for test_obj in self.FortranTests:
            if g_config.partial:
                if not partial_results.has_complete_entry(test_obj.name):
                    self.FortranTestsToRun.append(test_obj)
                elif g_verbose['info']:
                    print("Found results for test: " + test_obj.name)
            else:
                self.FortranTestsToRun.append(test_obj)
        self.count = 0
        for test_obj in self.CTestsToRun + self.CPPTestsToRun + self.FortranTestsToRun:
            if test_obj.to_run():
                self.count += 1

    def run_test_list(self):
        loc = 0
        self.count = 0
        for test_obj in self.CTestsToRun + self.CPPTestsToRun + self.FortranTestsToRun:
            if test_obj.to_run():
                self.count += 1
        for test_obj in self.CTestsToRun + self.CPPTestsToRun + self.FortranTestsToRun:
            if not test_obj.to_run():
                if g_verbose['info']:
                    print("Skipping test: " + test_obj.name)
                continue
            loc += 1
            if g_verbose['info']:
                print("Running test " + str(loc) + " out of " + str(self.count))
            sys.stdout.flush()
            test_obj.run()

    def run_specified_list(self, test_list):
        for test in test_list:
            found = False
            for test_obj in self.CTests + self.CPPTests + self.FortranTests:
                if test == test_obj.name:
                    found = True
            if not found:
                print("Did not understand argument: " + test)
                sys.exit()
        for test in test_list:
            for test_obj in self.CTests + self.CPPTests + self.FortranTests:
                if test == test_obj.name:
                    if test_obj.to_run():
                        sys.stdout.flush()
                        test_obj.run()

    def get_test(self, testname):
        if isFortran(testname):
            for test_object in self.FortranTests:
                if test_object.name == testname:
                    return test_object
        if isCPP(testname):
            for test_object in self.CPPTests:
                if test_object.name == testname:
                    return test_object
        if isC(testname):
            for test_object in self.CTests:
                if test_object.name == testname:
                    return test_object
        return None

    def get_results(self):
        return self.results

    def get_dict_info(self):
        returned = {'id': self.id}
        for test_obj in (self.CPPTests + self.CTests + self.FortranTests):
            returned[test_obj.name] = {}
            returned[test_obj.name]["num tests"] = len(test_obj.tags)
            returned[test_obj.name]['tests'] = {}
            for test_num in test_obj.tests:
                returned[test_obj.name]['tests'][test_num] = {}
                returned[test_obj.name]['tests'][test_num]['versions'] = test_obj.versions[test_num]
                returned[test_obj.name]['tests'][test_num]['tags'] = test_obj.tags[test_num]
                returned[test_obj.name]['tests'][test_num]['content'] = ''.join(test_obj.contents[test_num])
        return returned

    def sortLists(self):
        stringList = []
        sortedTestObj = [None] * len(self.CTests)
        for x in self.CTests:
            stringList.append(x.name)
        stringList.sort()
        for x in list(range(len(self.CTests))):
            sortedTestObj[stringList.index(self.CTests[x].name)] = self.CTests[x]
        self.CTests = sortedTestObj
        sortedTestObj = [None] * len(self.CPPTests)
        stringList = []
        for x in self.CPPTests:
            stringList.append(x.name)
        stringList.sort()
        for x in list(range(len(self.CPPTests))):
            sortedTestObj[stringList.index(self.CPPTests[x].name)] = self.CPPTests[x]
        self.CPPTests = sortedTestObj
        sortedTestObj = [None] * len(self.FortranTests)
        stringList = []
        for x in self.FortranTests:
            stringList.append(x.name)
        stringList.sort()
        for x in list(range(len(self.FortranTests))):
            sortedTestObj[stringList.index(self.FortranTests[x].name)] = self.FortranTests[x]
        self.FortranTests = sortedTestObj


class system:
    def __init__(self, system_dict=None):
        if system_dict is not None:
            self.create_self_from_dict(system_dict)
            return
        self.name = g_config.system_name
        self.machine = platform.machine()
        self.node = platform.node()
        self.processor = platform.processor()
        self.release = platform.release()
        self.version = platform.version()
        self.id = -1

    def __eq__(self, other):
        if self.name is not None:
            if other['name'] is not None:
                if self.name == other['name']:
                    return True
                else:
                    return False
            else:
                return False
        if self.machine != other['machine']:
            return False
        if self.node != other['node']:
            return False
        if self.processor != other['processor']:
            return False
        if self.release != other['release']:
            return False
        if self.version != other['version']:
            return False
        return True

    def create_self_from_dict(self, system_dict):
        for key in system_dict.keys():
            setattr(self, key, system_dict[key])

    def return_dict(self):
        return {'name': self.name,
                    'machine': self.machine,
                    'node': self.node,
                    'processor': self.processor,
                    'release': self.release,
                    'version': self.version}

    def compare(self, other):
        if not isinstance(other, system):
            compared = system(system_dict=system)
        else:
            compared = other
        if not self.name is None:
            if not compared.name is None:
                if self.name == compared.name:
                    return True
                else:
                    return False
        if self.machine != compared.machine:
            return False
        if self.node != compared.node:
            return False
        if self.processor != compared.processor:
            return False
        if self.release != compared.release:
            return False
        if self.version != compared.version:
            return False
        return True

    def compare_attributes(self, other):
        if not isinstance(other, system):
            compared = system(system_dict=system)
        else:
            compared = other
        if self.machine != compared.machine:
            return False
        if self.node != compared.node:
            return False
        if self.processor != compared.processor:
            return False
        if self.release != compared.release:
            return False
        if self.version != compared.version:
            return False

    def compare_name(self, other):
        if isinstance(other, system):
            if self.name == other.name and self.name is not None:
                return True
            else:
                return False
        else:
            if 'name' in other.keys():
                if self.name == other['name'] and self.name is not None:
                    return True
                else:
                    return False
            else:
                return False


class config:
    def __init__(self, config_dict=None):
        if config_dict is not None:
            self.create_self_from_dict(config_dict)
            return
        self.config_name = ""  # type: str
        self.CC = ""  # type: str
        self.CPP = ""  # type: str
        self.FC = ""  # type: str

        self.CCFlags = []  # type: List[str]
        self.CPPFlags = []  # type: List[str]
        self.FCFlags = []  # type: List[str]

        self.CC_ACC_Version = None  # type: Optional[str]
        self.CPP_ACC_Version = None  # type: Optional[str]
        self.FC_ACC_Version = None  # type: Optional[str]

        self.include_tags = []  # type: List[str]
        self.exclude_tags = []  # type: List[str]

        self.tag_evaluation = None  # type: Optional[tag_evaluation]

        self.include_tests = []  # type: List[str]
        self.exclude_tests = []  # type: List[str]

        self.test_dir = join(dirname(realpath(__file__)), "tests", "src", "2.5")  # type: str
        self.build_dir = join(dirname(realpath(__file__)), 'build')  # type: str
        self.partial_results_dir = join(self.build_dir, "partial_results")  # type: str
        self.include_by_default = None  # type: Optional[bool]

        self.CC_ACC_Version = None  # type: Optional[str]
        self.CPP_ACC_Version = None  # type: Optional[str]
        self.FC_ACC_Version = None  # type: Optional[str]

        self.export_format = "json"  # type: str
        self.partial = False  # type: bool
        self.fast = False  # type: bool
        self.env = None
        self.seed = None # type: Optional[int]

        self.PreCompileCommands = []  # type: List[str]
        self.PostCompileCommands = []  # type: List[str]
        self.PreRunCommands = []  # type: List[str]
        self.PostRunCommands = []  # type: List[str]

        self.runtime_prefix = ""  # type: str
        self.system_name = None  # type: Optional[str]

        self.config_id = -1  # type: int
        self.timeout = 10
        # The following can also be : 'off', 'on'
        self.keep_policy = "on-error"  # type: str

    def __eq__(self, other):
        self_vars = vars(self)
        for attr in self_vars.keys():
            if attr in ['config_id', 'export_format']:
                continue
            if attr in other.keys():
                if self_vars[attr] != other[attr]:
                    return False
            else:
                return False
        return True

    def create_self_from_dict(self, dict):
        for key in dict.keys():
            setattr(self, key, dict[key])

    def finalize_config(self):
        self.eval_default_run()
        self.finalize_versions()
        if self.partial and self.export_format == "txt":
            print("Warning: Loading of partial results is unsupported since it is not possible to match configurations")
            print("         Partial results will be dumped into the output file but cannot be reloaded to continue progress")

    def get_acc_version(self, testname):
        if isC(testname):
            return self.CC_ACC_Version
        elif isCPP(testname):
            return self.CPP_ACC_Version
        elif isFortran(testname):
            return self.FC_ACC_Version
        else:
            print("Could not detect file type for " + filename)
            sys.exit()

    def __repr__(self):
        returned = "CC = " + self.CC + '\n'
        returned += "CPP = " + self.CPP + '\n'
        returned += "FC = " + self.FC + '\n'
        returned += "CCFlags = " + ', '.join(self.CCFlags) + '\n'
        returned += "CPPFlags = " + ', '.join(self.CPPFlags) + '\n'
        returned += "FCFlags = " + ', '.join(self.FCFlags) + '\n'
        returned += "Included Tags = " + ', '.join(self.include_tags) + '\n'
        returned += "Excluded Tags = " + ', '.join(self.exclude_tags) + '\n'
        returned += "Test Directory = " + self.test_dir + '\n'
        returned += "Tests included by default = " + str(self.include_by_default) + '\n'
        if self.C_ACC_Version is None:
            returned += "C_ACC_Version = NONE\n"
        else:
            returned += "C_ACC_Version = " + self.C_ACC_Version + '\n'
        if self.CPP_ACC_Version is None:
            returned += "CPP_ACC_Version = NONE\n"
        else:
            returned += "CPP_ACC_Version = " + self.CPP_ACC_Version + '\n'
        if self.FC_ACC_Version is None:
            returned += "FC_ACC_Version = NONE\n"
        else:
            returned += "FC_ACC_Version = " + self.FC_ACC_Version + '\n'
        returned += "Export Format = " + self.export_format + '\n'
        returned += "Pre-Compilation Commands = " + ', '.join(self.PreCompileCommands) + '\n'
        returned += "Post-Compilation Commands = " + ', '.join(self.PostCompileCommands) + '\n'
        returned += "Pre-Run Commands = " + ', '.join(self.PreRunCommands) + '\n'
        returned += "Post-Run Commands = " + ', '.join(self.PostRunCommands) + '\n'
        returned += "Runtime Prefix = " + self.runtime_prefix + '\n'
        returned += "System Name = " + self.system_name + '\n'
        returned += "Timeout = " + str(self.timeout)
        return returned

    def set_vendor(self, vendor):
        if vendor.upper() in ["PGI", "PGCC", "PGFORTRAN", "PGC++"]:
            self.CC = "pgcc"
            self.CPP = "pgc++"
            self.FC = "pgfortran"
            self.CCFlags = ['-acc', '-ta=tesla']
            self.CPPFlags = ['-acc', '-ta=tesla']
            self.FCFlags = ['-acc', '-ta=tesla']
        elif vendor.upper() in ['GCC', 'GNU', 'GFORTRAN', "G++"]:
            self.CC = "gcc"
            self.CPP = "g++"
            self.FC = "gfortran"
            self.CCFlags = ["-fopenacc", "-lm", "-foffload=-lm"]
            self.CPPFlags = ["-fopenacc", "-lm", "-foffload=-lm"]
            self.FCFlags = ["-fopenacc", "-lm", "-foffload=-lm", "-ffree-line-length-none"]
        else:
            print("Unrecognized compiler vendor: " + vendor)
            print("If you would like to be added to the infrastructure vendors, please submit an issue on our public repository at https://github.com/OpenACCUserGroup/OpenACCV-V")

    def eval_default_run(self):
        if self.tag_evaluation is not None:
            self.include_by_default = False
        elif len(self.include_tags) > 0:
            self.include_by_default = False
        elif not (self.CC_ACC_Version is None and self.CPP_ACC_Version is None and self.FC_ACC_Version is None):
            self.include_by_default = False
        elif len(self.exclude_tags) > 0:
            self.include_by_default = True
        elif len(self.include_tests) > 0 and len(self.exclude_tests) > 0:
            print("Cannot specify both included tests and excluded tests without including or excluding any tags")
            print("Excluded tests are being ignored and only included tests will be run")
            self.include_by_default = False
        elif len(self.include_tests) > 0:
            self.include_by_default = False
        elif len(self.exclude_tests) > 0:
            self.include_by_default = True
        else:
            self.include_by_default = True
        self.exclude_tests.append("acc_testsuite.Fh")
        self.exclude_tests.append("acc_testsuite.h")
        self.exclude_tests.append("acc_testsuite_declare.h")

    def finalize_versions(self):
        if self.CC != "":
            if self.CC_ACC_Version is None:
                self.CC_ACC_Version = self.run_version_detection("C")
        if self.CPP != "":
            if self.CPP_ACC_Version is None:
                self.CPP_ACC_Version = self.run_version_detection("CPP")
        if self.FC != "":
            if self.FC_ACC_Version is None:
                self.FC_ACC_Version = self.run_version_detection("Fortran")

    def run_version_detection(self, lang):
        local_path = sep.join(realpath(__file__).split(sep)[:-1])
        if lang == "C":
            if isfile(sep.join([local_path, "versiontest.c"])):
                test_command = [self.CC] + self.CCFlags + ['-o'] + [sep.join([local_path, "a.out"])] + [sep.join([local_path, "versiontest.c"])]
            else:
                print(sep.join([local_path, "versiontest.c"]))
                print("Could not determine OpenACC version for " + lang + ".  Did not detect versiontest.c in same directory.  Setting to default(2.7)")
                return "2.7"
        elif lang == "Fortran":
            if isfile(sep.join([local_path, "versiontest.F90"])):
                test_command = [self.FC] + self.FCFlags + ['-o'] + [sep.join([local_path, "a.out"])] + [sep.join([local_path, "versiontest.F90"])]
            else:
                print("Could not determine compiler version for " + lang + ".  Did not detect versiontest.F90 in same directory.  Setting to default(2.7)")
                return "2.7"
        elif lang in ["CPP", "C++"]:
            if isfile(sep.join([local_path, "versiontest.cpp"])):
                test_command = [self.CPP] + self.CCPFlags + ['-o'] + [sep.join([local_path, "a.out"])] + [sep.join([local_path, "versiontest.cpp"])]
            else:
                print("Could not determine compiler version for " + lang + ".  Did not detect versiontest.c in same directory.  Setting to default(2.7)")
                return "2.7"
        else:
            print("method config::run_version_detection failed.  Could not detect version")
            print("Input language: " + lang)
            sys.exit()
        if g_shell is None:
            temp_shell = shellInterface()
        else:
            temp_shell = g_shell
        compilation_results = temp_shell.runCommand(test_command)
        if compilation_results[0] != 0 or compilation_results[2] != "":
            print("Could not detect OpenACC version for " + lang + ".  Version testing file failed compilation.  Setting to default(2.7)")
            if g_verbose['info']:
                print("Version testing executable failed with return code: " + str(res))
                print(test_command)
                print(err)
            return "2.7"
        [res, out, err, runtime] = temp_shell.runCommand(join(local_path, "a.out"))
        try:
            remove(join(local_path, "a.out"))
        except OSError:
            print("Could not remove temporary version detection executable at: " + join(local_path, "./a.out"))
        if res != 0 or err != "":
            print("Could not detect OpenACC version for " + lang + ".  Version testing file failed runtime.  Setting to default(2.7)")
            if g_verbose['info']:
                print("Version testing executable failed with return code: " + str(res))
                print(err)
            return "2.7"
        print("Version detection determined compiler OpenACC Version for " + lang + " as: " + out.strip())
        return out.strip()

    def include_config(self, filename):
        global g_verbose
        try:
            fil = open(filename, 'r')
        except IOError:
            print("Could not open config file.  This could be because the script is being called out of tree or because it has been deleted")
            print("Execution will continue with default configuration")
            return
        self.config_name = filename
        data = fil.readlines()
        fil.close()
        build_dir_overwritten = False
        for x in data:
            if not (x.startswith("!") or x.startswith("#")):
                if x.startswith("Vendor:"):
                    self.set_vendor((x.split(":")[-1]).strip())
                if x.startswith("CCFlags:"):
                    self.CCFlags = self.CCFlags + (':'.join(x.split(":")[1:])).strip().split(" ")
                if x.startswith("CPPFlags:"):
                    self.CPPFlags = self.CPPFlags + (':'.join(x.split(":")[1:])).strip().split(" ")
                if x.startswith("FCFlags:"):
                    self.FCFlags = self.FCFlags + (':'.join(x.split(":")[1:])).strip().split(" ")
                if x.startswith("CC:"):
                    self.CC = x.split(":")[-1].strip()
                if x.startswith("CPP:"):
                    self.CPP = x.split(":")[-1].strip()
                if x.startswith("FC:"):
                    self.FC = x.split(":")[-1].strip()
                if x.startswith("IncludeTags:"):
                    self.include_tags = self.include_tags + x.split(":")[-1].strip().split(" ")
                if x.startswith("ExcludeTags:"):
                    self.exclude_tags = self.exclude_tags + x.split(":")[-1].strip().split(" ")
                if x.startswith("IncludeTests:"):
                    self.include_tests = self.include_tests + x.split(":")[-1].strip().split(" ")
                if x.startswith("ExcludeTests:"):
                    self.exclude_tests = self.exclude_tests + x.split(":")[-1].strip().split(" ")
                if x.startswith("TagEvaluationString:"):
                    self.tag_evaluation = tag_evaluation(x.split(":")[-1].strip())
                # COMMANDS should all be evaluated better.  (None of this spliting on space, since most commands will have arguments)
                if x.startswith("PreCompileCommands:"):
                    self.PreCompileCommands = self.PreCompileCommands + x.split(":")[-1].strip().split(" ")
                if x.startswith("PostCompileCommands:"):
                    self.PostCompileCommands = self.PostCompileCommands + x.split(":")[-1].strip().split(" ")
                if x.startswith("PreRunCommands:"):
                    self.PreRunCommands = self.PreRunCommands + x.split(":")[-1].strip().split(" ")
                if x.startswith("PostRunCommands:"):
                    self.PostRunCommands = self.PostRunCommands + x.split(":")[-1].strip().split(" ")
                if x.startswith("Once:"):
                    g_shell.runCommand(x.split(":")[-1].strip())
                if x.startswith("ResultsFormat:"):
                    self.export_format = x.split(":")[-1].strip()
                if x.startswith("TestDir:"):
                    self.test_dir = ':'.join(x.split(":")[1:]).strip()
                if x.startswith("SystemName:"):
                    self.system_name = x.split(":")[-1].strip()
                if x.startswith("BuildDir:"):
                    build_dir_overwritten = True
                    self.build_dir = ':'.join(x.split(":")[1:]).strip()
                    self.partial_results_dir = join(self.build_dir, "partial_results")
                    assert_created_directory(self.build_dir)
                    assert_created_directory(self.partial_results_dir)
                if x.startswith("ACC_Version:"):
                    v = x.split(":")[-1].strip()
                    self.CC_ACC_Version = v
                    self.CPP_ACC_Version = v
                    self.FC_ACC_Version = v
                if x.startswith("C_ACC_Version:"):
                    self.CC_ACC_Version = x.split(":")[-1].strip()
                if x.startswith("CPP_ACC_Version:"):
                    self.CPP_ACC_Version = x.split(":")[-1].strip()
                if x.startswith("Fortran_ACC_Version:"):
                    self.FC_ACC_Version = x.split(":")[-1].strip()
                if x.startswith("RuntimePrefix:"):
                    self.runtime_prefix = x.split(":")[-1].strip()
                if x.startswith("Fast:"):
                    if x.split(":")[-1].strip().upper() == "TRUE":
                        self.fast = True
                    elif x.split(":")[-1].strip().upper() == "FALSE":
                        self.fast = False
                    else:
                        print("Unrecognized option after Fast:")
                        sys.exit(1)
                if x.startswith("Timeout:"):
                    self.timeout = int(float(x.split(":")[-1]))
                if x.startswith("AllowPartial:"):
                    if x.split(":")[-1].strip().upper() == "TRUE":
                        self.partial = True
                    elif x.split(":")[-1].strip().upper() == "FALSE":
                        self.partial = False
                    else:
                        print("Unrecognized option after AllowPartial.  Should be True or False")
                        sys.exit()
                if x.startswith("Env:"):
                    filename = x.split(":")[-1].strip()
                    file_obj = open(filename, 'r')
                    self.env = jsonLoadWrapper(file_obj)
                    file_obj.close()
                if x.startswith("Seed:"):
                    temp = x.split(":")[-1].strip()
                    try:
                        self.seed = int(temp)
                    except ValueError:
                        print("Seed value must be and integer")
                        sys.exit()
                if x.upper().startswith("SHOWCOMMANDS"):
                    g_verbose['commands'] = True
                if x.upper().startswith("SHOWRESULTS"):
                    g_verbose['results'] = True
                if x.upper().startswith("SHOWERRORS"):
                    g_verbose["errors"] = True
                if x.upper().startswith("SHOWOUTPUT"):
                    g_verbose['output'] = True
                if x.upper().startswith("SHOWOSERRORS"):
                    g_verbose['oserrors'] = True
                if x.upper().startswith("SHOWINFO"):
                    g_verbose['info'] = True
                if x.upper().startswith("SHOWALL"):
                    g_verbose['commands'] = True
                    g_verbose['results'] = True
                    g_verbose['errors'] = True
                    g_verbose['output'] = True
                    g_verbose['oserrors'] = True
                    g_verbose['info'] = True
                if x.upper().startswith("SHOWDEBUG"):
                    g_verbose['debug'] = True
        if len(self.exclude_tags) > 0 or len(self.include_tags) > 0:
            if self.tag_evaluation is not None:
                print("Config file cannot use both include/exclude tags and a tag evaluation string")
                sys.exit(1)
        if not build_dir_overwritten:
            assert_created_directory(self.build_dir)
            assert_created_directory(self.partial_results_dir)

    def return_dict(self):
        returned = {'CC': self.CC, 'CPP': self.CPP, 'FC': self.FC, 'CCFlags': self.CCFlags, 'CPPFlags': self.CPPFlags,
                    'FCFlags': self.FCFlags, 'include_tags': self.include_tags, 'exclude_tags': self.exclude_tags,
                    'C_ACC_Version': self.CC_ACC_Version, 'CPP_ACC_Version': self.CPP_ACC_Version,
                    'FC_ACC_Version': self.FC_ACC_Version, 'PreCompileCommands': self.PreCompileCommands,
                    'PostCompileCommands': self.PostCompileCommands, 'PreRunCommands': self.PreRunCommands,
                    'PostRunCommands': self.PostRunCommands}
        return returned




class tag_evaluation:
    def __init__(self, tag_string):
        self.tag_order = get_all_tags(tag_string)
        self.lookup = self.recursive_build(self.tag_order, [], tag_string)

    def eval_fast(self, tags):
        temp_reference = self.lookup
        for x in self.tag_order:
            if x in tags:
                temp_reference = temp_reference[True]
            else:
                temp_reference = temp_reference[False]
        return temp_reference

    def recursive_build(self, remaining_tags, values, tag_string):
        if len(remaining_tags) == 0:
            return self.eval_slow(values, tag_string)
        else:
            lookup = {False: self.recursive_build(remaining_tags[1:], values + [False], tag_string),
                      True: self.recursive_build(remaining_tags[1:], values + [True], tag_string)}
            return lookup

    def eval_slow(self, values, tag_string):
        depth = 0
        passed_str = ""
        start = 0
        for x in list(range(len(tag_string))):
            if tag_string[x] == "(":
                if depth > 0:
                    passed_str = passed_str + tag_string[x]
                else:
                    start = x
                depth += 1
            elif tag_string[x] == ")":
                if depth == 1:
                    tag_string = tag_string[:start] + str(self.eval_slow(values, passed_str)) + tag_string[x + 1:]
                    passed_str = ""
                    depth -= 1
                depth -= 1
            else:
                if depth > 0:
                    passed_str = passed_str + tag_string[x]
        internal_tag_order = []
        internal_values = []
        for x in self.tag_order:
            internal_tag_order.append(x)
        for x in values:
            internal_values.append(x)
        for x in list(range(len(self.tag_order))):
            internal_tag_order.append("!" + self.tag_order[x])
            internal_values.append(not values[x])
        internal_tag_order.append("!False")
        internal_values.append(True)
        internal_tag_order.append("!True")
        internal_values.append(False)
        ops = []
        for x in list(range(len(internal_tag_order))):
            tag_string = tag_string.replace(internal_tag_order[x], str(internal_values[x]))
        for x in tag_string:
            if x == "|":
                ops.append("|")
            elif x == "&":
                ops.append("&")
        tag_string = tag_string.replace("|", "&")
        bools = tag_string.split("&")
        result = None
        if len(ops) == 0:
            return bo(bools[0])
        if ops[0] == "|":
            result = bo(bools[0]) or bo(bools[1])
        if ops[0] == "&":
            result = bo(bools[0]) and bo(bools[1])
        if len(ops) == 1:
            return result
        for x in list(range(1, len(ops))):
            if ops[x] == "|":
                result = result or bools[x + 1]
            if ops[x] == "&":
                result = result and bools[x + 1]
        return result

def process_conditionals(command, res, err):
    found_pass = False
    while command.strip().upper().startswith("ONPASS:") or command.strip().upper().startswith("ONERROR:") or command.strip().upper.startswith("ONFAIL:"):
        if command.strip().upper().startswith("ONPASS:"):
            if (res == 0):
                command = command[7:]
                found_pass = True
                continue
        if command.strip().upper().starswith("ONERROR:"):
            if err != "":
                command = command[8:]
                found_pass = True
                continue
        if command.strip().upper.startswith("ONFAIL:"):
            if res != 0:
                command = command[7:]
                found_pass = True
                continue
    if found_pass:
        return command
    return False

def get_all_tags(tag_string):
    round0 = tag_string.split("|")
    round1 = []
    round2 = []
    round3 = []
    final = []
    for x in list(range(len(round0))):
        round1 = round1 + round0[x].split("&")
    for x in list(range(len(round1))):
        round2 = round2 + round1[x].split("(")
    for x in list(range(len(round2))):
        round3 = round3 + round2[x].split(")")
    for x in round3:
        if not x in final and x != "":
            final.append(x.replace("!", ""))
    return final


def bo(text):
    if text == "False":
        return False
    else:
        return True


def command_replace(command, find, replace):
    matches = re.finditer(find, command)
    false_matches = re.finditer("$" + find, command)
    match_inds = []
    for x in matches:
        match_inds.append(x.start())
    false_match_inds = []
    for x in false_matches:
        false_match_inds.append(x.start())
    for ind in match_inds:
        if ind - 1 not in false_match_inds:
            command = command[0:ind] + replace + command[ind + len(find):]
    return command

def translate(data, skipDict=False):
    if isinstance(data, dict) and not skipDict:
        returned = {}
        for key in data.keys():
            returned[translate(key, skipDict=True)] = translate(data[key], skipDict=True)
        return returned
    elif isinstance(data, list):
        return[translate(key, skipDict=True) for key in data]
    elif isinstance(data, unicode):
        return data.encode('utf-8')
    else:
        return data

def jsonLoadWrapper(f):
    if sys.version_info[0] == 3:
        return json.load(f)
    else:
        return translate(json.load(f, object_hook=translate), skipDict=True)
def isFortran(testname):
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


def isC(testname):
    if testname[-2:] == ".c":
        return True
    else:
        return False


def isCPP(testname):
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

def testsuite_compare(ts1, ts2):
    if len(list(set(ts1.keys()) ^ set(ts2.keys()))) > 0:
        return False
    for key in ts1.keys():
        if key == "id":
            continue
        if not complex_compare(ts1[key], ts2[key]):
            return False
    return True

def complex_compare(obj1, obj2):
    if isinstance(obj1, list):
        if isinstance(obj2, list):
            return list_compare(obj1, obj2)
        else:
            return False
    if isinstance(obj1, dict):
        if isinstance(obj2, dict):
            if len(list(set(obj1.keys())  ^ set(obj2.keys()))) > 0:
                return False
            same = True
            for key in obj1.keys():
                same = same or complex_compare(obj1[key], obj2[key])
            return same
        else:
            return False
    return obj1 == obj2


def list_compare(list1, list2):
    if len(list1) != len(list2):
        return False
    for x in list1:
        if not x in list2:
            return False
    for x in list2:
        if not x in list1:
            return False
    return True


def run_command(command):
    if g_shell is None:
        t_shell = shellInterface()
        return t_shell.runCommand(command)
    return g_shell.runCommand(command)


def passed(results_array):
    if results_array[0] != 0 or len(results_array[2]) > 0:
        return False
    return True


def assert_created_directory(path):
    if not isdir(path):
        res = run_command("mkdir " + path)
        if not passed(res):
            print("There was an issue creating the directory: " + path)
            if g_verbose['oserrors']:
                print(res[2])
                print(res[0])
            sys.exit()

def generatePathListFromString(string):
    homedir = dirname(realpath(__file__))
    parts = string.split(',')
    curfilename = ""
    paths = []
    for part in parts:
        if curfilename != "":
            curfilename = curfilename + ',' + part
        else:
            curfilename = part
        if isfile(curfilename):
            paths.append(curfilename)
            curfilename = ""
        elif isfile(join(homedir, curfilename)):
            paths.append(join(homedir, curfilename))
            curfilename = ""
    if curfilename != "":
        print("Could not parse given config paths.  Please check to make sure they exist")
    return paths

def print_warnings():
    if sys.version_info[0] == 2 or (sys.version_info[0] == 3 and sys.version_info[1] < 3):
        print("Will not be able to terminate processing running past timeout.")
        print("To use this feature, please use python 3.3 or greater")

def export_env(outpath):
    temp_shell = shellInterface()
    current_env = temp_shell.env
    f = open(outpath, 'w')
    json.dump(current_env, f, indent=4, sort_keys=True)
    f.close()
    print("Exported current environment to: " + outpath)

def clean_build_dir(dirname):
    if isdir(dirname):
        for filename in listdir(dirname):
            if isfile(join(dirname, filename)):
                remove(join(dirname, filename))
            elif isdir(join(dirname, filename)):
                clean_build_dir(join(dirname, filename))
                rmdir(join(dirname, filename))

OpenACCVersions = ["1.0", "2.0", "2.5", "2.6", "2.7"]
g_config = None  # type: Optional[config]
g_system = None  # type: Optional[system]
g_results = None  # type: Optional[results]
g_shell = None  # type: Optional[shellInterface]
g_testsuite = None  # type: Optional[TestList]
g_subprocess_runtime = 0
g_verbose = {'commands': False, 'results': False, 'errors': False, 'output': False, 'oserrors': False, 'debug': False, 'info': False}

def main():
    start = time()
    print_warnings()
    global g_system
    global g_config
    global g_results
    global g_testsuite
    global g_shell
    infiles = []  # type: List[str]
    outfile = None  # type: Optional[str]
    config_path = []  # type: Optional[List[str]]
    temp_system_name = None  # type: str
    clean = False
    specific_tests = []
    run_configs = []
    for x in list(range(1, len(sys.argv))):
        if sys.argv[x].startswith("-c="):
            if config_path != []:
                print("Please have only one '-c=' flag.  If you want to run multiple configuations, please concatinate them as a comma deliniated list following the '-c='")
                sys.exit()
            config_path = '='.join(sys.argv[x].split("=")[1:]).strip()
            config_path = generatePathListFromString(config_path)
        elif sys.argv[x].startswith("-o="):
            if outfile is not None:
                print("Please have only one '-o= flag.  Can only output one results file.")
                sys.exit()
            outfile = '='.join(sys.argv[x].split("=")[1:]).strip()
        elif sys.argv[x].startswith("-in="):
            if infiles != []:
                print("Please have only one '-in=' flag.  If you want to merge multiple results files, please concatinate them as a comma deliniated list following the 'in='")
                sys.exit()
            infiles = ('='.join(sys.argv[x].split("=")[1:]).strip()).split(",")
        elif sys.argv[x].lower() == "verbose":
            g_verbose['commands'] = True
            g_verbose['results'] = True
            g_verbose['errors'] = True
            g_verbose['output'] = True
            g_verbose['oserrors'] = True
            g_verbose['info'] = True
        elif sys.argv[x].lower().startswith('-system='):
            temp_system_name = sys.argv[x].split('=')[-1]
        elif sys.argv[x].lower().startswith("-env="):
            export_env('='.join(sys.argv[x].split('=')[1:]))
        elif sys.argv[x].lower() == "clean":
            clean = True
        else:
            specific_tests.append(sys.argv[x])
            #print("Did not understand arg: " + sys.argv[x])
    if outfile is None and (config_path != [] or infiles != []) :
        print("Please specify an output file with the argument '-o=<output_file>'")
        sys.exit()
    for config_index in list(range(len(config_path))):
        g_config = config()
        g_config.include_config(config_path[config_index])
        g_config.finalize_config()
        if clean:
            clean_build_dir(g_config.build_dir)
            clean_build_dir(g_config.partial_results_dir)
            if not isdir(g_config.partial_results_dir):
                mkdir(g_config.partial_results_dir)
            if not isdir(g_config.build_dir):
                mkdir(g_config.build_dir)
        if temp_system_name is not None:
            if g_config.system_name is not None:
                print("Warning: system name defined in configuration.  Command line parameter overriding")
            g_config.system_name = temp_system_name
        g_system = system()
        g_testsuite = TestList()
        if config_index == 0:
            g_results = results()
        g_results.run_init()
        g_shell = shellInterface()
        if specific_tests != []:
            g_testsuite.run_specified_list(specific_tests)
        else:
            g_testsuite.run_test_list()
        run_configs.append(g_config)
    for results_filename in infiles:
        if g_results is None:
            g_config = config()
            g_system = system()
            g_testsuite = TestList()
            g_shell = shellInterface() #These are added to create an environment that results might expect when it is created
            g_results = results()
        g_results.add_results_file(results_filename)
    if g_results is not None:
        g_results.build_summary()
        g_results.output(outfile)
        for conf in run_configs:
            clean_build_dir(conf.partial_results_dir)
            clean_build_dir(conf.build_dir)
    print("Time to complete: " + str(time() - start))
    print("Time spent running/compiling: " + str(g_subprocess_runtime))

if __name__ == "__main__":
    main()
