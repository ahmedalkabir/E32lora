#ifndef E32LORA_CONFIG_H_
#define E32LORA_CONFIG_H_

#define to_uint8_t(x) static_cast<uint8_t>(x)

/* A predefinied constant to be used for 
 supplying user with current configuration of 
 module */

// DEFINITATION NAME OF SPEED
#define AIR_DATA_RATE_0K3 "0.3K"
#define AIR_DATA_RATE_1K2 "1.2K"
#define AIR_DATA_RATE_2K4 "2.4K"
#define AIR_DATA_RATE_4K8 "4.8K"
#define AIR_DATA_RATE_9K6 "9.6K"
#define AIR_DATA_RATE_19K2 "19.2K"

#define TTL_UART_BAUDRATE1200 "1200"
#define TTL_UART_BAUDRATE2400 "2400"
#define TTL_UART_BAUDRATE4800 "4800"
#define TTL_UART_BAUDRATE9600 "9600"
#define TTL_UART_BAUDRATE19200 "19200"
#define TTL_UART_BAUDRATE38400 "34800"
#define TTL_UART_BAUDRATE57600 "57600"
#define TTL_UART_BAUDRATE115200 "115200"

#define UART_PARITY_BIT_8N1 "8N1"
#define UART_PARITY_BIT_8O1 "8O1"
#define UART_PARITY_BIT_8E1 "8E1"

// DEFINITATION NAME OF OPTION

#define TRANSMISSION_POWER_20dBm "20dBm"
#define TRANSMISSION_POWER_17dBm "17dBm"
#define TRANSMISSION_POWER_14dBm "14dBm"
#define TRANSMISSION_POWER_10dBm "10dBm"

#define FEC_SWITCH_OFF_FEC "OFF_FEC"
#define FEC_SWITCH_ON_FEC "ON_FEC"

#define WIRELESS_WAKE_UP_TIME_250MS "250ms"
#define WIRELESS_WAKE_UP_TIME_500MS "500ms"
#define WIRELESS_WAKE_UP_TIME_750MS "750ms"
#define WIRELESS_WAKE_UP_TIME_1000MS "1000ms"
#define WIRELESS_WAKE_UP_TIME_1250MS "1250ms"
#define WIRELESS_WAKE_UP_TIME_1500MS "1500ms"
#define WIRELESS_WAKE_UP_TIME_1750MS "1750ms"
#define WIRELESS_WAKE_UP_TIME_2000MS "2000ms"

#define IO_DRIVE_MODE_PUSH_PULL_UP "PUSH PULL UP"
#define IO_DRIVE_MODE_OPEN_COLLECTER "OPEN COLLECTER"

#define TRANSPARENT_TRANSMISSION_MODE "TRANSPARENT MODE"
#define FIXED_TRANSMISIION_MODE "FIXED MODE"

#define value_to_bits(n, b, s) ((n & (b << s)) >> s)

namespace CHANNEL
{
// we substraction from 410, because it's started from 410
// as based on the datasheet
constexpr uint8_t setup(uint16_t channel)
{
    return (channel - 410);
}
} // namespace CHANNEL

namespace SPEED
{

template <uint8_t speed>
struct air_data_rate
{
    /* data */
    static constexpr const char *value = air_data_rate<value_to_bits(speed, 0b111, 0)>::value;
};

template <>
struct air_data_rate<0b000>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_0K3;
};

template <>
struct air_data_rate<0b001>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_1K2;
};

template <>
struct air_data_rate<0b010>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_2K4;
};

template <>
struct air_data_rate<0b011>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_4K8;
};

template <>
struct air_data_rate<0b100>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_9K6;
};

template <>
struct air_data_rate<0b101>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_19K2;
};

template <>
struct air_data_rate<0b110>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_19K2;
};

template <>
struct air_data_rate<0b111>
{
    /* data */
    static constexpr const char *value = AIR_DATA_RATE_19K2;
};

// AIR DATA RATE
// 2 1 0
// 0 0 0
enum class AIR_DATA_RATE : uint8_t
{
    s0p3K = 0b000,
    sp2K = 0b001,
    s2p4K = 0b010,
    s4p8K = 0b011,
    s9p6k = 0b100,
    s19p2k = 0b101,
};

/* TTL UART BAUDRATE */

template <uint8_t speed>
struct ttl_uart_baudrate
{
    static constexpr const char *value = ttl_uart_baudrate<value_to_bits(speed, 0b111, 3)>::value;
};

template <>
struct ttl_uart_baudrate<0b000>
{
    static constexpr const char *value = TTL_UART_BAUDRATE1200;
};

template <>
struct ttl_uart_baudrate<0b001>
{
    static constexpr const char *value = TTL_UART_BAUDRATE2400;
};

template <>
struct ttl_uart_baudrate<0b010>
{
    static constexpr const char *value = TTL_UART_BAUDRATE4800;
};

template <>
struct ttl_uart_baudrate<0b011>
{
    static constexpr const char *value = TTL_UART_BAUDRATE9600;
};

template <>
struct ttl_uart_baudrate<0b100>
{
    static constexpr const char *value = TTL_UART_BAUDRATE19200;
};

template <>
struct ttl_uart_baudrate<0b101>
{
    static constexpr const char *value = TTL_UART_BAUDRATE38400;
};

template <>
struct ttl_uart_baudrate<0b110>
{
    static constexpr const char *value = TTL_UART_BAUDRATE57600;
};

template <>
struct ttl_uart_baudrate<0b111>
{
    static constexpr const char *value = TTL_UART_BAUDRATE115200;
};

// TTL UART BAUDRATE
// 5 4 3
// 0 0 0
enum class TTL_UART_BAUDRATE : uint8_t
{
    B1200 = (0b000 << 3),
    B2400 = (0b001 << 3),
    B4800 = (0b010 << 3),
    B9600 = (0b011 << 3),
    B19200 = (0b100 << 3),
    B34800 = (0b101 << 3),
    B57600 = (0b110 << 3),
    B115200 = (0b111 << 3)
};

/* UART PARITY BIT */
template <uint8_t speed>
struct uart_parity_bit
{
    static constexpr const char *value = uart_parity_bit<value_to_bits(speed, 0b11, 6)>::value;
};

template <>
struct uart_parity_bit<0b00>
{
    static constexpr const char *value = UART_PARITY_BIT_8N1;
};

template <>
struct uart_parity_bit<0b01>
{
    static constexpr const char *value = UART_PARITY_BIT_8O1;
};

template <>
struct uart_parity_bit<0b10>
{
    static constexpr const char *value = UART_PARITY_BIT_8E1;
};

// UART PARITY BIT
// 7 6
// 0 0
enum class UART_PARITY_BIT : uint8_t
{
    P8N1 = (0b00 << 6),
    P8O1 = (0b01 << 6),
    P8E1 = (0b10 << 6),
};

constexpr uint8_t setup(AIR_DATA_RATE air, TTL_UART_BAUDRATE ttl_baud, UART_PARITY_BIT uart)
{
    return to_uint8_t(air) | to_uint8_t(ttl_baud) | to_uint8_t(uart);
}

} // namespace SPEED

namespace OPTION
{

/* transmition power */
template <uint8_t option>
struct transmit_power
{
    static constexpr const char *value = transmit_power<value_to_bits(option, 0b11, 0)>::value;
};

template <>
struct transmit_power<0b00>
{
    static constexpr const char *value = TRANSMISSION_POWER_20dBm;
};

template <>
struct transmit_power<0b01>
{
    static constexpr const char *value = TRANSMISSION_POWER_17dBm;
};

template <>
struct transmit_power<0b10>
{
    static constexpr const char *value = TRANSMISSION_POWER_14dBm;
};

template <>
struct transmit_power<0b11>
{
    static constexpr const char *value = TRANSMISSION_POWER_10dBm;
};

// 1 0
// 0 0
enum class TRANSMIT_POWER : uint8_t
{
    dBm20 = 0b00,
    dBm17 = 0b01,
    dBm14 = 0b10,
    dBm10 = 0b11,
};

/* FEC SWITCH */
template <uint8_t option>
struct fec_switch
{
    static constexpr const char *value = fec_switch<value_to_bits(option, 0b1, 2)>::value;
};

template <>
struct fec_switch<0b0>
{
    static constexpr const char *value = FEC_SWITCH_OFF_FEC;
};

template <>
struct fec_switch<0b1>
{
    static constexpr const char *value = FEC_SWITCH_ON_FEC;
};

// 2
// 0
enum class FEC_SWITCH : uint8_t
{
    OFF_FEC = (0b0 << 2),
    ON_FEC = (0b1 << 2)
};

/* wireless wake up time */
template <uint8_t option>
struct wireless_wakeup_time
{
    static constexpr const char *value = wireless_wakeup_time<value_to_bits(option, 0b111, 3)>::value;
};

template <>
struct wireless_wakeup_time<0b000>
{
    static constexpr const char *value = WIRELESS_WAKE_UP_TIME_250MS;
};

// 5 4 3
// 0 0 0
enum class WIRELESS_WAKEUP_TIME : uint8_t
{
    T250MS = (0b000 << 3),
    T500MS = (0b001 << 3),
    T750MS = (0b010 << 3),
    T1000MS = (0b011 << 3),
    T1250MS = (0b100 << 3),
    T1500MS = (0b101 << 3),
    T1750MS = (0b110 << 3),
    T2000MS = (0b111 << 3)
};

// 6
// 0
enum class IO_DRIVE_MODE : uint8_t
{
    PUSH_PULL_UP = (0b1 << 6),
    OPEN_COLLECTER = (0b0 << 6)
};

/* Fixed transmission enabling bit */

template <uint8_t option>
struct transmission_mode
{
    static constexpr const char *value = transmission_mode<value_to_bits(option, 0b1, 7)>::value;
};

template <>
struct transmission_mode<0b0>
{
    static constexpr const char *value = TRANSPARENT_TRANSMISSION_MODE;
};

template <>
struct transmission_mode<0b1>
{
    static constexpr const char *value = FIXED_TRANSMISIION_MODE;
};

// 7
// 0
enum class TRANSMISSION_MODE : uint8_t
{
    TRANSPARENT_MODE = (0b0 << 7),
    FIXED_MODE = (0b1 << 7)
};

// function to setup the OPTION parameter at compile time
constexpr uint8_t setup(TRANSMIT_POWER pwr, FEC_SWITCH fec, WIRELESS_WAKEUP_TIME wakeup_time, IO_DRIVE_MODE io, TRANSMISSION_MODE transm_mode)
{
    return to_uint8_t(pwr) | to_uint8_t(fec) | to_uint8_t(wakeup_time) | to_uint8_t(io) | to_uint8_t(transm_mode);
}
} // namespace OPTION

#endif