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
#define LIMIT 80
typedef void (*on_recv_func_t)(const uint8_t *buffer, int length);

enum class OPERATING_MODE
{
    NORMAL,
    WAKE_UP,
    POWER_SAVING,
    SLEEP
};

enum class Mode
{
    ModeTx,
    ModeRx
};

// Packet to be used within
// read_message() if statement
struct Packet
{
    uint8_t *buffer;
    uint8_t length;
};

class E32Lora
{
private:
    uint8_t _m0;
    uint8_t _m1;
    uint8_t _aux;
    OPERATING_MODE _internal_mode;
    on_recv_func_t _callback_func_on_recv = NULL;
    Mode _mode;

    Stream *lora;

    uint8_t buffer[LIMIT];
    uint8_t index_buffer = 0;
    bool start = false;

    Packet new_message;
    bool IsThereNewMessageInBuffer = false;

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

    void _set_operating_mode(OPERATING_MODE mode)
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

    void _send(const uint8_t *buffer, uint8_t len)
    {
        if (digitalRead(_aux))
        {
            lora->write(0x7E);
            lora->write(buffer, len);
            lora->write(0x7E);
            while (!digitalRead(_aux))
            {
#ifdef ESP8266
                yield();
#endif
            }
        }
    }

    void _set_mode(Mode m)
    {
        _mode = m;
    }

    inline void _waitAuxHigh()
    {
        while (!digitalRead(_aux))
        {
#ifdef ESP8266
            yield();
#endif
        }
    }

    inline void _waitAuxLow()
    {
        while (digitalRead(_aux))
        {
#ifdef ESP8266
            yield();
#endif
        }
    }

public:
    /*
        E32Lora: provide this construction with desired pin that you want to connect
        to your module, argument are pins of arduino board M0, M1, AUX respectively, 
        and the fourth argument should be feed with desired mode you want to operate on
    */
    E32Lora(uint8_t m0, uint8_t m1, uint8_t aux, OPERATING_MODE mode)
        : _m0(m0), _m1(m1), _aux(aux), _internal_mode(mode)
    {
        pinMode(_m0, OUTPUT);
        pinMode(_m1, OUTPUT);
        pinMode(_aux, INPUT);
    }

    void set_on_recv_cb(on_recv_func_t cb)
    {
        _callback_func_on_recv = cb;
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
        _set_operating_mode(_internal_mode);
        _set_mode(Mode::ModeRx);
    }

    /*
        lora: this method it must be called to process received data from 
        other modules
    */
    void loop()
    {
        if (_mode == Mode::ModeRx)
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
                                while (!digitalRead(_aux))
                                    ;
                                // for now use 0x7A as ACK tag

                                if (_internal_mode == OPERATING_MODE::POWER_SAVING)
                                {
                                    OPERATING_MODE _prev = _internal_mode;
                                    _set_operating_mode(OPERATING_MODE::NORMAL);
                                    lora->write(0x7A);
                                    _waitAuxHigh();
                                }
                                else
                                {
                                    lora->write(0x7A);
                                    _waitAuxHigh();
                                }
                                if (index_buffer == 0)
                                {
                                    start = true;
                                }
                                (_callback_func_on_recv != NULL) ? _callback_func_on_recv(buffer, index_buffer) : (void)NULL;

                                // update IsThereNewMessageInBuffer
                                IsThereNewMessageInBuffer = true;
                                new_message.buffer = buffer;
                                new_message.length = index_buffer;
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
                    else
                    {
                        IsThereNewMessageInBuffer = false;
                    }
                }
            } else {
                IsThereNewMessageInBuffer = false;
            }
        }
    }

    /*
    
    */
    bool _waitForAck()
    {
        unsigned long start_time = millis();
        while (1)
        {
#ifdef ESP8266
            yield();
#endif
            // maybe you need to consider about
            // the timeout
            if (millis() - start_time > 1500)
            {
                return false;
            }

            if (!lora->available())
                continue;

            return lora->read() == 0x7A ? true : false;
        }
    }
    template <size_t LEN>
    bool send(const uint8_t (&buffer)[LEN])
    {

        if (_internal_mode == OPERATING_MODE::NORMAL || _internal_mode == OPERATING_MODE::WAKE_UP)
        {
            if (digitalRead(_aux))
            {
                _send(buffer, LEN);
                return _waitForAck();
            }
        }
        else
        {
            OPERATING_MODE prev_mode = _internal_mode;
            _set_operating_mode(OPERATING_MODE::WAKE_UP);
            delay(50);
            while (!digitalRead(_aux))
            {
#ifdef ESP82266
                yield();
#endif
            }
            if (digitalRead(_aux))
            {
                _send(buffer, LEN);
                return _waitForAck();
            }
            delay(50);
            _set_operating_mode(prev_mode);
            delay(50);
            while (!digitalRead(_aux))
            {
#ifdef ESP82266
                yield();
#endif
            }
        }
    }

    void setupConfiguration(uint16_t address, uint8_t speed, uint8_t channel, uint8_t option)
    {
        // let's switch to sleep mode first
        OPERATING_MODE prev_mode = _internal_mode;
        _set_operating_mode(OPERATING_MODE::SLEEP);

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

        _set_operating_mode(prev_mode);
    }

    OPERATING_MODE currentMode()
    {
        return _internal_mode;
    }

    // added on 5 - 9 - 2020
    // wrapper around loop function
    bool isThereNewMessage()
    {
        //
        loop();
        return IsThereNewMessageInBuffer;
    }

    Packet *readMessage()
    {
        return &new_message;
    }
};

#endif