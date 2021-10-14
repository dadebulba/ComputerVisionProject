# Anomaly Detection in Crowded Scenarios

## Installation
In order to install all the dependencies required to run correctly the mmAnomaly module, a bash script (setup.sh) has been developed and it is located in the principal folder. To install the dependencies, the aforementioned script has to be run with root privileges through the command “sudo ./setup.sh --install”. Note that the overall procedure may take quite a long time.

## Configure and run
In order to run the mmAnomaly module, the C++ project needs to be built. In order to achieve this, the bash script setup.sh can be exploited. The project can be built in two ways:
When “sudo ./setup.sh --install” is executed, the script will build the project automatically after having downloaded and installed all the required libraries;
Executing the command “./setup.sh --build”.

When the build is finished, an executable file “mmAnomalyExecutable” is generated. It can be run making explicit the module number that has to be executed (e.g., “./mmAnomalyExecutable 2” to run the second module). A flag “-v” or “--verbose” can be passed as the last parameter if more statistics are needed in the console output. Note that the configuration files inside the config folder need to be adapted before running the executable.

## Theory
To understand how the two modules work is suggested to take a look to the report present in the repository.
