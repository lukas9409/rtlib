#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"
#include "../memory/object_pool.h"
#include <string.h>
#include <assert.h>
#include <stdint.h>

typedef struct Node
{
    Node* prev;
    Node* next;
} Node;

typedef struct List
{
    Node* first;
    Node end; // Aditional node for end iterator
    size_t element_size;
    Pool *pool;
} List;

static int List_PushMiddle(List* const self, const void* const data, int index)
{
    assert(self);
    assert(Pool_Size(self->pool) >= 2);

    uint8_t* ptr = (uint8_t*)Pool_Alloc(self->pool);
    if(!ptr)
    {
        return -1;
    }

    Node* new_node = (Node*)ptr;

    Node* prev_node = self->first;
    Node* current_node = NULL;

    while(index--)
    {
        assert(current_node);
        assert(prev_node);

        prev_node = current_node;
        current_node = current_node->next;
    }
    prev_node->next = new_node;
    current_node->prev = new_node;
    new_node->next = current_node;
    new_node->prev = prev_node;

    return Pool_Size(self->pool);
}

List* List_Init(void* buffer, size_t size, size_t element_size)
{
    List* list = (List*)buffer;
    list->first = &list->end;
    list->end.next = NULL;
    list->end.prev = NULL;
    list->element_size = element_size;
    list->pool = Pool_Init((uint8_t*)buffer + sizeof(List), size - sizeof(List), list->element_size + sizeof(Node));
    return list;
}

size_t List_Capacity(List* const self)
{
    assert(self);

    return Pool_Capacity(self->pool);
}

size_t List_Size(List* const self)
{
    assert(self);

    return Pool_Size(self->pool);
}

int List_Insert(List* const self, const void* const data, int index)
{
    assert(self);

    const size_t list_size = Pool_Size(self->pool);

    if(index > list_size)
    {
        return -1;
    }

    if(index == 0)
    {
        return List_PushFront(self, data);
    }
    else if(index == list_size)
    {
        return List_PushBack(self, data);
    }
    else
    {
        return List_PushMiddle(self, data, index);
    }
}

int List_PushBack(List* const self, const void* const data)
{
    assert(self);

    uint8_t* ptr = (uint8_t*)Pool_Alloc(self->pool);
    if(!ptr)
    {
        return -1;
    }

    Node* node = (Node*)ptr;
    Node* oldLastNode = self->end.prev;

    if(self->first == &(self->end))
    {
        self->first = node;
    }
    else
    {
        assert(oldLastNode);
        oldLastNode->next = node;
    }
    
    assert(node);

    node->next = &self->end;
    node->prev = oldLastNode;
    self->end.prev = node;

    memcpy(ptr + sizeof(Node), data, self->element_size);
    
    return Pool_Size(self->pool);
}

int List_PushFront(List* const self, const void* const data)
{
    assert(self);

    uint8_t* ptr = (uint8_t*)Pool_Alloc(self->pool);
    if(!ptr)
    {
        return -1;
    }

    Node* node = (Node*)ptr;

    node->prev = NULL;
    node->next = self->first;
    self->first->prev = node;
    self->first = node;

    memcpy(ptr + sizeof(Node), data, self->element_size);
    
    return Pool_Size(self->pool);
}

const void* List_Front(List* const self)
{
    assert(self);

    void* ptr = (uint8_t*)self->first + sizeof(Node);
    return ptr;
}

const void* List_Back(List* const self)
{
    assert(self);

    void* ptr = (uint8_t*)self->end.prev + sizeof(Node);
    return ptr;
}

ListIterator List_Begin(List* const self)
{
    assert(self);

    ListIterator iterator;
    iterator.node = self->first;

    return iterator;
}

ListIterator List_End(List* const self)
{
    assert(self);

    ListIterator iterator;
    iterator.node = &self->end;

    return iterator;
}

void ListIterator_Increment(ListIterator* const self)
{
    assert(self);

    self->node = self->node->next;
}

void ListIterator_Decrement(ListIterator* const self)
{
    assert(self);

    self->node = self->node->prev;
}

void* ListIterator_Value(ListIterator* const self)
{
    assert(self);

    void* ptr = (uint8_t*)self->node + sizeof(Node);
    return ptr;
}

bool ListIterator_Equal(ListIterator* const self, ListIterator* const list_iterator)
{
    const bool are_equal = self->node == list_iterator->node;
    return are_equal;
}

#ifdef __cplusplus
}
#endif