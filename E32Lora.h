#ifndef E32LORA_H_
#define E32LORA_H_
#include <Arduino.h>
#include "E32Lora_config.h"

/*
    I know sometimes it's better to use struct instead of enum with namespace container 
    but I prefered this method for compile-time setup.
    if you have better solution please fed me up with yours.

    ammm, there's a lot of things need to add but for now library helps 
    for send and receive data from lora modules 
*/

// OPERATING MODE
// NOTICE THAT SLEEP MODE CAN BE USED FOR SETTING COMMAND

/*
    
*/
#define LIMIT 128
typedef void (*on_recv_func_t)(const char *buffer, int length);

enum class OPERATING_MODE
{
    NORMAL,
    WAKE_UP,
    POWER_SAVING,
    SLEEP
};

class E32Lora
{
private:
    uint8_t _m0;
    uint8_t _m1;
    uint8_t _aux;
    OPERATING_MODE _internal_mode;
    on_recv_func_t _callback_func_on_recv;

    Stream *lora;

    uint8_t buffer[LIMIT];
    uint8_t index_buffer = 0;
    bool start = false;

    char configuration[7];

    inline void _wake_up_mode()
    {
        /*
            UART and wireless channel are open, the only difference with mode (NORMAL = 0)
            is that before transmitting data, increasing the wake up code automatically, so
            that it can awake the receiver under mode (SLEEP = 3).
        */
        digitalWrite(_m0, HIGH);
        digitalWrite(_m1, LOW);
        delay(100);
        // wait for _aux to rise edge
        while (!digitalRead(_aux))
            ;
        delay(2);
    }

    inline void _sleep_mode()
    {
        /*
            sleep mode, receiving parameter setting command is available.
        */
        digitalWrite(_m0, HIGH);
        digitalWrite(_m1, HIGH);
        delay(100);
        // wait for _aux to rise edge
        while (!digitalRead(_aux))
            ;
        delay(2);
    }

    void fill(char *buffer, Stream *_stream)
    {
        char *head = buffer;
        char *pBuffer = head;

        // let's feed it up
        while (_stream->available())
        {
            char ch = _stream->read();
            *pBuffer++ = ch;
        }
        *pBuffer = '\0';
    }

    void _set_mode(OPERATING_MODE mode)
    {
        _internal_mode = mode;
        switch (mode)
        {
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

public:
    /*
        E32Lora: provide this construction with desired pin that you want to connect
        to your module, argument are pins of arduino board M0, M1, AUX respectively, 
        and the fourth argument should be feed with desired mode you want to operate on
    */
    E32Lora(uint8_t m0, uint8_t m1, uint8_t aux, OPERATING_MODE mode, on_recv_func_t on_recv_callback)
        : _m0(m0), _m1(m1), _aux(aux), _internal_mode(mode), _callback_func_on_recv(on_recv_callback)
    {
        pinMode(_m0, OUTPUT);
        pinMode(_m1, OUTPUT);
        pinMode(_aux, INPUT);
    }

    /*
        begin: this function it should be feed with a desired serial stream 
        to start and communicate with module, you must call this function first
        before anything else 
    */
    void begin(Stream *handler)
    {
        //
        lora = handler;
        _set_mode(_internal_mode);
    }

    /*
        lora: this method it must be called to process received data from 
        other modules
    */
    void loop()
    {
        if (!digitalRead(_aux))
        {
            while (!digitalRead(_aux))
            {
                #ifdef ESP8266
                yield();
                #endif
                if (lora->available())
                {
                    char ch = lora->read();

                    // if we received a start deleimeter
                    // let's read the message
                    if (start)
                    {
                        if (ch == 0x7E)
                        {
                            start = false;
                            buffer[index_buffer] = '\0';
                            _callback_func_on_recv((const char *)buffer, index_buffer);
                            while(!digitalRead(_aux));
                            break;
                        }
                        buffer[index_buffer++] = ch;
                    }

                    // to indicate if we recived message
                    // or not
                    if (ch == 0x7E)
                    {
                        index_buffer = 0;
                        start = true;
                    }
                }
            }
        }

 
    }

    /*
    
    */
    void send(const char *buffer)
    {
        if (digitalRead(_aux))
        {
            lora->write(0x7E);
            lora->print(buffer);
            lora->write(0x7E);
            while (!digitalRead(_aux))
                ;
        }
    }

    void setupConfiguration(uint16_t address, uint8_t speed, uint8_t channel, uint8_t option)
    {
        // let's switch to sleep mode first
        OPERATING_MODE prev_mode = _internal_mode;
        _set_mode(OPERATING_MODE::SLEEP);

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
        // well, after setuping the confiugration the lora is going
        // send back new information about new saved settings
        // so let's check
        // for test now
        while (!digitalRead(_aux))
            ;
        if (lora->available())
        {
            // fill the buffer with new information
            fill(configuration, lora);
        }

        _set_mode(prev_mode);
    }

    OPERATING_MODE currentMode()
    {
        return _internal_mode;
    }
};

#endif