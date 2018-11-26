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
            _got.speed = 0;
            _got.torque = 0;
            _got.current = 0;
            _got.currentDevice = 0;
            _got.motorTemp = 0;
            _got.status = 0;
            _got.hardEnabled = false;

            _rxID = STD_RX_ID;
            _txID = STD_TX_ID;
        }


        int16_t getSpeed();
        bool setSpeed(int16_t speed);
        bool requestSpeed(uint8_t interval = INTVL_IMMEDIATE);

        bool setAccel(int16_t period);
        bool setDecel(int16_t period);

        int16_t getTorque();
        bool setTorque(int16_t torque);
        bool requestTorque(uint8_t interval = INTVL_IMMEDIATE);

        uint8_t getCurrent();
        bool requestCurrent(uint8_t interval = INTVL_IMMEDIATE);
        uint8_t getCurrentDevice();
        bool requestCurrentDevice(uint8_t interval = INTVL_IMMEDIATE);

        uint8_t getMotorTemp();
        bool requestMotorTemp(uint8_t interval = INTVL_IMMEDIATE);
        uint8_t getStatus();
        bool requestStatus(uint8_t interval = INTVL_IMMEDIATE);

        void setSoftEnable(bool enable);
        bool getHardEnable();
        bool requestHardEnabled(uint8_t interval);

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
            int16_t speed, torque;
            int8_t current, currentDevice, motorTemp, status;
            bool hardEnabled;
        } _got;

        bool _sendCAN(M_data m_data);
        bool _requestData(uint8_t dataAddress, uint8_t interval = 0x0);
        void _listenCAN();
        bool _parseMessage(CANMessage &msg);
        int16_t _getReceived16Bit(CANMessage &msg);
        int32_t _getReceived32Bit(CANMessage &msg);
};

#endif