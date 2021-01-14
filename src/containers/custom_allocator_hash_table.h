#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "error_codes.h"

#define declare_custom_allocator_hash_table_t(container_t, member_t, allocator_t) \
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
typedef struct container_t##_iterator \
{ \
    container_t##_node * node; \
    container_t* container; \
} container_t##_iterator; \
\
typedef int(*container_t##compare_t)(const member_t*, const member_t*); \
typedef unsigned int(*container_t##hash_t)(const member_t*); \
\
typedef struct container_t \
{ \
    container_t##_node** nodes_table; \
    size_t nodes_table_size; \
    container_t##compare_t compare_function; \
    container_t##hash_t hash_function; \
    size_t size; \
    allocator_t allocator; \
} container_t; \
\
void container_t##_Construct(container_t* const self, container_t##compare_t compare, container_t##hash_t hash); \
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


#define define_custom_allocator_hash_table_t(container_t, member_t, allocator_t) \
static bool __##container_t##_InsertNodeToHashTable(container_t* const self, container_t##_node* node, container_t##_node** hash_table, size_t hash_table_size) \
{ \
    const unsigned int hash_value = self->hash_function((const member_t*)&node->value); \
    unsigned int index = hash_value % hash_table_size; \
    container_t##_node* before_the_last_node_for_this_hash = NULL; \
    container_t##_node* last_node_for_this_hash = hash_table[index]; \
    \
    while(last_node_for_this_hash) \
    { \
        if(self->compare_function((const member_t*)&node->value, (const member_t*)&last_node_for_this_hash->value) == 0) \
        { \
            return false; \
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
        hash_table[index] = node; \
    } \
    node->prev = before_the_last_node_for_this_hash; \
    return true; \
} \
\
void container_t##_Construct(container_t* const self, container_t##compare_t compare, container_t##hash_t hash) \
{ \
    assert(self); \
    assert(compare); \
    assert(hash); \
    \
    memset(self, 0, sizeof(*self)); \
    self->size = 0; \
    self->nodes_table_size = 4; \
    self->compare_function = compare; \
    self->hash_function = hash; \
    allocator_t##_Construct(&self->allocator); \
    const size_t nodes_table_size_in_bytes = (self->nodes_table_size + 1) * sizeof(self->nodes_table[0]); \
    self->nodes_table = (container_t##_node**)allocator_t##_Allocate(&self->allocator, nodes_table_size_in_bytes); \
    assert(self->nodes_table); \
    memset(self->nodes_table, 0, nodes_table_size_in_bytes); \
} \
\
void container_t##_Destroy(container_t* const self) \
{ \
    while(container_t##_Size(self) != 0) \
    { \
        container_t##_iterator begin = container_t##_Begin(self); \
        container_t##_Erase(self, &begin); \
    } \
    allocator_t##_Deallocate(&self->allocator, self->nodes_table); \
    allocator_t##_Destroy(&self->allocator); \
} \
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
    container_t##_node* node = (container_t##_node*)allocator_t##_Allocate(&self->allocator, sizeof(container_t##_node)); \
    memset(node, 0, sizeof(container_t##_node)); \
    if(node) \
    { \
        node->value = data; \
        if(self->nodes_table_size == self->size) \
        { \
            size_t new_nodes_table_size = self->nodes_table_size * 2; \
            const size_t nodes_table_size_in_bytes = (new_nodes_table_size + 1) * sizeof(self->nodes_table[0]); \
            container_t##_node** new_nodes_table = (container_t##_node**)allocator_t##_Allocate(&self->allocator, nodes_table_size_in_bytes); \
            memset(new_nodes_table, 0, nodes_table_size_in_bytes); \
            if(new_nodes_table) \
            { \
                const size_t size_copy = self->size; \
                while(self->size) \
                { \
                    container_t##_iterator it=container_t##_Begin(self); \
                    container_t##_node* rehased_node = (container_t##_node*)allocator_t##_Allocate(&self->allocator, sizeof(container_t##_node)); \
                    rehased_node->prev = NULL; \
                    rehased_node->next = NULL; \
                    rehased_node->value = it.node->value; \
                    if(!__##container_t##_InsertNodeToHashTable(self, rehased_node, new_nodes_table, new_nodes_table_size)) \
                    { \
                        assert(false); /* Should never happen */ \
                    } \
                    container_t##_Erase(self, &it); \
                } \
                self->size = size_copy; \
                allocator_t##_Deallocate(&self->allocator, self->nodes_table); \
                self->nodes_table = new_nodes_table; \
                self->nodes_table_size = new_nodes_table_size; \
            } \
            else \
            { \
                return ALLOCATION_ERROR; \
            } \
        } \
        if(!__##container_t##_InsertNodeToHashTable(self, node, self->nodes_table, self->nodes_table_size)) \
        { \
            allocator_t##_Deallocate(&self->allocator, node); \
            return ELEMENT_EXISTS; \
        } \
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
        unsigned int index = hash_value % self->nodes_table_size; \
        self->nodes_table[index] = next_node; \
    } \
    \
    if(next_node) \
    { \
        next_node->prev = prev_node; \
    } \
    allocator_t##_Deallocate(&self->allocator, iterator->node); \
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
    container_t##_node* begin_node = (container_t##_node*)self->nodes_table[self->nodes_table_size]; \
    for(unsigned int i=0; i<self->nodes_table_size; ++i) \
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
    container_t##_node* end_node = (container_t##_node*)self->nodes_table[self->nodes_table_size]; \
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
        unsigned int index = hash_value % self->container->nodes_table_size; \
        \
        container_t##_node** ptr_to_next_node_ptr = &self->container->nodes_table[index + 1]; \
        \
        container_t##_node** ptr_to_end_node_ptr = &self->container->nodes_table[self->container->nodes_table_size]; \
        \
        while(ptr_to_next_node_ptr != ptr_to_end_node_ptr) \
        { \
            if(*ptr_to_next_node_ptr) \
            { \
                break; \
            } \
            ptr_to_next_node_ptr++; \
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
    if(self->node != self->container->nodes_table[self->container->nodes_table_size] && self->node->prev) \
    { \
        self->node = self->node->prev; \
    } \
    else \
    { \
        unsigned int index = 0; \
        if(self->node == self->container->nodes_table[self->container->nodes_table_size]) \
        { \
            index = self->container->nodes_table_size; \
        } \
        else \
        { \
            const unsigned int hash_value = self->container->hash_function((const member_t*)&self->node->value); \
            index = hash_value % self->container->nodes_table_size; \
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
    unsigned int index = hash_value % self->nodes_table_size; \
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

