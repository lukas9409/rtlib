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

uint8_t crc8(const uint8_t * data, size_t len)
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

static const uint16_t ccitt_hash[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad,
    0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a,
    0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
    0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861,
    0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
    0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87,
    0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
    0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
    0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290,
    0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e,
    0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f,
    0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
    0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83,
    0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

uint16_t crc16(const uint8_t * buffer, size_t size)
{
    uint16_t crc = 0;
    while(size-- > 0)
    {
        crc = (crc << 8) ^ ccitt_hash[((crc >> 8) ^ *(buffer++)) & 0x00FF];
    }
    return crc;
}

// const uint32_t crc32_tab[] = {
//     0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832,
//     0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
//     0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a,
//     0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
//     0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
//     0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
//     0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
//     0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
//     0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4,
//     0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
//     0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074,
//     0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
//     0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525,
//     0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
//     0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
//     0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
//     0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76,
//     0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
//     0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6,
//     0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
//     0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
//     0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
//     0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7,
//     0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
//     0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
//     0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
//     0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330,
//     0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
//     0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
// };

// uint32_t crc32(const void * buf, size_t size)
// {
//     const uint8_t * p = buf;
//     uint32_t crc;

//     crc = ~0U;
//     while(size--)
//         crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
//     return crc ^ ~0U;
// }

static const unsigned int crc32_table[] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005, 0x2608edb8,
    0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
    0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f, 0x639b0da6,
    0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84,
    0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
    0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a,
    0xec7dd02d, 0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
    0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
    0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba, 0xaca5c697, 0xa864db20, 0xa527fdf9,
    0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b,
    0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c,
    0x774bb0eb, 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b, 0x0315d626,
    0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
    0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
    0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a,
    0x8cf30bad, 0x81b02d74, 0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
    0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093,
    0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679,
    0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
    0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09, 0x8d79e0be, 0x803ac667,
    0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

unsigned int crc32(const unsigned char * buf, int len)
{
    unsigned int crc = 0;
    while(len--)
    {
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
        buf++;
    }
    return crc;
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

int packPayloadSizeV1(PubSubHeaderV1 header, int payload_size, uint8_t * output, int output_size)
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

int unpackPayloadSizeV1(PubSubHeaderV1 header, int * payload_size, uint8_t * input, int input_size)
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

int packPayloadV1(const uint8_t * payload, int payload_size, uint8_t * output, int output_size)
{
    if(payload_size > output_size)
    {
        return -1;
    }
    memcpy(output, payload, payload_size);

    return payload_size;
}

int unpackPayloadV1(uint8_t * payload, int payload_size, const uint8_t * input, int input_size)
{
    if(input_size < payload_size)
    {
        return -1;
    }
    memcpy(payload, input, payload_size);
    return payload_size;
}

int calculateAndPackCrcV1(PubSubHeaderV1 header, uint8_t * packet_first_byte, int parsed_bytes, uint8_t * output,
                          int output_size)
{
    int crc_size_bytes = 0;

    switch(header.crc)
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
            uint32_t crc   = crc32(packet_first_byte, parsed_bytes);
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
    return crc_size_bytes;
}

bool verifyCrcV1(PubSubHeaderV1 header, const uint8_t * packet, int size)
{
    bool is_ok = false;

    switch(header.crc)
    {
        case Crc_None:
            is_ok = true;
            break;

        case Crc_8: {
            uint8_t crc = crc8(packet, size);
            if(crc == 0)
            {
                is_ok = true;
            }
        }
        break;

        case Crc_16: {
            uint16_t crc = crc16(packet, size);
            if(crc == 0)
            {
                is_ok = true;
            }
        }
        break;

        case Crc_32: {
            uint32_t crc = crc32(packet, size);
            if(crc == 0)
            {
                is_ok = true;
            }
        }
        break;

        default:
            break;
    }
    return is_ok;
}
