#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "pubsub_prot_common.h"


#define declare_static_pubsub_prot_t(protocol_t, capacity, subscribe_messages) \
typedef int (*protocol_t##_subscribe_callback)(const uint8_t* data, int size); \
\
typedef struct protocol_t \
{ \
    IPubSubProtTransportLayer* transport_layer; \
    IPubSubProtMutex* mutex; \
    uint8_t buffer[capacity]; \
} protocol_t; \
\
void protocol_t##_Construct(protocol_t* const self, IPubSubProtTransportLayer* transport_layer, IPubSubProtMutex* mutex); \
void protocol_t##_Destroy(protocol_t* const self); \
int protocol_t##_Publish(protocol_t* const self, uint64_t id, const uint8_t* data, int size); \
int protocol_t##_Subscribe(protocol_t* const self, uint64_t id, protocol_t##_subscribe_callback callback); \
int protocol_t##_Unsubscribe(protocol_t* const self, uint64_t id); \
int protocol_t##_Yield(protocol_t* const self);


#define define_static_pubsub_prot_t(protocol_t, capacity, subscribe_messages) \
void protocol_t##_Construct(protocol_t* const self, IPubSubProtTransportLayer* transport_layer, IPubSubProtMutex* mutex) \
{ \
    assert(self); \
} \
\
void protocol_t##_Destroy(protocol_t* const self) \
{ \
    assert(self); \
} \
\
void protocol_t##_Publish(protocol_t* const self, uint64_t id, const uint8_t* data, int size, PubsubProtQoS qos, int timeout) \
{ \
    assert(self); \
    assert(self->transport_layer); \
} \
\
void protocol_t##_Subscribe(protocol_t* const self, uint64_t id, protocol_t##_subscribe_callback callback) \
{ \
    assert(self); \
} \
\
int protocol_t##_Unsubscribe(protocol_t* const self, uint64_t id) \
{ \
    assert(self); \
} \
\
void protocol_t##_Yield(protocol_t* const self) \
{ \
    assert(self); \
    assert(self->transport_layer); \
}
