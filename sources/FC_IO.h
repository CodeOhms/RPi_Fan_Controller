#ifndef   FC_IO_H
#define   FC_IO_H

#include <string>
#include <fmt/format.h>
#include <pigpio.h>

//Error handling
namespace FCErrorData {
    extern std::string eMessage, fileName, functionName;
    extern unsigned errorLine, verbosityLevel;
    extern bool bEmpty;

    void clear();

    void ERRIOWrite( unsigned pin, int errorCode, unsigned errorLine,
      std::string function );

    void ERRPwm( unsigned pin, unsigned errorLine, std::string function );

}

//Abstract/interface class
class BasicDevice {
    public:
        virtual void on() {}
        virtual void off() {}

};

//Abstract/interface class
class PWMDeviceBase {
    public:
        virtual void execute() {}

};

class PWMDevice : public PWMDeviceBase {
    private:
        unsigned pin;
        unsigned dCycle;

    public:
        PWMDevice() : pin(0), dCycle(0) { }
        PWMDevice(unsigned pin, unsigned dCycle) : pin(pin),
          dCycle(dCycle) { }

        virtual void setOutputPin(unsigned newPin);

        virtual void setDutyCycle(unsigned newDCycle);
        virtual void execute();

};

class Switch : public BasicDevice {
    private:
        unsigned pin;

    public:
        Switch() : pin(0) { }
        Switch(unsigned outputPin) : pin(outputPin) { }

        virtual void on();
        virtual void off();

};

#endif // FC_IO_H
