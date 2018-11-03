#ifndef bamocar_can_h
#define bamocar_can_h

#include "mbed.h"
#include "bamocar-can.h"
#include "bamocar-registers.h"

class M_data 
{
  public:
    M_data(uint8_t m_data1)
    {
      data[0] = m_data1;
      dataLength = 1;
    }

    M_data(uint8_t m_data1, uint8_t m_data2)
    {
      data[0] = m_data1;
      data[1] = m_data2;
      dataLength = 2;
    }

    M_data(uint8_t m_data1, uint8_t m_data2, uint8_t m_data3)
    {
      data[0] = m_data1;
      data[1] = m_data2;
      data[2] = m_data3;
      dataLength = 3;
    }

    M_data(uint8_t m_data1, uint8_t m_data2, uint8_t m_data3, uint8_t m_data4)
    {
      data[0] = m_data1;
      data[1] = m_data2;
      data[2] = m_data3;
      data[3] = m_data4;
      dataLength = 4;
    }

    uint8_t get(uint8_t position)
    {
      if(position >= dataLength) return 0;
      return data[position];
    }

    uint8_t length()
    {
      return dataLength;
    }

  protected:
    uint8_t data[4];
    uint8_t dataLength;
};

//--------------------------------------------------------------------

class Bamocar
{
public:
  Bamocar(CAN can)
    : _canReceive(can), _canTransmit(can) {}

  Bamocar(CAN canReceive, CAN canTransmit)
    : _canReceive(canReceive), _canTransmit(canTransmit) {}

  void initCAN();

  void getSpeed(uint8_t interval);
  void setSpeed(uint16_t speed);
  void setAccel(uint16_t period);
  void setDecel(uint16_t period);
  void setTorque(uint16_t torque);
  void getCurrent(uint8_t interval);
  void getCurrentDevice(uint8_t interval);
  void getMotorTemp(uint8_t interval);
  void getStatus(uint8_t interval);
  void setSoftEnable(bool enable);
  void getHardEnable(uint8_t interval);
  void setRxID(uint8_t rxID);

  void listenCAN();

protected:
  /**
          * Receive ID of motor controller
        */
  uint8_t _m_rxID = STD_RX_ID;

  /**
          * Buadrate of motor controller
        */
  uint16_t _m_baudrate = STD_BAUD_RATE;

  /**
          * CAN-Bus
        */
  CAN &_canReceive;
  CAN &_canTransmit;

  void sendCAN(M_data m_data);
};

#endif