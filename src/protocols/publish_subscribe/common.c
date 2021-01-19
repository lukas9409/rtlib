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

uint8_t crc8(uint8_t * data, size_t len)
{
    uint8_t crc = 0xff;
    size_t i, j;
    for(i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(j = 0; j < 8; j++)
        {
            if((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}

#define POLY 0x8408
unsigned short crc16(char * data_p, unsigned short length)
{
    unsigned char i;
    unsigned int data;
    unsigned int crc = 0xffff;

    if(length == 0)
        return (~crc);

    do
    {
        for(i = 0, data = (unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1)
        {
            if((crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ POLY;
            else
                crc >>= 1;
        }
    } while(--length);

    crc  = ~crc;
    data = crc;
    crc  = (crc << 8) | (data >> 8 & 0xff);

    return (crc);
}

const uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832,
    0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a,
    0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
    0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
    0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4,
    0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074,
    0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525,
    0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
    0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76,
    0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6,
    0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
    0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7,
    0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
    0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330,
    0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t crc32(const void * buf, size_t size)
{
    const uint8_t * p = buf;
    uint32_t crc;

    crc = ~0U;
    while(size--)
        crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

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

static int getIdSizeInBytesFromHeader(PubSubHeaderV1 header)
{
    int size = -1;

    if(header.id_size <= ID_8BYTE && header.id_size >= ID_1BYTE)
    {
        size = header.id_size + 1;
    }
    return size;
}

static int getPayloadLengthFieldSizeInBytesFromHeader(PubSubHeaderV1 header)
{
    int size = -1;

    if(header.payload_length_size >= PAYLOAD_0BYTE && header.payload_length_size <= PAYLOAD_3BYTE)
    {
        size = header.payload_length_size;
    }
    return size;
}

static int getCrcSizeInBytesFromHeader(PubSubHeaderV1 header)
{
    int size = -1;

    switch(header.crc)
    {
        case Crc_None:
            size = 0;
            break;

        case Crc_8:
            size = 1;
            break;

        case Crc_16:
            size = 2;
            break;

        case Crc_32:
            size = 4;
            break;

        default:
            break;
    }
    return size;
}

static int calculateRequiredPayloadSectionSize(PubSubHeaderV1 header, int payload_size)
{
    if(payload_size < 0)
    {
        return -1;
    }

    int message_id_size = getIdSizeInBytesFromHeader(header);
    if(message_id_size < 0)
    {
        return -1;
    }

    int payload_length_size = getPayloadLengthFieldSizeInBytesFromHeader(header);
    if(payload_length_size < 0)
    {
        return -1;
    }

    int crc_size = getCrcSizeInBytesFromHeader(header);
    if(crc_size < 0)
    {
        return -1;
    }
    return message_id_size + payload_length_size + payload_size + crc_size;
}

int packMessageIdV1(PubSubHeaderV1 header, uint64_t message_id, uint8_t * output, int output_size)
{
    switch(header.id_size)
    {
        case ID_1BYTE:
            if(message_id > 0xff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = message_id & 0xff;
            break;

        case ID_2BYTE:
            if(message_id > 0xffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 8) & 0xff;
            output[1] = message_id & 0xff;
            break;

        case ID_3BYTE:
            if(message_id > 0xffffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 16) & 0xff;
            output[1] = (message_id >> 8) & 0xff;
            output[2] = message_id & 0xff;
            break;

        case ID_4BYTE:
            if(message_id > 0xffffffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 24) & 0xff;
            output[1] = (message_id >> 16) & 0xff;
            output[2] = (message_id >> 8) & 0xff;
            output[3] = message_id & 0xff;
            break;

        case ID_5BYTE:
            if(message_id > 0xffffffffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 32) & 0xff;
            output[1] = (message_id >> 24) & 0xff;
            output[2] = (message_id >> 16) & 0xff;
            output[3] = (message_id >> 8) & 0xff;
            output[4] = message_id & 0xff;
            break;

        case ID_6BYTE:
            if(message_id > 0xffffffffffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 40) & 0xff;
            output[1] = (message_id >> 32) & 0xff;
            output[2] = (message_id >> 24) & 0xff;
            output[3] = (message_id >> 16) & 0xff;
            output[4] = (message_id >> 8) & 0xff;
            output[5] = message_id & 0xff;
            break;

        case ID_7BYTE:
            if(message_id > 0xffffffffffffff)
            {
                return ARGUMENT_MISMATCH;
            }
            output[0] = (message_id >> 48) & 0xff;
            output[1] = (message_id >> 40) & 0xff;
            output[2] = (message_id >> 32) & 0xff;
            output[3] = (message_id >> 24) & 0xff;
            output[4] = (message_id >> 16) & 0xff;
            output[5] = (message_id >> 8) & 0xff;
            output[6] = message_id & 0xff;
            break;

        case ID_8BYTE:
            output[0] = (message_id >> 56) & 0xff;
            output[1] = (message_id >> 48) & 0xff;
            output[2] = (message_id >> 40) & 0xff;
            output[3] = (message_id >> 32) & 0xff;
            output[4] = (message_id >> 24) & 0xff;
            output[5] = (message_id >> 16) & 0xff;
            output[6] = (message_id >> 8) & 0xff;
            output[7] = message_id & 0xff;
            break;

        default:
            return -1;
            break;
    }
    int message_id_size = getIdSizeInBytesFromHeader(header);
    return message_id_size;
}

int unpackMessageIdV1(PubSubHeaderV1 header, uint64_t * message_id, const uint8_t * input, int input_size)
{
    *message_id = 0;
    switch(header.id_size)
    {
        case ID_1BYTE: {
            uint32_t temp = input[0];
            *message_id   = (uint64_t)temp;
        }
        break;

        case ID_2BYTE: {
            uint32_t temp = (input[0] << 8) | input[1];
            *message_id   = (uint64_t)temp;
        }
        break;

        case ID_3BYTE: {
            uint32_t temp = (input[0] << 16) | (input[1] << 8) | input[2];
            *message_id   = (uint64_t)temp;
        }
        break;

        case ID_4BYTE: {
            uint32_t temp = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
            *message_id   = (uint64_t)temp;
        }
        break;

        case ID_5BYTE: {
            uint32_t temp1 = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
            uint32_t temp2 = input[4];
            *message_id    = (uint64_t)temp1 << 8 | (uint64_t)temp2;
        }
        break;

        case ID_6BYTE: {
            uint32_t temp1 = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
            uint32_t temp2 = (input[4] << 8) | input[5];
            *message_id    = (uint64_t)temp1 << 16 | (uint64_t)temp2;
        }
        break;

        case ID_7BYTE: {
            uint32_t temp1 = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
            uint32_t temp2 = (input[4] << 16) | (input[5] << 8) | input[6];
            *message_id    = (uint64_t)temp1 << 24 | (uint64_t)temp2;
        }
        break;

        case ID_8BYTE: {
            uint32_t temp1 = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
            uint32_t temp2 = (input[4] << 24) | (input[5] << 16) | (input[6] << 8) | input[7];
            *message_id    = (uint64_t)temp1 << 32 | (uint64_t)temp2;
        }
        break;

        default:
            return -1;
            break;
    }
    int message_id_size = getIdSizeInBytesFromHeader(header);
    return message_id_size;
}

int packPayloadSizeV1(PubSubHeaderV1 header, uint32_t payload_size, uint8_t * output, int output_size)
{
    switch(header.payload_length_size)
    {
        case PAYLOAD_0BYTE:
            if(payload_size != 0)
            {
                return -1;
            }
            break;

        case PAYLOAD_1BYTE:
            if(payload_size > 0xff)
            {
                return -1;
            }
            output[0] = payload_size & 0xff;
            break;

        case PAYLOAD_2BYTE:
            if(payload_size > 0xffff)
            {
                return -1;
            }
            output[0] = (payload_size >> 8) & 0xff;
            output[1] = payload_size & 0xff;
            break;

        case PAYLOAD_3BYTE:
            if(payload_size > 0xffffff)
            {
                return -1;
            }
            output[0] = (payload_size >> 16) & 0xff;
            output[1] = (payload_size >> 8) & 0xff;
            output[2] = payload_size & 0xff;
            break;

        default:
            return -1;
            break;
    }
    const int parsed_bytes = getPayloadLengthFieldSizeInBytesFromHeader(header);
    return parsed_bytes;
}

int unpackPayloadSizeV1(PubSubHeaderV1 header, uint32_t * payload_size, uint8_t * input, int input_size)
{
    switch(header.payload_length_size)
    {
        case PAYLOAD_0BYTE:
            break;

        case PAYLOAD_1BYTE:
            *payload_size = input[0];
            break;

        case PAYLOAD_2BYTE:
            *payload_size = (input[0] << 8) | input[1];
            break;

        case PAYLOAD_3BYTE:
            *payload_size = (input[0] << 16) | (input[1] << 8) | input[2];
            break;

        default:
            return -1;
            break;
    }
    const int parsed_bytes = getPayloadLengthFieldSizeInBytesFromHeader(header);
    return parsed_bytes;
}

static int packPayloadV1(uint8_t * payload, int payload_size, uint8_t * output, int output_size)
{
    if(payload_size > output_size)
    {
        return -1;
    }
    memcpy(output, payload, payload_size);

    return payload_size;
}

static int calculateAndPackCrcV1(uint8_t * packet_first_byte, int parsed_bytes, uint8_t * output, int output_size)
{
    PubSubHeaderV1 * header = (PubSubHeaderV1 *)packet_first_byte;

    int crc_size_bytes = 0;

    switch(header->crc)
    {
        case Crc_None:
            break;

        case Crc_8: {
            uint8_t crc    = crc8(packet_first_byte, parsed_bytes);
            output[0]      = crc;
            crc_size_bytes = 1;
        }
        break;

        case Crc_16: {
            uint16_t crc   = crc16(packet_first_byte, parsed_bytes);
            output[0]      = (crc >> 8) & 0xff;
            output[1]      = crc & 0xff;
            crc_size_bytes = 2;
        }
        break;

        case Crc_32: {
            uint16_t crc   = crc32(packet_first_byte, parsed_bytes);
            output[0]      = (crc >> 24) & 0xff;
            output[1]      = (crc >> 16) & 0xff;
            output[2]      = (crc >> 8) & 0xff;
            output[3]      = crc & 0xff;
            crc_size_bytes = 4;
        }
        break;

        default:
            return -1;
            break;
    }
    return parsed_bytes;
}

// int packPayloadV1(PubSubHeaderV1 header, uint64_t message_id, uint8_t * payload, int payload_size, uint8_t *
// output,
//                   int output_size)
// {
//     const int required_size = calculateRequiredPayloadSectionSize(header, payload_size);
//     if(required_size < 0)
//     {
//         return -1;
//     }

//     if(required_size > output_size)
//     {
//         return -1;
//     }

//     int packed_bytes = 0;
//     int ret          = packMessageIdV1(header, message_id, output, output_size);
//     if(ret < 0)
//     {
//         return ret;
//     }
//     packed_bytes += ret;

//     ret = packPayloadSizeV1(header, payload_size, output + packed_bytes, output_size - packed_bytes);
//     if(ret < 0)
//     {
//         return ret;
//     }
//     packed_bytes += ret;

//     ret = packPayloadV1(payload, payload_size, output + packed_bytes, output_size - packed_bytes);
//     if(ret < 0)
//     {
//         return ret;
//     }
//     packed_bytes += ret;

//     return required_size;
// }