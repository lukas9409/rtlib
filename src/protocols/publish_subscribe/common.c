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

typedef struct PubSubHeaderV1Internal
{
    uint8_t version_id : 3;  // Common field in all headers
    uint8_t id_size : 3;
    uint8_t payload_length_size : 2;
    uint8_t qos : 2;
    uint8_t crc : 2;
    uint8_t pid : 4;
} PubSubHeaderV1Internal;

int packHeaderV1(PubSubHeaderV1 header, uint8_t * data, int size)
{
    if(!data)
    {
        return -1;
    }

    if(size < sizeof(PubSubHeaderV1))
    {
        return -1;
    }

    PubSubHeaderV1Internal complete_header = { .version_id          = VERSION_1,
                                               .id_size             = header.id_size,
                                               .payload_length_size = header.payload_length_size,
                                               .qos                 = header.qos,
                                               .crc                 = header.crc,
                                               .pid                 = header.pid };
    // memcpy(data, &complete_header, sizeof(complete_header));

    data[0] = VERSION_1 << 5 | header.id_size << 2 | header.payload_length_size;
    data[1] = header.qos << 6 | header.crc << 4 | header.pid;

    return sizeof(complete_header);
}

int unpackHeaderV1(PubSubHeaderV1 * header, const uint8_t * data, int size)
{
    if(!header || !data)
    {
        return -1;
    }

    if(size < sizeof(PubSubHeaderV1))
    {
        return -1;
    }
    uint8_t version = (data[0] >> 5) & 0b111;

    if(version != VERSION_1)
    {
        return -1;
    }

    uint8_t id_size             = (data[0] >> 2) & 0b111;
    uint8_t payload_length_size = data[0] & 0b11;
    uint8_t qos                 = (data[1] >> 6) & 0b11;
    uint8_t crc                 = (data[1] >> 4) & 0b11;
    uint8_t pid                 = (data[1]) & 0b1111;

    header->id_size             = id_size;
    header->payload_length_size = payload_length_size;
    header->qos                 = qos;
    header->crc                 = crc;
    header->pid                 = pid;

    return sizeof(*header);
}
