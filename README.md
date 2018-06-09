# RPi_Fan_Controller
A simple program to control fans connected to RPi GPIO pins.

# Building
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
Currently, the CMake script doesn't have an option to build using existing dependency installs. However, simply renaming the directories called 'lib' and 'include' changes where the script searches. Your own dependency installs **must** be in the default install locations. All dependencies must be installed for this workaround to work. An option will be added in the future to use existing dependecy installs.
