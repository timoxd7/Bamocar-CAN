#include "bamocar-can.h"

/**
 * ==========
 * Bamocar::startCAN
 * ==========
 */
void Bamocar::initCAN()
{
    _canReceive.frequency(_m_baudrate);
    _canTransmit.frequency(_m_baudrate);
    printf("CAN init ok!\r\n");
}

/**
 * ==========
 * Bamocar::setRxID
 * ==========
 */
void Bamocar::setRxID(uint8_t rxID){
    _m_rxID = rxID;
}

/**
 * ==========
 * Bamocar::sendCAN
 * ==========
 */
void Bamocar::sendCAN(M_data m_data)
{
    CANMessage sendMsg = CANMessage();

    for(uint8_t i = 0; i < m_data.length(); i++)
    {
        sendMsg.data[i] = m_data.get(i);
    }

    sendMsg.id = _m_rxID;
    sendMsg.len = m_data.length();
    sendMsg.format = CANStandard;
    if (_canTransmit.write(sendMsg))
    {
        printf("loop send()\r\n");
        printf("Message sent: %x %x %x\r\n", m_data.get(0), m_data.get(1), m_data.get(2));
    }
}

/**
 * ==========
 * Bamocar::listenCAN
 * ==========
 */
void Bamocar::listenCAN()
{
    CANMessage receiveMsg = CANMessage();

    if (_canReceive.read(receiveMsg))
    {
        printf("Message received: %x %x %x %x %x %x, from %d\r\n", receiveMsg.data[0], receiveMsg.data[1], receiveMsg.data[2], receiveMsg.data[3], receiveMsg.data[4], receiveMsg.data[5], receiveMsg.id);
    }
}

//----------------------------------------------------------------------------------------------

void Bamocar::getSpeed(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, REG_N_ACTUAL, interval));
}

void Bamocar::setSpeed(uint16_t speed)
{
    sendCAN(M_data(REG_N_CMD, (speed & 0xFF), (speed >> 8)));
}

void Bamocar::setAccel(uint16_t period)
{
    sendCAN(M_data(REG_RAMP_ACC, (period & 0xFF), (period >> 8)));
}

void Bamocar::setDecel(uint16_t period)
{
    sendCAN(M_data(REG_RAMP_DEC, (period & 0xFF), (period >> 8)));
}

void Bamocar::setTorque(uint16_t torque)
{
    sendCAN(M_data(REG_TORQUE_CMD, (torque & 0xFF), (torque >> 8)));
}

void Bamocar::getCurrent(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, REG_I_ACTUAL, interval));
}

void Bamocar::getCurrentDevice(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, REG_I_DEVICE, interval));
}

void Bamocar::getMotorTemp(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, REG_TEMP_MOTOR, interval));
}

void Bamocar::getStatus(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, 0xE2, interval));
}

void Bamocar::setSoftEnable(bool enable)
{
    uint8_t m_data2 = 0;

    if (enable){
        m_data2 = 0x00;
    }
    else{
        m_data2 = 0x04;
    }

    sendCAN(M_data(REG_ENABLE, m_data2, 0x00));
}

void Bamocar::getHardEnable(uint8_t interval)
{
    sendCAN(M_data(REG_REQUEST, 0xE8, interval));
}
