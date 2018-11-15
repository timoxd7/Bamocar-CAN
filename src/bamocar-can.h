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
            _can.attach(callback(this, &Bamocar::listenCAN), CAN::RxIrq);
        }

        // TODO Implement getters!!! (only sending commands at the moment, but not processing the answer)

        uint16_t getSpeed(uint8_t interval);
        bool setSpeed(uint16_t speed);

        bool setAccel(uint16_t period);
        bool setDecel(uint16_t period);

        bool setTorque(uint16_t torque);

        uint8_t getCurrent(uint8_t interval);
        uint8_t getCurrentDevice(uint8_t interval);

        uint8_t getMotorTemp(uint8_t interval);
        uint8_t getStatus(uint8_t interval);

        void setSoftEnable(bool enable);
        bool getHardEnable(uint8_t interval);

        void setRxID(uint8_t rxID);

        void listenCAN();

    protected:
        // Receive ID of motor controller
        uint8_t _m_rxID = STD_RX_ID;

        // CAN-Bus
        CAN _can;

        // Because we get data over CAN triggering an interrupt,
        // we have to save the values in this object to be red later
        struct _got {
            uint16_t speed, torque;
            uint8_t current, motorTemp, status;
        } _got;

        bool _sendCAN(M_data m_data);
};

#endif