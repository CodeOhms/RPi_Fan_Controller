#ifndef   FC_COMMANDS_H
#define   FC_COMMANDS_H

#include "FC_IO.h"
#include <memory>

//Abstract/interface class
class ICommand {
    public:
        virtual void execute() { }
};

/*Switch*/
class DeviceOn : public ICommand {
    private:
        std::shared_ptr<Switch> Device;

    public:
        DeviceOn() : Device( std::make_shared<Switch>() ) { }
        DeviceOn(unsigned outputPin) :
          Device( std::make_shared<Switch>(outputPin) ) { }

        virtual void execute() {
           Device->on();
        }
};

class DeviceOff : public ICommand {
    private:
        std::shared_ptr<Switch> Device;

    public:
        DeviceOff() : Device( std::make_shared<Switch>() ) { }
        DeviceOff(unsigned outputPin) :
          Device( std::make_shared<Switch>(outputPin) ) { }

        virtual void execute() {
            Device->off();
        }
};


/*Pulse width modulation*/
class PWMSetDCycle : public ICommand {
    private:
        std::shared_ptr<PWMDevice> PWMDev;

    public:
        PWMSetDCycle() : PWMDev( std::make_shared<PWMDevice>() ) { }
        PWMSetDCycle(unsigned outputPin, unsigned dCycle) :
          PWMDev( std::make_shared<PWMDevice>(outputPin, dCycle) ) { }

        virtual void execute() {
            PWMDev->execute();
        }

        virtual void setOutputPin(int newPin) {
            PWMDev->setOutputPin(newPin);
        }

        virtual void setDutyCycle(unsigned newDCycle) {
            PWMDev->setDutyCycle(newDCycle);
        }

};

#endif // FC_COMMANDS_H
