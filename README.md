# RPi_Fan_Controller
A simple program to control fans connected to RPi GPIO pins.

# Configuring
In the root directory exists a configuration file named [FanController.conf](FanController.conf).
The config file uses the [TOML v0.4.0](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.4.0.md) language specifications.

## Customising configuration
Within the configuration file, each variable possesses a short description.
The main variables of interest include: `heatParameters`, `interval` and
`intervalUnit`. The first couples a fan speed - between 0 to 100 - to a range
of temperatures. These 'trigger ranges' define the temperatures each fan
requires to activate. Variables `interval` and `intervalUnit`
are combined to specify an amount of time.
For example, an `interval` of 10 at `intervalUnit` 1
translates to 10 seconds. The time produced defines how regular the
temperatures, provided by chosen sensors, should be checked against associated
'trigger ranges'. If you require any more information about configuration,
please read the [config file](FanController.conf) first before inquiring :).

# Installing
Installing this program is as easy as downloading the latest debian package
and installing it.
`apt-get` or `dpkg` are both vaild options for installing deb packages.
The packages can be [downloaded from here](https://github.com/CodeOhms/RPi_Fan_Controller/releases).
#### dpkg
```
sudo dpkg -i [deb package]
```
#### apt-get
```
sudo apt-get install [deb package]
```
# Building
## Using build script
Included in the root is a file called 'buildFC.sh'. It will require
sudo permissions to install built files, and to install the software
as a service. A systemd unit file is used to achieve this.

## Using CMake script

## Default
For convenience, third party dependencies are included.
Each dependency that needs compiling is included, pre-compiled.
By default, the CMake script uses these local versions of dependencies to
build the program. Simply run this for the default make:
```
mkdir build
cd build
cmake ..
```
And to install:
```
make install
```
You may need to use `sudo` permissions for this to work:
```
sudo make install
```
Which will install pre-compiled libraries, if they are shared.
It will also set the `RPATH` of the program to `$ORIGIN/../lib`. 
## Don't install dependencies, or set RPATH
If you don't want these to be installed, and the RPATH to be set,
use the flag `-D PORTABLE=OFF` on the CMake script:
```
cmake -D PORTABLE=OFF
```
## Debug
To build the program in debug mode use the flag `-D CMAKE_BUILD_TYPE=Debug`.
By default, this flag is set to `Release`.
```
cmake -D CMAKE_BUILD_TYPE=Debug
```
## Using existing installs of dependencies
An option exists to use existing dependency installs.
The `-D DEP_USE_EXISTING` CMake flag achieves this.
Your own dependency installs **must** be in the default install locations.
All dependencies must be installed for this option to work.
```
cmake -D DEP_USE_EXISTING=ON ..
```
## List the dependencies' file locations
To ensure the correct dependency files are chosen by the CMake script,
use the `-D DEP_LOCATIONS` flag.
```
cmake -D DEP_LOCATIONS ..
```
