#include "common.h"
#include <string.h>

/*
+--------------------------------------------------------------+-------------------------------------+
|                       Header (2 bytes)                       |      Payload section (x bytes)      |
+---------+---------+---------------------+------+------+------+------+-------------+---------+------+
|   3bit  |   3bit  |         2bit        | 2bit | 2bit | 4bit | 1-8B |     0-3B    |   0-xB  | 0-4B |
+---------+---------+---------------------+------+------+------+------+-------------+---------+------+
| version | id size | payload length size |  QoS |  CRC |  pid |  id  | length size | payload |  CRC |
+---------+---------+---------------------+------+------+------+------+-------------+---------+------+
*/

typedef struct PubSubHeaderBitfieldV1
{
    uint8_t version : 3;
    uint8_t id_size : 3;
    uint8_t payload_length_size : 2;
    uint8_t qos : 2;
    uint8_t crc : 2;
    uint8_t pid : 4;
} PubSubHeaderBitfieldV1;

int packHeaderV1(PubSubHeaderV1 header, uint8_t * data, int size)
{
    if(!data)
    {
        return -1;
    }

    if(size < sizeof(PubSubHeaderBitfieldV1))
    {
        return -1;
    }
    memset(data, 0, sizeof(PubSubHeaderBitfieldV1));

    // PubSubHeaderBitfieldV1 bifield = { .version             = 0,
    //                                    .id_size             = header.id_size,
    //                                    .payload_length_size = header.payload_length_size,
    //                                    .qos                 = header.qos,
    //                                    .crc                 = header.crc,
    //                                    .pid                 = header.pid };

    // pack byte by byte
    uint8_t firstByte  = 0;
    uint8_t secondByte = 0;

    firstByte  = header.id_size << 2 | header.payload_length_size;
    secondByte = header.qos << 6 | header.crc << 4 | header.pid;

    data[0] = firstByte;
    data[1] = secondByte;
    // memcpy(data, &bifield, sizeof(bifield));

    return 2;
}

int unpackHeaderV1(PubSubHeaderV1 * header, const uint8_t * data, int size)
{}
