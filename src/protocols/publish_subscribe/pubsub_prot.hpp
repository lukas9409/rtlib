#pragma once

#include <memory>
#include "interfaces.hpp"
#include "common.h"


class PubSubProtocol
{
public:
    using Callback = int(*)(const uint8_t* data, int size);

    PubSubProtocol(IPubSubTransportLayer* transport_layer, IPubSubSystemUtils* system_utils);
    ~PubSubProtocol() = default;

    int publish(int64_t id, const uint8_t* data, int size, TransmissionParameters transmission_parameters);
    int subscribe(uint64_t id, Callback callback);
    int unsubscribe(uint64_t id);
    int yield();

private:
    IPubSubTransportLayer* m_transport_layer;
    IPubSubSystemUtils* m_system_utils;
};