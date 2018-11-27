# Bamocar-CAN - CAN Interface for Bamocar Motor Controller

## Functionality
The Microcontroller can communicate over CAN with the Motor Controller. To send Values, the first Byte of the CAN Message has to be the Register Address which should be set. To Read a Value from a Register, the first Byte has to be the "Request"-Byte, followed by the wanted Address of the wanted Register. There can also be spezifyed an interval in which the given Register-Value should be sent.

To easily access these Registers, this library abstracts the Registers and CAN-Bus. The got Values are stored in Variables and if a value is set, it is simultaniously sent over CAN to the Controller.

## Usage
To get a Value, a getter is given (for example: getSpeed()). This returns the last got Value stored on the Microcontroller.

To request a Value (e.g. update it), a request method is given (for example: requestSpeed()). After that, the Motor Controller will send the current Value once.
To request a Value repeadly, an interval can be spezifyed too (for example: requestSpeed(10) -> the Speed will be sent every 10ms from the Motor Controller).
Both will return true if the Message was Sent successfully to the Motor Controller

To set a Value, a setter is given (for example: setSpeed()). This returns true, if the Message was Sent successfully to the Motor Controller. Not every Value can be set (like Current...).

Almost all Values/Register Addresses follow this interface. There are some exceptions, please see bamocar-can.h for more information.

## Registers
### Speed ("Tempomat")
    int16_t Speed

### Torque (applied to the Motor)
    int16_t Torque

### Current currently running throu the Motor (A)
    uint8_t Current
    uint8_t CurrentDevice

### Get Temperature of the Motor
    uint8_t MotorTemp
    uint8_t ControllerTemp
    uint8_t AirTemp

### Status of the Motor
    uint8_t Status

### Some Control Variables
    void setSoftEnable(bool enable);
    bool getHardEnable();
    void setRxID(uint8_t rxID);
    void setTxID(uint8_t txID)