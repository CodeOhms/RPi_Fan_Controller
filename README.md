# RPi_Fan_Controller
A simple program to control fans connected to RPi GPIO pins.

# Building
## Using build script
Included in the root is a file called 'buildFC.sh'. It will require sudo permissions to install built files, and to install the software as a service. A systemd unit file is used to achieve this.

## Using CMake script

## Default
For convienience, third party dependencies are included. Each dependency that needs compiling is included, pre-compiled. By default, the CMake script uses these local versions of dependencies to build the program. Simply run this for the default make:
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
Which will install pre-compiled libararies, if they are shared. It will also set the `RPATH` of the program to `$ORIGIN/../lib`. 
## Don't install dependencies, or set RPATH
If you don't want these to be installed, and the RPATH to be set, use the flag `-D PORTABLE=OFF` on the CMake script:
```
cmake -D PORTABLE=OFF
```
## Debug
To build the program in debug mode use the flag `-D CMAKE_BUILD_TYPE=Debug`. By default, this flag is set to `Release`.
```
cmake -D CMAKE_BUILD_TYPE=Debug
```
## Using existing installs of dependencies
An option exists to use existing dependency installs. The `-D DEP_USE_EXISTING` CMake flag achieves this. Your own dependency installs **must** be in the default install locations. All dependencies must be installed for this option to work.
```
cmake -D DEP_USE_EXISTING=ON ..
```
## List the dependencies' file locations
To ensure the correct dependency files are chosen by the CMake script, use the `-D DEP_LOCATIONS` flag.
```
cmake -D DEP_LOCATIONS ..
```
