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
 * Bamocar::_sendCAN
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
 * Bamocar::_requestData
 * ==========
 */
bool Bamocar::_requestData(uint8_t dataAddress, uint8_t interval) {
    return _sendCAN(M_data(REG_REQUEST, dataAddress, interval));
}

/**
 * ==========
 * Bamocar::_listenCAN
 * Bamocar::_parseMessage
 * ==========
 */
void Bamocar::_listenCAN() {
    CANMessage msg = CANMessage();

    while (_can.read(msg)) {
        // To make it easyer to use the CAN-Bus for other things later too,
        // using a method for parsing the message
        _parseMessage(msg);
    }
}

bool Bamocar::_parseMessage(CANMessage &msg) {
    if (msg.id == _txID) {
        int64_t receivedData = 0;
        if (msg.len == 4) {
            receivedData = _getReceived16Bit(msg);
        } else if (msg.len == 6) {
            receivedData = _getReceived32Bit(msg);
        } else return false;

        switch (msg.data[0]) {
            case REG_N_ACTUAL:
                _got.speed = receivedData;
                break;

            case REG_TORQUE:
                _got.torque = receivedData;
                break;

            case REG_I_ACTUAL:
                _got.current = receivedData;
                break;

            case REG_I_DEVICE:
                _got.currentDevice = receivedData;
                break;

            case REG_TEMP_MOTOR:
                _got.currentDevice = receivedData;
                break;

            case REG_STATUS:
                _got.status = receivedData;
                break;

            case REG_HARD_ENABLED:
                _got.hardEnabled = receivedData;
                break;

            default:
                return false;
        }

        return true;
    }

    return false;
}

int16_t Bamocar::_getReceived16Bit(CANMessage &msg) {
    int16_t returnValue;

    returnValue = msg.data[1];
    returnValue |= (msg.data[2] << 8);

    return returnValue;
}

int32_t Bamocar::_getReceived32Bit(CANMessage &msg) {
    int16_t returnValue;

    returnValue = msg.data[1];
    returnValue |= (msg.data[2] << 8);
    returnValue |= (msg.data[3] << 16);
    returnValue |= (msg.data[4] << 24);

    return returnValue;
}

//----------------------------------------------------------------------------------------------

int16_t Bamocar::getSpeed() {
    return _got.speed;
}

bool Bamocar::setSpeed(int16_t speed) {
    return _sendCAN(M_data(REG_N_CMD, (speed & 0xFF), (speed >> 8)));
}

bool Bamocar::requestSpeed(uint8_t interval) {
    return _requestData(REG_N_ACTUAL, interval);
}


bool Bamocar::setAccel(int16_t period) {
    return _sendCAN(M_data(REG_RAMP_ACC, (period & 0xFF), (period >> 8)));
}

bool Bamocar::setDecel(int16_t period) {
    return _sendCAN(M_data(REG_RAMP_DEC, (period & 0xFF), (period >> 8)));
}


int16_t Bamocar::getTorque() {
    return _got.torque;
}

bool Bamocar::setTorque(int16_t torque) {
    return _sendCAN(M_data(REG_TORQUE, (torque & 0xFF), (torque >> 8)));
}

bool Bamocar::requestTorque(uint8_t interval) {
    return _requestData(REG_TORQUE, interval);
}


uint8_t Bamocar::getCurrent() {
    return _got.current;
}

bool Bamocar::requestCurrent(uint8_t interval) {
    return _requestData(REG_I_ACTUAL, interval);
}

uint8_t Bamocar::getCurrentDevice() {
    return _got.currentDevice;
}

bool Bamocar::requestCurrentDevice(uint8_t interval) {
    return _requestData(REG_I_DEVICE, interval);
}


uint8_t Bamocar::getMotorTemp() {
    return _got.motorTemp;
}

bool Bamocar::requestMotorTemp(uint8_t interval) {
    return _requestData(REG_TEMP_MOTOR, interval);
}

uint8_t Bamocar::getStatus() {
    return _got.status;
}

bool Bamocar::requestStatus(uint8_t interval) {
    return _requestData(REG_STATUS, interval);
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

bool Bamocar::getHardEnable() {
    return _got.hardEnabled;
}

bool Bamocar::requestHardEnabled(uint8_t interval) {
    return _requestData(REG_HARD_ENABLED, interval);
}