# Bamocar-CAN
## CAN Interface for Motor Controller

### Bamocar Object
    Bamocar motorController();

### Initialize library
    void startCAN();

### Get and Set Speed ("Tempomat")
    void getSpeed(char interval);
    void setSpeed(unsigned int speed);

### Set Acceleration for Motorcontroller (Not Acceleration of the whole car, control variable!)
    void setAccel(unsigned int period);
    void setDecel(unsigned int period);

### Set Torque (Actual Torque of the Motor -> this should be set by the Gas Pedal)
    void setTorque(unsigned int torque);

### Get Current currently running throu the Motor
    void getCurrent(char interval);
    void getCurrentDevice(char interval);

### Get Temperature of the Motor
    void getMotorTemp(char interval);

### Get Status of the Motor
    void getStatus(char interval);

### Some Control Variables
    void setSoftEnable(bool enable);
    void getHardEnable(char interval);
    void setRxID(unsigned short rxID);

### Not needed if using own CAN-Service
void listenCAN();