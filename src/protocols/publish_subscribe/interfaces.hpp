#pragma once

#include <cstdint>

struct IPubSubTransportLayer
{
    virtual ~IPubSubTransportLayer() = default;

    virtual int write(const uint8_t* data, int size) = 0;
    virtual int read(uint8_t* data, int size) = 0;
};

struct IPubSubSystemUtils
{
    virtual ~IPubSubSystemUtils() = default;

    virtual void sleep(int timeout_ms) = 0;
    virtual int time() = 0;
};