#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /*
    +--------------------------------------------------------------+-------------------------------------+
    |                       Header (2 bytes)                       |      Payload section (x bytes)      |
    +---------+---------+---------------------+------+------+------+------+-------------+---------+------+
    |   3bit  |   3bit  |         2bit        | 2bit | 2bit | 4bit | 1-8B |     0-3B    |   0-xB  | 0-4B |
    +---------+---------+---------------------+------+------+------+------+-------------+---------+------+
    | version |mid size | payload length size |  QoS |  CRC |  pid |  mid | length size | payload |  CRC |
    +---------+---------+---------------------+------+------+------+------+-------------+---------+------+
    */

    typedef enum
    {
        // External errors
        ALLOCATION_ERROR = -255,
        INVALID_CRC,
        UNKNOWN_VERSION,

        // Internal errors
        TIMEOUT = -127,
        ARGUMENT_MISMATCH,

        OK = 0
    } PubsubProtErrorCode;

    typedef enum
    {
        VERSION_1
    } PubsubProtVersion;

    typedef enum
    {
        ID_1BYTE,
        ID_2BYTE,
        ID_3BYTE,
        ID_4BYTE,
        ID_5BYTE,
        ID_6BYTE,
        ID_7BYTE,
        ID_8BYTE
    } PubSubIdSize;

    typedef enum
    {
        PAYLOAD_0BYTE,
        PAYLOAD_1BYTE,
        PAYLOAD_2BYTE,
        PAYLOAD_3BYTE
    } PubSubPayloadParametersSize;

    typedef enum
    {
        QoS0,
        QoS1,
        QoS2
    } PubsubProtQoS;

    typedef enum
    {
        Crc_None,
        Crc_8,
        Crc_16,
        Crc_32
    } PubsubProtCrc;

    typedef enum
    {
        PUBLISH,
        PUBACK,
        PUBNACK
    } PubSubPid;

    typedef struct TransmissionParameters
    {
        int timeout_ms;
        int step_ms;
        PubSubIdSize id_size;
        PubSubPayloadParametersSize payload_length_size;
        PubsubProtQoS qos;
        PubsubProtCrc crc;

    } TransmissionParameters;

    typedef struct PubSubHeaderV1
    {
        uint8_t id_size : 3;
        uint8_t payload_length_size : 2;
        uint8_t qos : 2;
        uint8_t crc : 2;
        uint8_t pid : 4;
    } PubSubHeaderV1;

    int packHeaderV1(PubSubHeaderV1 header, uint8_t * data, int size);
    int unpackHeaderV1(PubSubHeaderV1 * header, const uint8_t * data, int size);
    int packMessageIdV1(PubSubHeaderV1 header, uint64_t message_id, uint8_t * output, int output_size);
    int unpackMessageIdV1(PubSubHeaderV1 header, uint64_t * message_id, const uint8_t * input, int input_size);
    int packPayloadSizeV1(PubSubHeaderV1 header, int payload_size, uint8_t * output, int output_size);
    int unpackPayloadSizeV1(PubSubHeaderV1 header, int * payload_size, uint8_t * input, int input_size);
    int packPayloadV1(const uint8_t * payload, int payload_size, uint8_t * output, int output_size);
    int unpackPayloadV1(uint8_t * payload, int payload_size, const uint8_t * input, int input_size);

#ifdef __cplusplus
}
#endif