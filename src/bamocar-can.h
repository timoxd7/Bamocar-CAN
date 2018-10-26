#ifndef bamocar_can_h
#define bamocar_can_h

class Bamocar
{
public:
  Bamocar();

  void startCAN();

  void getSpeed(char interval);
  void setSpeed(unsigned int speed);
  void setAccel(unsigned int period);
  void setDecel(unsigned int period);
  void setTorque(unsigned int torque);
  void getCurrent(char interval);
  void getCurrentDevice(char interval);
  void getMotorTemp(char interval);
  void getStatus(char interval);
  void setSoftEnable(bool enable);
  void getHardEnable(char interval);
  void setRxID(unsigned short rxID);

  void listenCAN();

private:
  /**
          * Receive ID of motor controller
        */
  unsigned short m_rxID;
  /**
          * Buadrate of motor controller
        */
  char m_baudrate;

  char m_data[3];

  void sendCAN(char stmp[]);
};

#endif