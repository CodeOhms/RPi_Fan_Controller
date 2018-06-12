#!/bin/bash
program_name="FanController"
echo "This script compiles and installs the $program_name program using cmake"

#Setup variables
declare -i useDefaultBuildDir=1
declare -i useDefault=1
declare -i portable=1
declare -i debug=1
declare -i dep_use_existing=1
declare -i dep_locations=1
buildDir="build"
cmakeFlags=""

#Function for flipping build switches
buildSwitch ()
{
    local prompt=$1
    local doEcho=$2
    if [ "$doEcho" == "" ]; then
        doEcho=0
    fi

    PS3="$prompt"
    select option in yes no quit
    do
        case $option in
            yes)
                if [ $doEcho == 1 ]; then
                    echo "Chose $option"
                fi
                return 0
                ;;
            no)
                if [ $doEcho == 1 ]; then
                    echo "Chose $option"
                fi
                return 1
                ;;
            quit)
                if [ $doEcho == 1 ]; then
                    echo "Chose $option"
                fi
                return 2
                ;;
            *)
                echo "Please choose yes or no"
                ;;
        esac
    done
}

query ()
{
    #$1 = prompt text
    #$2 = echo user choice (optional)

    echo
    buildSwitch "$1" $2
    local _store=$?

    #If user chose quit
    if [ $_store == 2 ]; then
        exit 0
    fi

    return $_store
}

build ()
{
    echo
    mkdir "$buildDir"

    cd "$buildDir"

    echo
    cmake $cmakeFlags ..
    query "Did cmake fail? "
    if [ $? == 0 ]; then
        exit 1
    fi

    echo
    make install
    query "Did make fail? "
    if [ $? == 0 ]; then
        exit 1
    fi
}

query "Use the default build directory name (which is $buildDir)? "
useDefaultBuildDir=$?
if [ $useDefaultBuildDir == 1 ]; then
    echo "Please type the new directory name"
    read buildDir
fi

query "Do you want to use the default options? "
useDefault=$?
if [ $useDefault != 0 ]; then
    #User said no to default settings
    query "Do you want to build portable mode? "
    portable=$?
    if [ $portable == 0 ]; then
        cmakeFlags+=" -D PORTABLE=ON "
    fi

    query "Do you want to build debug mode? "
    debug=$?
    if [ $debug == 0 ]; then
        cmakeFlags+=" -D CMAKE_BUILD_TYPE=Debug "
    fi

    query "Do you want to use existing dependency installs? "
    dep_use_existing=$?
    if [ $dep_use_existing == 0 ]; then
        cmakeFlags+=" -D DEP_USE_EXISTING=ON "
    fi

    query "Do you want to print dependency locations? "
    dep_locations=$?
    if [ $dep_locations == 0 ]; then
        cmakeFlags+=" -D DEP_LOCATIONS=ON "
    fi
fi

build

#Enable the FanController service, to start on boot
echo
systemctl enable FanController.service

#Unset functions after use
unset buildSwitch
unset query
