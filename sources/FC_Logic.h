#ifndef   FC_LOGIC_H
#define   FC_LOGIC_H

#include <vector>
#include <tuple>
#include <map>
#include <pigpio.h>
#include "Modules/Misc_Tools/Time_Tools/include/Timer.h"
#include "Modules/Misc_Tools/Time_Tools/include/Time_Tools.h"
#include "Modules/Misc_Tools/Thermal_Tools/include/Thermal.h"
#include "Modules/File_System_Tools/include/FSystem_Tools.h"
#include "Modules/Log_Tools/include/Log_Command.h"
#include "location_config.h"
#include "FC_Commands.h"
#include "FC_Log.hpp"

namespace LogT =LogTools;
namespace EData=FCErrorData;
namespace FSt  =File_System_Tools;

/*High level API*/
class FanControl
{
    FCLog& DebugLog;

    PWMSetDCycle UseFan;

    public:
        FanControl(FCLog& Log) : DebugLog(Log) { }

        bool initialise();

        bool changeDCycle( unsigned dutyCycle );
        bool changeFanState( unsigned fanPin );

};

/*Control logic*/
struct ControLogicData
{
    //Usually 31
    signed      numGPIOPins;

    signed long interval;
    //[0] milliseconds, [1] seconds, [2] minutes, [3] hours
    unsigned    intervalUnit;

//                          Fan ID (NOTE: Fan Group ID derived from index)
    std::vector<std::vector<int>> fanInfo;
////////////////////////////////////////////////////////////////
//(-1 RPi built-in sensor)
//                   Pin Number (NOTE: Sensor ID derived from index)
    std::vector<signed> sensorPins;
//NOTE: Sensor ID derived from index
//                                         Low, High , PWM percentage
//              Temp range pair[<      ,     ], PWM percentage
    std::map<int,std::vector<std::tuple<signed,signed,unsigned>>> \
      sensoRanges;
//           Sensor ID, Fan Group ID
    std::map<int,int>   fanSensors;

};

class ControLogic
{
    ControLogicData& Data;

    FCLog&                DebugLog;
    FanControl            FControl;
    Thermal::ThermalStats RPIHeat;

    int currentTemp( bool& bSuccess, int sensorPin );

    bool changeFanState( int sID, unsigned dCycle );
    bool runFan();

    public:
        ControLogic( FCLog& Log, ControLogicData& Data )
          : DebugLog(Log), Data(Data), FControl(DebugLog) { }
        bool stopFans();

        bool tempCheck();

};

namespace Setup
{
    bool readConfig( ControLogicData& Data, std::string& initError );

    ControLogic createLogic( FCLog& Log, ControLogicData& LogicData,
      bool& bSuccess, std::string& error );
}

//Call this to run automatically on interval
class AutoFan
{
    bool initialised = false;
    std::string initError;

    FCLog           DebugLog;
    ControLogicData LogicData;
    ControLogic     FanController;

    Timer           CheckTemp;

    void runCheck()
    {
        if( !FanController.tempCheck() )
            stop();
    }

    public:
        AutoFan() : FanController( Setup::createLogic(DebugLog, LogicData,
                                   initialised, initError) ) { }

        bool initialise( std::string& error )
        {
            //Check if createLogic succeeded
            if(!initialised)
            {
                error = initError;
                return initialised;
            }

            if(gpioInitialise() < 0)
            {
                error = "Pigpio library failed to start!";
                gpioTerminate();
                initialised = false;
                return initialised;
            }

            for(unsigned pin=0; pin <= LogicData.numGPIOPins; pin++)
            {
                int result = gpioSetPWMrange(pin, 100);
                if( (result == PI_BAD_USER_GPIO) ||
                  (result == PI_BAD_DUTYRANGE) )
                {
                    error =  "Pigpio library failed to change PWM range!";
                    gpioTerminate();
                    initialised = false;
                    return initialised;
                }
            }

            CheckTemp.setFunction( std::bind(&AutoFan::runCheck, this) );
            initialised = CheckTemp.setInterval(LogicData.interval);
            initialised = CheckTemp.setIntervalUnit(LogicData.intervalUnit);
            initialised = CheckTemp.setRepetitions(Timer::infinite);
            initialised = CheckTemp.init();
            if(!initialised)
                initError = "Failed to setup Timer class.";

            error = initError;
            return initialised;
        }

        void log(std::string message, std::string file,
          std::string function, unsigned errorLine, int verbosity=0)
        {
            DebugLog.report( message, file, function, errorLine, verbosity );
        }
        void log(std::string message, int verbosity=0)
        {
            DebugLog.report( message, verbosity );
        }

        void start()
        {
            if(initialised)
                CheckTemp.start(false);
        }
        void stop()
        {
            CheckTemp.stop();
            FanController.stopFans();
            gpioTerminate();
        }

};

#endif // FC_LOGIC_H
