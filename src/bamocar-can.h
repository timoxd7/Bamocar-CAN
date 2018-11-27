#ifndef bamocar_can_h
#define bamocar_can_h

#include "mbed.h"
#include "bamocar-registers.h"

#define CAN_TIMEOUT 0.01 // s

class M_data {
    public:
        M_data(uint8_t m_data1) {
            data[0] = m_data1;
            dataLength = 1;
        }

        M_data(uint8_t m_data1, uint8_t m_data2) {
            data[0] = m_data1;
            data[1] = m_data2;
            dataLength = 2;
        }

        M_data(uint8_t m_data1, uint8_t m_data2, uint8_t m_data3) {
            data[0] = m_data1;
            data[1] = m_data2;
            data[2] = m_data3;
            dataLength = 3;
        }

        M_data(uint8_t m_data1, uint8_t m_data2, uint8_t m_data3, uint8_t m_data4) {
            data[0] = m_data1;
            data[1] = m_data2;
            data[2] = m_data3;
            data[3] = m_data4;
            dataLength = 4;
        }

        uint8_t get(uint8_t position) {
            if(position >= dataLength) return 0;
            return data[position];
        }

        uint8_t length() {
            return dataLength;
        }

    protected:
        uint8_t data[4];
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
        int16_t getSpeed();
        bool setSpeed(int16_t speed);
        bool requestSpeed(uint8_t interval = INTVL_IMMEDIATE);

        // Still don't know what these make
        bool setAccel(int16_t period);
        bool setDecel(int16_t period);

        // Torque (most important one to control the Motor)
        int16_t getTorque();
        bool setTorque(int16_t torque);
        bool requestTorque(uint8_t interval = INTVL_IMMEDIATE);

        // Current (A)
        uint8_t getCurrent();
        uint8_t getCurrentDevice();
        bool requestCurrent(uint8_t interval = INTVL_IMMEDIATE);
        bool requestCurrentDevice(uint8_t interval = INTVL_IMMEDIATE);

        // Temperatures
        uint8_t getMotorTemp();
        uint8_t getControllerTemp();
        uint8_t getAirTemp();
        bool requestMotorTemp(uint8_t interval = INTVL_IMMEDIATE);
        bool requestControllerTemp(uint8_t interval = INTVL_IMMEDIATE);
        bool requestAirTemp(uint8_t interval = INTVL_IMMEDIATE);

        // Status
        uint8_t getStatus();
        bool requestStatus(uint8_t interval = INTVL_IMMEDIATE);

        // Enable
        void setSoftEnable(bool enable);
        bool getHardEnable();
        bool requestHardEnabled(uint8_t interval);

        // CAN IDs (of the Motor Controller)
        void setRxID(uint8_t rxID);
        void setTxID(uint8_t txID);

    protected:
        // Receive ID of motor controller
        uint8_t _rxID;
        uint8_t _txID;

        // CAN-Bus
        CAN _can;

        // Because we get data over CAN triggering an interrupt,
        // we have to save the values in this object to be red later
        struct _got {
            int16_t speed = 0, torque = 0;
            int8_t current = 0, currentDevice = 0,
                   motorTemp = 0, controllerTemp = 0, airTemp = 0,
                   status = 0;
            bool hardEnabled = false;
        } _got;

        bool _sendCAN(M_data m_data);
        bool _requestData(uint8_t dataAddress, uint8_t interval = 0x0);
        void _listenCAN();
        bool _parseMessage(CANMessage &msg);
        int16_t _getReceived16Bit(CANMessage &msg);
        int32_t _getReceived32Bit(CANMessage &msg);
};

#endif