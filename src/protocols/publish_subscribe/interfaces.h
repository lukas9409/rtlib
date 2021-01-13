#pragma once

typedef struct IPubSubProtTransportLayer IPubSubProtTransportLayer;

typedef struct IPubSubProtTransportLayer
{
    int (*write)(IPubSubProtTransportLayer* const self, const uint8_t* data, int size);
    int (*read)(IPubSubProtTransportLayer* const self, uint8_t* data, int size);
} IPubSubProtTransportLayer;

typedef struct IPubSubProtMutex IPubSubProtMutex;

typedef struct IPubSubProtMutex
{
    int (*lock)(IPubSubProtMutex* const self, int timeout);
    int (*unlock)(IPubSubProtMutex* const self);
} IPubSubProtMutex;
