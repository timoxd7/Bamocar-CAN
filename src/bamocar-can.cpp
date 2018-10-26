#include "bamocar-can.h"
#include "bamocar-registers.h"
#include "mbed.h"

/**
 * ==========
 * Bamocar
 * ==========
 */
Bamocar::Bamocar()
{
    //sets rx address to bamocar controller default
    m_rxID = 0x201;
}

CAN can1(PB_8, PB_9);
CAN can2(PB_5, PB_6); // (rd, td)
CANMessage recieveMsg;
CANMessage sendMsg;

/**
 * ==========
 * Bamocar::startCAN
 * ==========
 */
void Bamocar::startCAN()
{
    can1.frequency(100000);
    can2.frequency(100000);
    printf("CAN init ok!\r\n");
}

/**
 * ==========
 * Bamocar::setRxID
 * ==========
 */
void Bamocar::setRxID(unsigned short rxID){
    m_rxID = rxID;
}

/**
 * ==========
 * Bamocar::sendCAN
 * ==========
 */
void Bamocar::sendCAN(char stmp[])
{
    sendMsg.data[0] = stmp[0];
    sendMsg.data[1] = stmp[1];
    sendMsg.data[2] = stmp[2]; 
    sendMsg.id = m_rxID;
    sendMsg.len = 0x03;
    sendMsg.format = CANStandard;
    if (can1.write(sendMsg))
    {
        printf("loop send()\r\n");
        printf("Message sent: %x %x %x\r\n", stmp[0],stmp[1], stmp[2]);
    }
}

/**
 * ==========
 * Bamocar::listenCAN
 * ==========
 */
void Bamocar::listenCAN()
{
    if (can2.read(recieveMsg))
    {
        printf("Message received: %x %x %x %x %x %x, from %d\r\n", recieveMsg.data[0], recieveMsg.data[1], recieveMsg.data[2], recieveMsg.data[3], recieveMsg.data[4], recieveMsg.data[5], recieveMsg.id);
    }
}

void Bamocar::getSpeed(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = REG_N_ACTUAL;
    m_data[2] = interval;
    sendCAN(m_data);
}
void Bamocar::setSpeed(unsigned int speed)
{
    m_data[0] = REG_N_CMD;
    m_data[1] = speed & 0xFF; 
    m_data[2] = speed >> 8;
    sendCAN(m_data);
}
void Bamocar::setAccel(unsigned int period)
{
    m_data[0] = REG_RAMP_ACC;
    m_data[1] = period & 0xFF; 
    m_data[2] = period >> 8;
    sendCAN(m_data);
}
void Bamocar::setDecel(unsigned int period)
{
    m_data[0] = REG_RAMP_DEC;
    m_data[1] = period & 0xFF; 
    m_data[2] = period >> 8;
    sendCAN(m_data);
}
void Bamocar::setTorque(unsigned int torque)
{
    m_data[0] = REG_TORQUE_CMD;
    m_data[1] = torque & 0xFF; 
    m_data[2] = torque >> 8;
    sendCAN(m_data);
}
void Bamocar::getCurrent(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = REG_I_ACTUAL;
    m_data[2] = interval;
    sendCAN(m_data);
}
void Bamocar::getCurrentDevice(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = REG_I_DEVICE;
    m_data[2] = interval;
    sendCAN(m_data);
}
void Bamocar::getMotorTemp(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = REG_TEMP_MOTOR;
    m_data[2] = interval;
    sendCAN(m_data);
}
void Bamocar::getStatus(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = 0xE2;
    m_data[2] = interval;
    sendCAN(m_data);
}
void Bamocar::setSoftEnable(bool enable)
{
    m_data[0] = REG_ENABLE;
    if (enable){
        m_data[1] = 0x00;
    }
    else{
        m_data[1] = 0x04;
    }
    m_data[2] = 0x00;
    sendCAN(m_data);
}
void Bamocar::getHardEnable(char interval)
{
    m_data[0] = REG_REQUEST;
    m_data[1] = 0xE8; //consider defining a register
    m_data[2] = interval;
    sendCAN(m_data);
}
