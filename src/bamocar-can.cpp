#include "bamocar-can.h"

/**
 * ==========
 * Bamocar::setRxID
 * Bamocar::setTxID
 * ==========
 */
void Bamocar::setRxID(uint16_t rxID) {
    _rxID = rxID;
}

void Bamocar::setTxID(uint16_t txID) {
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
            case REG_STATUS:
                _got.STATUS = receivedData;
                break;

            case REG_READY:
                _got.READY = receivedData;
                break;

            case REG_N_ACTUAL:
                _got.N_ACTUAL = receivedData;
                break;

            case REG_N_MAX:
                _got.N_MAX = receivedData;
                break;

            case REG_I_ACTUAL:
                _got.I_ACTUAL = receivedData;
                break;

            case REG_I_DEVICE:
                _got.I_DEVICE = receivedData;
                break;

            case REG_I_200PC:
                _got.I_200PC = receivedData;
                break;

            case REG_TORQUE:
                _got.TORQUE = receivedData;
                break;

            case REG_RAMP_ACC:
                _got.RAMP_ACC = receivedData;
                break;

            case REG_RAMP_DEC:
                _got.RAMP_DEC = receivedData;
                break;

            case REG_TEMP_MOTOR:
                _got.TEMP_MOTOR = receivedData;
                break;

            case REG_TEMP_IGBT:
                _got.TEMP_IGBT = receivedData;
                break;

            case REG_TEMP_AIR:
                _got.TEMP_AIR = receivedData;
                break;

            case REG_HARD_ENABLED:
                _got.HARD_ENABLED = receivedData;
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

// Speed
float Bamocar::getSpeed() {
    return _got.N_MAX * (_got.N_ACTUAL / 32767);
}

bool Bamocar::setSpeed(int16_t speed) {
    return _sendCAN(M_data(REG_N_CMD, (int16_t)speed));
}

bool Bamocar::requestSpeed(uint8_t interval) {
    bool success = true;
    if (!_requestData(REG_N_ACTUAL, interval))
        success = false;

    if (!_requestData(REG_N_MAX, interval))
        success = false;

    return success;
}

// Accel
bool Bamocar::setAccel(int16_t accel) {
    return _sendCAN(M_data(REG_RAMP_ACC, (int16_t)accel));
}

bool Bamocar::setDecel(int16_t decel) {
    return _sendCAN(M_data(REG_RAMP_DEC, (int16_t)decel));
}

// Torque
float Bamocar::getTorque() {
    return _got.TORQUE/0x5555;
}

bool Bamocar::setTorque(float torque) {
    if (torque > TORQUE_MAX_PERCENT)
        torque = TORQUE_MAX_PERCENT;

    int16_t torque16 = torque * 0x5555;
    return _sendCAN(M_data(REG_TORQUE, (int16_t)torque16));
}

bool Bamocar::requestTorque(uint8_t interval) {
    return _requestData(REG_TORQUE, interval);
}

// Current
float Bamocar::getCurrent() {
    return ((2/10) * _got.I_DEVICE * (_got.I_ACTUAL / _got.I_200PC));
}

bool Bamocar::requestCurrent(uint8_t interval) {
    bool success = true;
    if (!_requestData(REG_I_ACTUAL, interval))
        success = false;

    if (!_requestData(REG_I_DEVICE, interval))
        success = false;

    if (!_requestData(REG_I_200PC, interval))
        success = false;

    return success;
}

// Temperatures
uint16_t Bamocar::getMotorTemp() {
    return _got.TEMP_MOTOR;
}

bool Bamocar::requestMotorTemp(uint8_t interval) {
    return _requestData(REG_TEMP_MOTOR, interval);
}

uint16_t Bamocar::getControllerTemp() {
    return _got.TEMP_IGBT;
}

bool Bamocar::requestControllerTemp(uint8_t interval) {
    return _requestData(REG_TEMP_IGBT, interval);
}

uint16_t Bamocar::getAirTemp() {
    return _got.TEMP_AIR;
}

bool Bamocar::requestAirTemp(uint8_t interval) {
    return _requestData(REG_TEMP_AIR, interval);
}

// Status
uint16_t Bamocar::getStatus() {
    return _got.STATUS;
}

bool Bamocar::requestStatus(uint8_t interval) {
    return _requestData(REG_STATUS, interval);
}

// Enable
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
    return _got.HARD_ENABLED;
}

bool Bamocar::requestHardEnabled(uint8_t interval) {
    return _requestData(REG_HARD_ENABLED, interval);
}