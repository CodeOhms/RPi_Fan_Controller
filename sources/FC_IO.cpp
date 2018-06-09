/*Receiver code*/

#include "FC_IO.h"

//Error handling
namespace EData=FCErrorData;
namespace FCErrorData {
    std::string eMessage, fileName, functionName = "";
    unsigned errorLine, verbosityLevel = 0;
    bool bEmpty = true;

    void ERRIOWrite( unsigned pin, int errorCode, unsigned errorLine,
      std::string function ) {
        EData::clear();

        EData::eMessage = fmt::format("Failed to change pin {} level: {}", pin,
          (errorCode == PI_BAD_GPIO) ? "PI_BAD_GPIO"  : "PI_BAD_LEVEL");

        EData::fileName = __FILE__;
        EData::functionName = function;
        EData::errorLine = errorLine;
        EData::bEmpty = false;

    }

    void ERRPwm( unsigned pin, unsigned errorLine, std::string function ) {
        EData::clear();

        EData::eMessage = fmt::format("Failed to change pin {} PWM level", pin);
        EData::fileName = __FILE__;
        EData::functionName = function;
        EData::errorLine = errorLine;
        EData::bEmpty = false;

    }

    void clear() {
        EData::eMessage, EData::fileName, EData::functionName = "";
        EData::errorLine, EData::verbosityLevel = 0;
        EData::bEmpty = true;

    }

}


/*Fan*/
void Switch::on() {
    if( gpioRead(pin) == 1 )
        return;

    int error = gpioWrite(pin, 1);
    if( error != 0)
        EData::ERRIOWrite(pin, error, __LINE__, __FUNCTION__);

}

void Switch::off() {
    if( gpioRead(pin) == 0 )
        return;

    int error = gpioWrite(pin, 0);
    if( error != 0)
        EData::ERRIOWrite(pin, error, __LINE__, __FUNCTION__);

}


/*PWM*/
void PWMDevice::setOutputPin(unsigned newPin) {
    pin = newPin;
}

void PWMDevice::setDutyCycle(unsigned newDCycle) {
    dCycle = newDCycle;
}

void PWMDevice::execute() {
    if( gpioPWM(pin, dCycle) != 0)
        EData::ERRPwm(pin, __LINE__, __FUNCTION__);
}
