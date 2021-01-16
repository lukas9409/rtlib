#include "pubsub_prot.hpp"
#include <cassert>
#include <string.h>

PubSubProtocol::PubSubProtocol(IPubSubTransportLayer * transport_layer, IPubSubSystemUtils * system_utils) :
    m_transport_layer{ transport_layer }, m_system_utils{ system_utils }
{
    assert(m_transport_layer);
    assert(m_system_utils);
}

int PubSubProtocol::publish(int64_t id, const uint8_t * data, int size, TransmissionParameters transmission_parameters)
{
    // PubSubHeaderV1 header = {
    //     .version = 0,
    //     .id_size = transmission_parameters.id_size,
    //     .payload_length_size = transmission_parameters.payload_length_size,
    //     .qos = transmission_parameters.qos,
    //     .crc = transmission_parameters.crc,
    //     .pid = PUBLISH
    // };

    // size_t crc_size = 0;
    // switch(header.crc)
    // {
    //     case Crc_None:
    //         crc_size = 0;
    //         break;

    //     case Crc_8:
    //         crc_size = 1;
    //         break;

    //     case Crc_16:
    //         crc_size = 2;
    //         break;

    //     case Crc_32:
    //         crc_size = 4;
    //         break;

    //     default:
    //         assert(false);
    //         break;
    // }
    // const size_t to_alloc_bytes = sizeof(header) + (header.id_size + 1) + header.payload_length_size + size +
    // crc_size; uint8_t* to_send_data = (uint8_t*)malloc(to_alloc_bytes); memcpy(to_send_data, &header,
    // sizeof(header));
    // // memcpy(to_send_data + sizeof(header), , sizeof(header));
    // // memcpy(to_send_data + sizeof(header), , header.id_size + 1);
    // memcpy(to_send_data + sizeof(header) + header.id_size + 1 + header.payload_length_size,
    // data, size);

    // m_transport_layer->write(to_send_data, to_alloc_bytes);
    return 0;
}

int PubSubProtocol::subscribe(uint64_t id, Callback callback)
{
    return 0;
}

int PubSubProtocol::unsubscribe(uint64_t id)
{
    return 0;
}

int PubSubProtocol::yield()
{
    return 0;
}