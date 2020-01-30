#ifndef E32LORA_H_
#define E32LORA_H_
#include <Arduino.h>

// OPERATING MODE 
// NOTICE THAT SLEEP MODE CAN BE USED FOR SETTING COMMAND

#define to_uint8_t(x) static_cast<uint8_t>(x)
namespace SPEED {
    // AIR DATA RATE 
    // 2 1 0
    // 0 0 0
    enum class AIR_DATA_RATE : uint8_t {
        s0p3K   = 0b000,
        sp2K    = 0b001,
        s2p4K   = 0b010,
        s4p8K   = 0b011,
        s9p6k   = 0b100,
        s19p2k  = 0b101,
    };

    // TTL UART BAUDRATE
    // 5 4 3
    // 0 0 0
    enum class TTL_UART_BAUDRATE : uint8_t {
        B1200   = (0b000 << 3),
        B2400   = (0b001 << 3),
        B4800   = (0b010 << 3),
        B9600   = (0b011 << 3),
        B19200  = (0b100 << 3),
        B34800  = (0b101 << 3),
        B57600  = (0b110 << 3),
        B115200 = (0b111 << 3)
    };

    // UART PARITY BIT
    // 7 6
    // 0 0
    enum class UART_PARITY_BIT : uint8_t {
        P8N1 = (0b00 << 6),
        P8O1 = (0b01 << 6),
        P8E1 = (0b10 << 6),
    };

}

namespace OPTION {
    // 1 0
    // 0 0
    enum class TRANSMIT_POWER : uint8_t {
        dBm20 = 0b00,
        dBm17 = 0b01,
        dBm14 = 0b10,
        dBm10 = 0b11,
    };

    // 2
    // 0
    enum class FEC_SWITCH : uint8_t {
        OFF_FEC = (0b0 << 3),
        ON_FEC  = (0b1 << 3)
    };

    // 5 4 3
    // 0 0 0 
    enum class WIRELESS_WAKEUP_TIME : uint8_t {
        T250MS  = (0b000 << 4),
        T500MS  = (0b001 << 4),
        T750MS  = (0b010 << 4),
        T1000MS = (0b011 << 4),
        T1250MS = (0b100 << 4),
        T1500MS = (0b101 << 4),
        T1750MS = (0b110 << 4),
        T2000MS = (0b111 << 4)
    };

    // 6
    // 0
    enum class IO_DRIVE_MODE : uint8_t {
        PUSH_PULL_UP = (0b0 << 5),
        OPEN_COLLECTER = (0b1 << 5)
    };
}


enum class OPERATING_MODE {
    NORMAL,
    WAKE_UP,
    POWER_SAVING,
    SLEEP
};



class E32Lora {
private:
    uint8_t _m0;
    uint8_t _m1;
    uint8_t _aux;
    Stream *lora;
public:
    E32Lora(uint8_t m0, uint8_t m1, uint8_t aux) : _m0(m0), _m1(m1), _aux(aux) {
        pinMode(_m0, OUTPUT);
        pinMode(_m1, OUTPUT);
    } 
    // this function will operate the module base on its mode
    void begin(Stream *handler,OPERATING_MODE mode){
        // 
        lora = handler;
        switch(mode){
            case OPERATING_MODE::NORMAL:
                /*
                    UART and wireless channel are open, transparent transmission is on
                */
                digitalWrite(_m0, LOW);
                digitalWrite(_m1, LOW);
                delay(100);
                break;
            case OPERATING_MODE::WAKE_UP:
                /*
                    UART and wireless channel are open, the only difference with mode (NORMAL = 0)
                    is that before transmitting data, increasing the wake up code automatically, so
                    that it can awake the receiver under mode (SLEEP = 3).
                */
                digitalWrite(_m0, HIGH);
                digitalWrite(_m1, LOW);
                delay(100);
                break;
            case OPERATING_MODE::POWER_SAVING:
                /*
                    UART close, wireless is under air-awaken mode, after receiving data, UART open and send data.
                */
                digitalWrite(_m0, LOW);
                digitalWrite(_m1, HIGH);
                delay(100);
                break;
            case OPERATING_MODE::SLEEP:
                /*
                    sleep mode, receiving parameter setting command is available.
                */
                digitalWrite(_m0, HIGH);
                digitalWrite(_m1, HIGH);
                delay(100);
                break;
            default:
                break;
        }
        delay(50);
    }
    // it will send up to 58 bytes
    void sendSinglePacket(uint16_t address, uint8_t channel,const char *packet){
        // let's check if the module isn't busy
        if(digitalRead(_aux)){
            // write addrh
            lora->write(((address >> 8) & 0xff));
            // write addrl
            lora->write((address & 0xff));
            // write channel
            lora->write(channel);
            // send a buffer, notice that it will except just 58 bytes and ignore the other
            // as the datasheet state is max tx is 58 bytes
            lora->print(packet);
            // wait just until module send the packet
            while(!digitalRead(_aux));
            delay(50);
        }
    }

    void sendData(uint16_t address, uint8_t channel, const char *data){
        // char *startData = const_cast<char *>(data);
        // char *cursor = startData;
        // for(uint8_t i = 0; i < 58; i++){
        //     cursor++;
        // }
    }

    bool Message(){
        return (!digitalRead(_aux));
    }

    char *readMessage(char *buffer){
        delay(5);
        char *head = buffer;
        char *pChar = buffer;
        while (!digitalRead(_aux));
        {
            if (lora->available())
            {
                while (lora->available())
                {
                    char ch = lora->read();
                    *pChar++ = ch;
                }
            }
        }
        *pChar = '\0';
        while(!digitalRead(_aux));
        return head;
    }
};

#endif