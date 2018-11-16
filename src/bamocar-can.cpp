#include "bamocar-can.h"

/**
 * ==========
 * Bamocar::setRxID
 * Bamocar::setTxID
 * ==========
 */
void Bamocar::setRxID(uint8_t rxID) {
    _rxID = rxID;
}

void Bamocar::setTxID(uint8_t txID) {
    _txID = txID;
}

/**
 * ==========
 * Bamocar::sendCAN
 * ==========
 */
bool Bamocar::_sendCAN(M_data m_data) {
    CANMessage sendMsg = CANMessage();

    for(uint8_t i = 0; i < m_data.length(); i++) {
        sendMsg.data[i] = m_data.get(i);
    }

    sendMsg.id = _rxID;
    sendMsg.len = m_data.length();
    sendMsg.format = CANStandard;

    Timer canTimeoutTimer = Timer();
    canTimeoutTimer.start();
    bool msgSent = false;
    while(!msgSent && (canTimeoutTimer.read() < CAN_TIMEOUT)) {
        msgSent = _can.write(sendMsg);
    }

    return msgSent;
}

/**
 * ==========
 * Bamocar::listenCAN
 * ==========
 */
void Bamocar::listenCAN() {
    CANMessage receiveMsg = CANMessage();

    if (_can.read(receiveMsg)) {
        // TODO do something
        //printf("Message received: %x %x %x %x %x %x, from %d\r\n", receiveMsg.data[0], receiveMsg.data[1], receiveMsg.data[2], receiveMsg.data[3], receiveMsg.data[4], receiveMsg.data[5], receiveMsg.id);
    }
}

//----------------------------------------------------------------------------------------------

uint16_t Bamocar::getSpeed(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, REG_N_ACTUAL, interval));
}

bool Bamocar::setSpeed(uint16_t speed) {
    return _sendCAN(M_data(REG_N_CMD, (speed & 0xFF), (speed >> 8)));
}

bool Bamocar::setAccel(uint16_t period) {
    return _sendCAN(M_data(REG_RAMP_ACC, (period & 0xFF), (period >> 8)));
}

bool Bamocar::setDecel(uint16_t period) {
    return _sendCAN(M_data(REG_RAMP_DEC, (period & 0xFF), (period >> 8)));
}

bool Bamocar::setTorque(uint16_t torque) {
    return _sendCAN(M_data(REG_TORQUE_CMD, (torque & 0xFF), (torque >> 8)));
}

uint8_t Bamocar::getCurrent(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, REG_I_ACTUAL, interval));
}

uint8_t Bamocar::getCurrentDevice(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, REG_I_DEVICE, interval));
}

uint8_t Bamocar::getMotorTemp(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, REG_TEMP_MOTOR, interval));
}

uint8_t Bamocar::getStatus(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, 0xE2, interval));
}

void Bamocar::setSoftEnable(bool enable) {
    uint8_t m_data2 = 0;

    if (enable) {
        m_data2 = 0x00;
    } else {
        m_data2 = 0x04;
    }

    _sendCAN(M_data(REG_ENABLE, m_data2, 0x00));
}

bool Bamocar::getHardEnable(uint8_t interval) {
    _sendCAN(M_data(REG_REQUEST, 0xE8, interval));
}
