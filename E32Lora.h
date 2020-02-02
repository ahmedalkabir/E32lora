#ifndef E32LORA_H_
#define E32LORA_H_
#include <Arduino.h>

/*
    I know sometimes it's better to use struct instead of enum with namespace container 
    but I prefered this method for compile-time setup, nothing else for now.
    if you have better solution please fed me up with yours
*/

// OPERATING MODE 
// NOTICE THAT SLEEP MODE CAN BE USED FOR SETTING COMMAND

/*
    
*/

#define to_uint8_t(x) static_cast<uint8_t>(x)

namespace CHANNEL{
    // we substraction from 410, because it's started from 410 
    // as based on the datasheet 
    constexpr uint8_t setup(uint16_t channel){
        return (channel - 410);
    }
}

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

    constexpr uint8_t setup(AIR_DATA_RATE tAIRD, TTL_UART_BAUDRATE tTTLUBAUD, UART_PARITY_BIT tUART){
        return to_uint8_t(tAIRD) | to_uint8_t(tTTLUBAUD) | to_uint8_t(tUART);
    }
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
        OFF_FEC = (0b0 << 2),
        ON_FEC  = (0b1 << 2)
    };

    // 5 4 3
    // 0 0 0 
    enum class WIRELESS_WAKEUP_TIME : uint8_t {
        T250MS  = (0b000 << 3),
        T500MS  = (0b001 << 3),
        T750MS  = (0b010 << 3),
        T1000MS = (0b011 << 3),
        T1250MS = (0b100 << 3),
        T1500MS = (0b101 << 3),
        T1750MS = (0b110 << 3),
        T2000MS = (0b111 << 3)
    };

    // 6
    // 0
    enum class IO_DRIVE_MODE : uint8_t {
        PUSH_PULL_UP    = (0b1 << 6),
        OPEN_COLLECTER  = (0b0 << 6)
    };

    // 7
    // 0
    enum class TRANMISSION_MODE : uint8_t {
        TRANSPARENT_MODE    = (0b0 << 7),
        FIXED_MODE          = (0b1 << 7)
    };

    // function to setup the OPTION parameter at compile time 
    constexpr uint8_t setup(TRANSMIT_POWER tPWR, FEC_SWITCH tFEC, WIRELESS_WAKEUP_TIME tWW, IO_DRIVE_MODE tIO, TRANMISSION_MODE tTRM){
        return to_uint8_t(tPWR) | to_uint8_t(tFEC) | to_uint8_t(tWW) | to_uint8_t(tIO) | to_uint8_t(tTRM);
    }
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
    void begin(Stream *handler){
        // 
        lora = handler;
    }
    
    void operatingMode(OPERATING_MODE mode){
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

    void setupConfiguration(uint16_t address, uint8_t speed, uint8_t channel, uint8_t option){
        // let's switch to sleep mode first
        digitalWrite(_m0, HIGH);
        digitalWrite(_m1, HIGH);
        delay(100);

        // now let's write the option to LORA module
        // C0 parameter it's going to save the configuration
        lora->write(0xC0);
        // write ADDH
        lora->write((address >> 8) & 0xff);
        // write ADDL
        lora->write(address & 0xff);
        // write speed configuration
        lora->write(speed);
        // write channel
        lora->write(channel);
        // write option configuration
        lora->write(option);

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

    // send data to lora device
    void sendData(const char *data){
        lora->write(data);
    }

    // TODO: might to delete
    bool Message(){
        return (!digitalRead(_aux));
    }

    // it's going to read received buffer
    bool readMessage(char *buffer){
        unsigned long start_time = 0;
        if(!digitalRead(_aux)){
            delay(5);
            char *pChar = buffer;
            
            // let's check if there's data to read 
            while(!digitalRead(_aux)){
                repeat:
                if(lora->available()){
                    while (lora->available())
                    {
                        char ch = lora->read();
                        *pChar++ = ch;
                    }            
                }
            }

            if(digitalRead(_aux)){
                start_time = millis();
                while(digitalRead(_aux)){
                    if(!digitalRead(_aux) || (millis() - start_time > 300)){
                        break;
                    }
                }
                if((millis() - start_time) < 300){
                    delay(5);
                    if(!digitalRead(_aux)){
                        goto repeat;
                    }
                }
            }
            *pChar = '\0';
            return true;
        }
        return false;
    }


};

#endif