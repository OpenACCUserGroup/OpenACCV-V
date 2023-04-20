<a name="readme-top"></a>

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![BSD 3-Clause License][license-shield]][license-url]

![Logo](images/OpenACCVVdarknew.png#gh-light-mode-only)
![Logo](images/OpenACCVVwhitenew.png#gh-dark-mode-only)


<!-- PROJECT LOGO -->
<br />
<div align="center" allowed_elements>

  <h3 align="center">Validation and Verification Testsuite</h3>

  <p align="center">
    The Public repository for the OpenACC Validation & Verification Testsuite.
    <br />
    <a href="https://github.com/OpenACC/validation-suite/tree/master"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://crpl.cis.udel.edu/oaccvv/results/">View Results</a>
    ·
    <a href="https://github.com/OpenACC/validation-suite/tree/master/issues">Report Bug</a>
    ·
    <a href="https://github.com/OpenACC/validation-suite/tree/master/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#members">Members</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
    <li><a href="#publications">Publications</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

This repository is updated with OpenACC test codes validating and verifying implementations of features and its conformance to the OpenACC specification. We are continuously adding and editing tests to conform to the latest version of the [OpenACC Specification](https://www.openacc.org/specification). 


What we do:
* Revealing ambiguities in the OpenACC Specification 
* Determining missing implementation of a feature
* Highlighting unmentioned restriction of a feature
* Evaluating implementations for multiple target platforms
* Identifying and reporting compiler bugs


Consult our website for more details on results and our project [OpenACC V&V Website](https://crpl.cis.udel.edu/oaccvv/).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started



### Prerequisites

Need to have the following installed on your system:

* [Python](https://www.python.org/)

* [OpenACC Compiler](https://www.openacc.org/tools)


### Installation

_In order to run the suite, we have provided a Python script, infrastructure.py that can be run. It is recommended to use Python 3.3 or later._

1. Clone the repo
   ```sh
   git clone https://github.com/OpenACCUserGroup/OpenACCV-V.git
   ```
3. Change the `init_config.txt` file to match your configuration

4. Run the script
   ```sh
   python3 infrastructure.py -c=<config_input_file> -o=<output_file>
   ```
   or
   ```sh
   python3 infrastructure.py -c=<configuration_file[,configuration_file2]> -o=<output_file> -in=<input_file[,input_file2]>  verbose -system=<system_name> -env=<environment_output>
    ```


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap

- [x] Add Initial Tests
- [x] Add Infrastructure
- [ ] Develop Example Guide
- [ ] Add More Case Features
- [ ] OpenACC Specification Version
    - [ ] 3.3
    - [ ] 3.2
    - [ ] 3.1
    - [ ] 3.0
    - [X] 2.7
    - [X] 2.5
    - [X] 2.0
    - [X] 1.0


See the [open issues](https://github.com/OpenACCUserGroup/OpenACCV-V/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

* Please send results from a run or code for particular OpenACC features

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with tags.
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Branch (`git checkout -b feature/missing`)
3. Commit your Changes (`git commit -m 'Add feature'`)
4. Push to the Branch (`git push origin feature/missing`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the BSD 3-Clause License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Aaron Jarmusch - jarmusch@udel.edu 

Sunita Chandrasekaran - schandra@udel.edu

CRPL - [https://crpl.cis.udel.edu/](https://crpl.cis.udel.edu/)

Website Link: [https://crpl.cis.udel.edu/oaccvv/results/](https://crpl.cis.udel.edu/oaccvv/results/)


<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Members

Current Student Authors:
* Aaron Jarmusch
* Aaron Liu
* Vaidhyanathan Ravichandran
* Christian Munley
* Daniel Horta
* Will Gunter
* Olive Odida

Contributors:
* Joel Denny
* Tobias Burnus
* Jeff Larkin
* Mathew Colgrove

Past Authors:
* Kyle Friedline

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Special thanks to the following organizations for their support

* [OpenACC Organization](https://www.openacc.org/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- PUBLICATIONS -->
## Publications

All publications relating to OpenAcc Validation Tesuite

* Jarmusch, A. M., Liu, A., Munley, C., Horta, D., Ravichandran, V., Denny, J., & Chandrasekaran, S. (2022). [Analysis of Validating and Verifying OpenACC Compilers 3.0 and Above.](https://arxiv.org/abs/2208.13071) Presentation given at the SC22 Conference. [SC22](https://sc22.supercomputing.org/)

* Jarmusch, A., Baker, N. & Chandrasekaran, Su. (2021). [Using IU Jetstream for OpenMP offloading and OpenACC testsuites.](content/About/Publications/_index.files/SC@!_Submission.pdf) Presentation given at the Jetstream Research Experience for Undergraduates Program Presentation Session. Also Presented at the Super Computer Conference in 2021. [SC21](https://sc21.supercomputing.org/presentation/?id=spostu106&sess=sess243)

* "OpenACC 2.5 Validation Testsuite targeting multiple architectures" at the 2nd International Workshop on Performance Portable Programming Models for Accelerators (P^3MA) co-located with ISC, Germany, 2017.  Paper: https://link.springer.com/chapter/10.1007/978-3-319-67630-2_39 Presentation Slides: http://www.csm.ornl.gov/workshops/p3ma2017/documents/Chandrasekaran_OpenACCV_V2.5.pdf

* "2014 IEEE International Parallel & Distributed Processing Symposium Workshops"
Paper: https://ieeexplore.ieee.org/abstract/document/6969543?casa_token=dIj3u0IMYakAAAAA:iri-F4x3Hsbd0unFKtQlsMrb40KMHLX5-6IAD64-uzWZDjEux8_8ue1QgTMMZi7uVRurnWJkHw

<p align="right">(<a href="#readme-top">back to top</a>)</p>




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/OpenACCUserGroup/OpenACCV-V?style=for-the-badge
[contributors-url]: https://github.com/OpenACCUserGroup/OpenACCV-V/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/OpenACCUserGroup/OpenACCV-V.svg?style=for-the-badge
[forks-url]: https://github.com/OpenACCUserGroup/OpenACCV-V/network/members
[stars-shield]: https://img.shields.io/github/stars/OpenACCUserGroup/OpenACCV-V.svg?style=for-the-badge
[stars-url]: https://github.com/OpenACCUserGroup/OpenACCV-V/stargazers
[issues-shield]: https://img.shields.io/github/issues/OpenACCUserGroup/OpenACCV-V.svg?style=for-the-badge
[issues-url]: https://github.com/OpenACCUserGroup/OpenACCV-V/issues
[license-shield]: https://img.shields.io/github/license/OpenACCUserGroup/OpenACCV-V.svg?style=for-the-badge
[license-url]: https://github.com/OpenACCUserGroup/OpenACCV-V/blob/master/LICENSE
