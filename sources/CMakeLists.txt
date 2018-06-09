#Static module libs => FSystem, Logging, Time and Thermal
add_subdirectory(Modules)

#Create location_config.h
set(Usr_Files_Dir "$ENV{HOME}/.FanController")
set(Config_Path "${Usr_Files_Dir}/FanController.conf")
set(Log_Path    "${Usr_Files_Dir}/FC_Debug.log")
configure_file(location_config.h.in
               "${CMAKE_SOURCE_DIR}/sources/location_config.h" @ONLY)

#Add source files to executable target
set(FC_Srcs FanController.cpp FC_Logic.cpp FC_IO.cpp)
add_executable(FanController
  ${FC_Srcs}
  $<TARGET_OBJECTS:FSystem> $<TARGET_OBJECTS:Logging> $<TARGET_OBJECTS:Time>
  $<TARGET_OBJECTS:Thermal>
)

#Setup libraries
if(NOT DEP_USE_EXISTING)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}")
endif()
find_library(Fmt_Dir fmt       REQUIRED)
find_library(Pigpio_Dir pigpio REQUIRED)

set(Libs ${Fmt_Dir} ${Pigpio_Dir})

    #Abort if the libraries weren't found
foreach(lib IN LISTS ${External_libs})
    if(${lib} == "${lib}-NOT_FOUND")
        message(FATAL_ERROR ${lib})
    endif()
endforeach()

set(Libs pthread ${Libs} ${FSystem_Libs} ${Logging_Libs})
target_link_libraries(FanController PRIVATE ${Libs})
if(DEP_LOCATIONS)
    message("Libs = ${Libs}\n")
endif()

#Install precompiled libs if executable should be moved around
option(PORTABLE "Install prebuilt libraries" ON)
if(PORTABLE)
    install(FILES ${Pigpio_Dir} ${FSystem_Libs} ${Logging_Libs}
      DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
    set_target_properties(FanController PROPERTIES INSTALL_RPATH
      "\$ORIGIN/../lib")
endif()

install(TARGETS FanController RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/FanController.conf"
  DESTINATION "${Usr_Files_Dir}" )
