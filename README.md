# OpenACCV-V
This repository is updated with OpenACC test codes validating and verifying implementations of features and its conformance to the OpenACC specification. We will continue to add more tests. 

Here is a link to the project presentation "OpenACC 2.5 Validation Testsuite targeting multiple architectures" http://www.csm.ornl.gov/workshops/p3ma2017/documents/Chandrasekaran_OpenACCV_V2.5.pdf at the 2nd International Workshop on Performance Portable Programming Models for Accelerators (P^3MA) co-located with ISC, Germany, 2017. The paper will be published within the Springer LNCS series in a few months time as post workshop proceedings. 

This testsuite is meant to test the functionality of the OpenACC language features.  The tests are decomposed to
test each small segment of the functionality of the language as described in the OpenACC specification
V2.5.  

In order to run the suite, we have provided a Python script, infrastructure.py that can be run.  The script can be invoked with the command (once Python has been loaded into your enironment) >python infrastructure.py -c=<config_input_file> -o=<output_file>

An input configuation file with instructions can be found in the repository as well in init_config.txt.  A single, non-existent file should be given for the output and the testsuite will be run according to the settings in the configuration file.  

In addition, the infrastructure can be run with >python infrastructure.py -env=<env_output_file>.  The file generated can be passed to the script through the configuration file and can be used to save an environment for a specific run of the testsuite. 

For any further questions, please feel free to contact us Sunita Chandrasekaran - schandra@udel.edu and Kyle Friedline - utimatu@udel.edu

