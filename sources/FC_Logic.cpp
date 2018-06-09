#ifndef   FC_LOGIC_HPP
#define   FC_LOGIC_HPP

#include "FC_Logic.h"

bool FanControl::changeDCycle( unsigned dutyCycle )
{
    UseFan.setDutyCycle( dutyCycle );
    if( !EData::bEmpty )
    {
        DebugLog.report( EData::eMessage, EData::fileName,
          EData::functionName, EData::errorLine );
        return false;
    }

    return true;
}
bool FanControl::changeFanState( unsigned fanPin )
{
    UseFan.setOutputPin( fanPin );
    if( !EData::bEmpty )
    {
        DebugLog.report( EData::eMessage, EData::fileName,
          EData::functionName, EData::errorLine );
        return false;
    }

    UseFan.execute();
    return true;
}


int ControLogic::currentTemp( bool& bSuccess, int sensorPin )
{
    int temp_i;

    if( !RPIHeat.read( temp_i, sensorPin ) )
        bSuccess = false;

    bSuccess = true;
    return temp_i;
}

bool ControLogic::changeFanState( int sID, unsigned dCycle )
{
    if( !FControl.changeDCycle(dCycle) )
        return false;

    int fanGroupID = Data.fanSensors[sID];
    for( auto fanID : Data.fanInfo[fanGroupID] )
    {
        if( !FControl.changeFanState( fanID ) )
            return false;
    }

    return true;
}

//Returns false if error happened
bool ControLogic::runFan()
{
    int heat;
    bool bDone;
    for( int i=0; i < Data.sensoRanges.size(); i++ )
    {
        heat = currentTemp( bDone, Data.sensorPins[i] );
        if( !bDone )
        {
            DebugLog.report( fmt::format( "Failed to read " \
              "temperature sensor at pin {}!",
              (Data.sensorPins[i] == -1)? \
                "built-in sensor" : std::to_string(heat) ) );
            return false;
        }

        for( auto tempRanges : Data.sensoRanges[i] )
        {
            if( ( heat >= std::get<0>(tempRanges) )
                  &&
                ( heat <= std::get<1>(tempRanges) ) )
            {
                if( changeFanState(i, std::get<2>(tempRanges) ) )
                    return true;
                else
                    return false;
            }
        }
        //Turn fan off if no matching range
        if( changeFanState(i, 0 ) )
            return true;
        else
            return false;
    }

    return true;
}

bool ControLogic::stopFans()
{
    for( auto group : Data.fanInfo )
    {
        for( auto fanPin : group )
        {
            if( !FControl.changeDCycle(0) )
                return false;

            if( !FControl.changeFanState( fanPin ) )
                return false;
        }
    }
    return true;
}

bool ControLogic::tempCheck()
{
    if( !runFan() )
        return false;
    return true;
}

namespace Setup
{
    bool readConfig( ControLogicData& Data, std::string& initError )
    {
        FSt::ConfigTool Config(CONFIG_PATH);

        if( !Config.initialise( initError ) )
            return false;

        bool bSuccess = false;
        auto readCheck = [&] () -> void
                        {
                            if( !bSuccess )
                            {
                                initError = Config.getError;
                                throw bool {false};
                            }
                        };

        toml::Array tomlA;

        try
        {
            Data.interval = Config.get<int>(bSuccess, "interval");
            readCheck();

            Data.intervalUnit = Config.get<int>(bSuccess,
              "intervalUnit");
            readCheck();
            if( !((Data.intervalUnit >= 0) &&
                  (Data.intervalUnit <= 3)) )
            {
                initError = "intervalUnit must be 0 to 3. "\
                  "See config for more info";
                return false;
            }

            tomlA = Config.get<toml::Array>(bSuccess, "fanInfo");
            readCheck();
            std::vector<int> fGroups;
            for( auto groups : tomlA )
            {
                fGroups = groups.as<std::vector<int>>();
                for( auto fID : fGroups )
                {
                    if( !( fID >= 0) )
                    {
                        initError = "fan output pins must be unsigned";
                        return false;
                    }
                }
                Data.fanInfo.push_back(fGroups);
            }
            fGroups.clear();

            tomlA = Config.get<toml::Array>(bSuccess, "sensorPins");
            readCheck();
            for( auto pin : tomlA )
            {
                if( !( pin.as<int>() >= -1 ) )
                {
                    initError = "sensor pins must be -1 or greater, not "
                      + std::to_string(pin.as<int>())
                      + ". See config for more info";
                    return false;
                }
                Data.sensorPins.push_back(pin.as<int>());
            }

            tomlA = Config.get<toml::Array>(bSuccess, "heatParameters");
            readCheck();
            int index = 0;
            std::vector<std::tuple<signed,signed,unsigned>> ranges;
            int speed;
            int low;
            int high;
            for( auto keys : tomlA )
            {
                tomlA = Config.get<toml::Array>(bSuccess, "ranges", keys);
                readCheck();
                for( auto parameters : tomlA )
                {
                    speed = Config.get<int>(bSuccess, "speed",
                      parameters);
                    readCheck();
                    if( !((speed >= 0) && (speed <= 100)) )
                    {
                        initError = "fan speed must be between 0 to 100, not "
                          + speed;
                        return false;
                    }
                    low = Config.get<int>(bSuccess, "low",
                      parameters);
                    high = Config.get<int>(bSuccess, "high",
                      parameters);
                    readCheck();

                    ranges.push_back( std::make_tuple (
                      low, high, speed) );
                }
                if( (index +1) > Data.sensorPins.size() )
                {
                    initError = "Too many array tables for heatParameters! " \
                      "Array tables exceed the limit of Sensor IDs. " \
                      "See config for more information";
                    return false;
                }

                Data.sensoRanges[index] = ranges;
                ranges.clear();

                index++;
            }

            index = 0;
            tomlA = Config.get<toml::Array>(bSuccess, "fanSensors");
            readCheck();
            for( auto sID : tomlA )
            {
                for( auto gID : sID.as<std::vector<int>>() )
                {
                    Data.fanSensors.insert(
                      std::pair<int,int>(index, gID) );
                }
                index++;
            }

            Data.numGPIOPins = Config.get<int>(bSuccess,
              "numGPIOPins");
            readCheck();

        } catch(bool e) {
            initError = Config.getError;
            return e;
        }

        return true;
    }

    ControLogic createLogic( FCLog& Log, ControLogicData& LogicData,
      bool& bSuccess, std::string& error )
    {
        if( readConfig(LogicData, error) )
        {
            bSuccess = true;
            ControLogic tempLogic(Log, LogicData);
            return tempLogic;
        }

        bSuccess = false;
        ControLogic failed(Log, LogicData);
        return failed;
    }

}

#endif // FC_LOGIC_HPP
