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

int packHeaderV1(PubSubHeaderV1 header, uint8_t * data, int size)
{
    if(!data)
    {
        return -1;
    }

    if(size < sizeof(header))
    {
        return -1;
    }
    memset(data, 0, sizeof(header));

    // pack byte by byte
    uint8_t firstByte  = 0;
    uint8_t secondByte = 0;

    firstByte  = header.id_size << 2 | header.payload_length_size;
    secondByte = header.qos << 6 | header.crc << 4 | header.pid;

    data[0] = firstByte;
    data[1] = secondByte;

    return sizeof(header);
}

int unpackHeaderV1(PubSubHeaderV1 * header, const uint8_t * data, int size)
{}
