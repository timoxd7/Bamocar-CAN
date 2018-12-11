#ifndef bamocar_can_h
#define bamocar_can_h

#include "mbed.h"
#include "bamocar-registers.h"

#define CAN_TIMEOUT 0.01 // s

#define TORQUE_MAX_PERCENT 1.00 // 100%

class M_data {
    public:
        M_data(uint8_t address, uint16_t m_data16) {
            data[0] = address;
            data[1] = m_data16 & 0xFF;
            data[2] = m_data16 >> 8;
            dataLength = 3;
        }

        M_data(uint8_t address, int16_t m_data16) {
            data[0] = address;
            data[1] = m_data16 & 0xFF;
            data[2] = m_data16 >> 8;
            dataLength = 3;
        }

        M_data(uint8_t address, uint32_t m_data32) {
            data[0] = address;
            data[1] = m_data32 & 0xFF;
            data[2] = (m_data32 >> 8) & 0xFF;
            data[3] = (m_data32 >> 16) & 0xFF;
            data[4] = (m_data32 >> 24) & 0xFF;
            dataLength = 5;
        }

        M_data(uint8_t address, int32_t m_data32) {
            data[0] = address;
            data[1] = m_data32 & 0xFF;
            data[2] = (m_data32 >> 8) & 0xFF;
            data[3] = (m_data32 >> 16) & 0xFF;
            data[4] = (m_data32 >> 24) & 0xFF;
            dataLength = 5;
        }

        M_data(uint8_t address, uint8_t requestedAddress, uint8_t interval) {
            data[0] = address;
            data[1] = requestedAddress;
            data[2] = interval;
            dataLength = 3;
        }

        uint8_t get(uint8_t position) {
            if(position >= dataLength) return 0;
            return data[position];
        }

        uint8_t length() {
            return dataLength;
        }

    protected:
        uint8_t data[5];
        uint8_t dataLength;
};

//--------------------------------------------------------------------

class Bamocar {
    public:
        Bamocar(PinName canRD, PinName canTD, int frequency = STD_BAUD_RATE)
            : _can(canRD, canTD, frequency) {
            _can.attach(callback(this, &Bamocar::_listenCAN), CAN::RxIrq);

            _rxID = STD_RX_ID;
            _txID = STD_TX_ID;
        }

        // Speed
        float getSpeed();
        bool setSpeed(int16_t speed);
        bool requestSpeed(uint8_t interval = INTVL_IMMEDIATE);

        // Set Acceleration and Deceleration for Speed control
        bool setAccel(int16_t accel);
        bool setDecel(int16_t decel);

        // Torque (most important one to control the Motor)
        float getTorque();
        bool setTorque(float torque);
        bool requestTorque(uint8_t interval = INTVL_IMMEDIATE);

        // Current (A)
        float getCurrent();
        bool requestCurrent(uint8_t interval = INTVL_IMMEDIATE);

        // Temperatures
        uint16_t getMotorTemp();
        uint16_t getControllerTemp();
        uint16_t getAirTemp();
        bool requestMotorTemp(uint8_t interval = INTVL_IMMEDIATE);
        bool requestControllerTemp(uint8_t interval = INTVL_IMMEDIATE);
        bool requestAirTemp(uint8_t interval = INTVL_IMMEDIATE);

        // Status
        uint16_t getStatus();
        bool requestStatus(uint8_t interval = INTVL_IMMEDIATE);

        // Enable
        void setSoftEnable(bool enable);
        bool getHardEnable();
        bool requestHardEnabled(uint8_t interval = INTVL_IMMEDIATE);

        // CAN IDs (of the Motor Controller)
        void setRxID(uint16_t rxID);
        void setTxID(uint16_t txID);

    protected:
        // Receive ID of motor controller
        uint16_t _rxID;
        uint16_t _txID;

        // CAN-Bus
        CAN _can;

        // Because we get data over CAN triggering an interrupt,
        // we have to save the values in this object to be red later
        struct _got {
            int16_t N_ACTUAL = 0, N_MAX = 0, TORQUE = 0;
            uint16_t READY = 0,
                     I_ACTUAL = 0, I_DEVICE = 0, I_200PC,
                     RAMP_ACC = 0, RAMP_DEC = 0,
                     TEMP_MOTOR = 0, TEMP_IGBT = 0, TEMP_AIR = 0,
                     HARD_ENABLED = 0;
            uint32_t STATUS = 0;
        } _got;

        bool _sendCAN(M_data m_data);
        bool _requestData(uint8_t dataAddress, uint8_t interval = INTVL_IMMEDIATE);
        void _listenCAN();
        bool _parseMessage(CANMessage &msg);
        int16_t _getReceived16Bit(CANMessage &msg);
        int32_t _getReceived32Bit(CANMessage &msg);
};

#endif