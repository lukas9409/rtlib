#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../memory/typed_pool.h"
#include "error_codes.h"

#define declare_static_hash_table_t(container_t, member_t, container_capacity) \
typedef struct container_t container_t; \
typedef struct container_t##_iterator container_t##_iterator; \
typedef struct container_t##_node container_t##_node; \
\
typedef struct container_t##_node \
{ \
    member_t value; \
    container_t##_node* prev; \
    container_t##_node* next; \
} container_t##_node; \
\
typed_pool_t(container_t##_pool, container_t##_node, container_capacity); \
\
typedef struct container_t##_iterator \
{ \
    container_t##_node * node; \
    container_t* container; \
} container_t##_iterator; \
\
typedef int(*container_t##_compare_t)(const member_t*, const member_t*); \
typedef unsigned int(*container_t##_hash_t)(const member_t*); \
\
typedef struct container_t \
{ \
    container_t##_node* nodes_table[container_capacity + 1]; \
    container_t##_compare_t compare_function; \
    container_t##_hash_t hash_function; \
    size_t size; \
    container_t##_pool pool; \
} container_t; \
\
void container_t##_Construct(container_t* const self, container_t##_compare_t compare, container_t##_hash_t hash); \
void container_t##_Destroy(container_t* const self); \
size_t container_t##_Size(const container_t * const self); \
bool container_t##_Empty(const container_t * const self); \
int container_t##_Insert(container_t * const self, member_t data); \
int container_t##_Erase(container_t * const self, container_t##_iterator* const iterator); \
container_t##_iterator container_t##_Begin(const container_t * const self); \
container_t##_iterator container_t##_End(const container_t * const self); \
member_t container_t##_Iterator_GetValue(const container_t##_iterator* const self); \
void container_t##_Iterator_SetValue(container_t##_iterator* const self, member_t value); \
bool container_t##_Iterator_Equal(const container_t##_iterator* const first, const container_t##_iterator* const second); \
void container_t##_Iterator_Increment(container_t##_iterator* const self); \
void container_t##_Iterator_Decrement(container_t##_iterator* const self); \
container_t##_iterator container_t##_Find(container_t * const self, member_t data);


#define define_static_hash_table_t(container_t, member_t, container_capacity) \
void container_t##_Construct(container_t* const self, container_t##_compare_t compare, container_t##_hash_t hash) \
{ \
    assert(self); \
    assert(compare); \
    assert(hash); \
    \
    memset(self, 0, sizeof(*self)); \
    self->size = 0; \
    self->compare_function = compare; \
    self->hash_function = hash; \
    container_t##_pool_Init(&self->pool); \
    assert(container_t##_pool_Capacity(&self->pool) == container_capacity); \
} \
\
void container_t##_Destroy(container_t* const self) {} \
\
size_t container_t##_Size(const container_t * const self) \
{ \
    assert(self); \
    \
    return self->size; \
} \
\
bool container_t##_Empty(const container_t * const self) \
{ \
    assert(self); \
    \
    return self->size == 0; \
} \
\
int container_t##_Insert(container_t * const self, member_t data) \
{ \
    assert(self); \
    \
    container_t##_node* node = container_t##_pool_Alloc(&self->pool); \
    if(node) \
    { \
        const unsigned int hash_value = self->hash_function((const member_t*)&data); \
        unsigned int index = hash_value % container_capacity; \
        container_t##_node* before_the_last_node_for_this_hash = NULL; \
        container_t##_node* last_node_for_this_hash = self->nodes_table[index]; \
        \
        while(last_node_for_this_hash) \
        { \
            if(self->compare_function((const member_t*)&data, (const member_t*)&last_node_for_this_hash->value) == 0) \
            { \
                container_t##_pool_Free(&self->pool, node); \
                return ELEMENT_EXISTS; \
            } \
            before_the_last_node_for_this_hash = last_node_for_this_hash; \
            last_node_for_this_hash = last_node_for_this_hash->next; \
        } \
        \
        if(before_the_last_node_for_this_hash) \
        { \
            before_the_last_node_for_this_hash->next = node; \
        } \
        else \
        { \
            self->nodes_table[index] = node; \
        } \
        node->prev = before_the_last_node_for_this_hash; \
        node->value = data; \
    } \
    else \
    { \
        return ALLOCATION_ERROR; \
    } \
    ++self->size; \
    return self->size; \
} \
\
int container_t##_Erase(container_t * const self, container_t##_iterator* const iterator) \
{ \
    assert(self); \
    \
    container_t##_node* prev_node = iterator->node->prev; \
    container_t##_node* next_node = iterator->node->next; \
    if(prev_node) \
    { \
        prev_node->next = next_node; \
    } \
    else \
    { \
        const unsigned int hash_value = self->hash_function((const member_t*)&iterator->node->value); \
        unsigned int index = hash_value % container_capacity; \
        self->nodes_table[index] = next_node; \
    } \
    \
    if(next_node) \
    { \
        next_node->prev = prev_node; \
    } \
    container_t##_pool_Free(&self->pool, iterator->node); \
    \
    --self->size; \
    return self->size; \
} \
\
container_t##_iterator container_t##_Begin(const container_t * const self) \
{ \
    assert(self); \
    \
    container_t##_iterator it = {0}; \
    container_t##_node* begin_node = (container_t##_node*)self->nodes_table[container_capacity]; \
    for(unsigned int i=0; i<container_capacity; ++i) \
    { \
        if(self->nodes_table[i]) \
        { \
            begin_node = self->nodes_table[i]; \
            break; \
        } \
    } \
    it.node = begin_node; \
    it.container = (container_t*)self; \
    return it; \
} \
\
container_t##_iterator container_t##_End(const container_t * const self) \
{ \
    assert(self); \
    \
    container_t##_iterator it = {0}; \
    container_t##_node* end_node = (container_t##_node*)self->nodes_table[container_capacity]; \
    it.node = end_node; \
    it.container = (container_t*)self; \
    return it; \
} \
\
member_t container_t##_Iterator_GetValue(const container_t##_iterator* const self) \
{ \
    assert(self); \
    \
    member_t member = self->node->value; \
    return member; \
} \
\
void container_t##_Iterator_SetValue(container_t##_iterator* const self, member_t value) \
{ \
    assert(self); \
    \
    container_t##_Erase(self->container, self); \
    container_t##_Insert(self->container, value); \
} \
\
bool container_t##_Iterator_Equal(const container_t##_iterator* const first, const container_t##_iterator* const second) \
{ \
    return first->node == second->node; \
} \
\
void container_t##_Iterator_Increment(container_t##_iterator* const self) \
{ \
    assert(self); \
    \
    container_t##_iterator end_it = container_t##_End(self->container); \
    if(self->node == end_it.node)\
    { \
        return; \
    } \
    if(self->node->next) \
    { \
        self->node = self->node->next; \
    } \
    else \
    { \
        const unsigned int hash_value = self->container->hash_function((const member_t*)&self->node->value); \
        unsigned int index = hash_value % container_capacity; \
        \
        container_t##_node** ptr_to_next_node_ptr = &self->container->nodes_table[index + 1]; \
        \
        container_t##_node** ptr_to_end_node_ptr = &self->container->nodes_table[container_capacity]; \
        \
        while(!*ptr_to_next_node_ptr) \
        { \
            ptr_to_next_node_ptr++; \
            if(ptr_to_next_node_ptr == ptr_to_end_node_ptr) \
            { \
                break; \
            } \
        } \
        self->node = *ptr_to_next_node_ptr; \
    } \
} \
\
void container_t##_Iterator_Decrement(container_t##_iterator* const self) \
{ \
    assert(self); \
    \
    container_t##_iterator begin_it = container_t##_Begin(self->container); \
    if(self->node == begin_it.node)\
    { \
        return; \
    } \
    if(self->node != self->container->nodes_table[container_capacity] && self->node->prev) \
    { \
        self->node = self->node->prev; \
    } \
    else \
    { \
        unsigned int index = 0; \
        if(self->node == self->container->nodes_table[container_capacity]) \
        { \
            index = container_capacity; \
        } \
        else \
        { \
            const unsigned int hash_value = self->container->hash_function((const member_t*)&self->node->value); \
            index = hash_value % container_capacity; \
            \
        } \
        container_t##_node** ptr_to_next_node_ptr = &self->container->nodes_table[index - 1]; \
        \
        while(!*ptr_to_next_node_ptr) \
        { \
            ptr_to_next_node_ptr--; \
            if(*ptr_to_next_node_ptr == begin_it.node) \
            { \
                break; \
            } \
        } \
        container_t##_node* subnode = *ptr_to_next_node_ptr; \
        while(subnode->next) \
        { \
            subnode = subnode->next; \
        } \
        self->node = subnode; \
    } \
} \
\
container_t##_iterator container_t##_Find(container_t * const self, member_t data) \
{ \
    container_t##_iterator it = container_t##_End(self); \
    const unsigned int hash_value = self->hash_function((const member_t*)&data); \
    unsigned int index = hash_value % container_capacity; \
    container_t##_node* temp_node = self->nodes_table[index]; \
    while(temp_node) \
    { \
        const int compare_result = self->compare_function((const member_t*)&data,  (const member_t*)&temp_node->value); \
        if(compare_result == 0) \
        { \
            it.node = temp_node; \
            break; \
        } \
        temp_node = temp_node->next; \
    } \
    it.container = (container_t*)self; \
    return it; \
} \

