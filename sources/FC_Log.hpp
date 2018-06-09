#ifndef   FC_LOG_HPP
#define   FC_LOG_HPP

#include <string>
#include "Modules/Log_Tools/include/Log_Command.h"
#include "Modules/Misc_Tools/Time_Tools/include/Time_Tools.h"
#include "location_config.h"
#include "FC_IO.h"

namespace LogT=LogTools;
class FCLog
{
    LogT::FileLogger FLog{LOG_PATH, "", std::ios::app};
    std::vector<LogT::LogData*> logTools = { &FLog };
    LogT::BasicLogging Loggers{ logTools };

    bool bFirstEntry = true;
    void firstRun(std::string& message)
    {
        if(bFirstEntry)
        {
            bFirstEntry = false;

            message = "################\n"
              + Time_Tools::getDate(1) + "\n"
              + message;
        }
    }

    public:
        void report(std::string message, std::string file,
          std::string function, unsigned errorLine, int verbosity=0)
        {
            firstRun(message);
            Loggers.report( message, file, function, errorLine, verbosity );
        }
        void report(std::string message, int verbosity=0)
        {
            firstRun(message);
            Loggers.report(message, verbosity);
        }

};

#endif // FC_LOG_HPP
