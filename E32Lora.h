#ifndef E32LORA_H_
#define E32LORA_H_
#include <Arduino.h>
class E32Lora {
private:
    uint8_t _m0;
    uint8_t _m1;
    uint8_t _aux;
    Stream *lora;
public:
    E32Lora(uint8_t m0, uint8_t m1, uint8_t aux) : _m0(m0), _m1(m1), _aux(aux) {} 
    // this function will operate the module base on its mode
    void begin(Stream *handler, uint8_t mode){
        // 
        lora = handler;
        switch(mode){
            case 0:
                pinMode(_m0, OUTPUT);
                pinMode(_m1, OUTPUT);
                digitalWrite(_m0, LOW);
                digitalWrite(_m1, LOW);
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