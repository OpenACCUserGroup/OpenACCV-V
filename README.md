# OpenACCV-V
This repository is updated with OpenACC test codes validating and verifying implementations of features and its conformance to the OpenACC specification. We are continuously adding and editing tests to conform to the latest version of the [OpenACC Specification](https://www.openacc.org/specification). 

## Executing program

In order to run the suite, we have provided a Python script, infrastructure.py that can be run. It is recommended to use Python 3.3 or later. Once Python has been loaded into your environment, the script can be invoked with the command  
```
python infrastructure.py -c=<config_input_file> -o=<output_file>
```

An input configuation file with instructions can be found in the repository as well in init_config.txt.  A single, non-existent file should be given for the output and the testsuite will be run according to the settings in the configuration file.  

In addition, the infrastructure can be run with
```
>python infrastructure.py -env=<env_output_file>
```
The file generated can be passed to the script through the configuration file and can be used to save an environment for a specific run of the testsuite.

## Authors

For any further questions, please feel free to contact us Sunita Chandrasekaran - schandra@udel.edu and Aaron Jarmusch - jarmusch@udel.edu

Past Authors:
Kyle Friedline

## License

This project is licensed under the BSD 3-Clause License - see the LICENSE.md file for details

## Publications

All publications relating to OpenAcc Validation Tesuite

"OpenACC 2.5 Validation Testsuite targeting multiple architectures" at the 2nd International Workshop on Performance Portable Programming Models for Accelerators (P^3MA) co-located with ISC, Germany, 2017.  Paper: https://link.springer.com/chapter/10.1007/978-3-319-67630-2_39 Presentation Slides: http://www.csm.ornl.gov/workshops/p3ma2017/documents/Chandrasekaran_OpenACCV_V2.5.pdf

"2014 IEEE International Parallel & Distributed Processing Symposium Workshops"
Paper: https://ieeexplore.ieee.org/abstract/document/6969543?casa_token=dIj3u0IMYakAAAAA:iri-F4x3Hsbd0unFKtQlsMrb40KMHLX5-6IAD64-uzWZDjEux8_8ue1QgTMMZi7uVRurnWJkHw
