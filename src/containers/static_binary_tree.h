#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../memory/static_pool.h"
#include "error_codes.h"

#define declare_static_binary_tree_t(container_t, member_t, container_capacity)                    \
    typedef struct container_t container_t;                                                        \
    typedef struct container_t##_Iterator container_t##_Iterator;                                  \
    typedef struct container_t##_node container_t##_node;                                          \
                                                                                                   \
    struct container_t##_node                                                                      \
    {                                                                                              \
        container_t##_node * right;                                                                \
        container_t##_node * left;                                                                 \
        container_t##_node * parent;                                                               \
        member_t value;                                                                            \
    };                                                                                             \
                                                                                                   \
    struct container_t##_Iterator                                                                  \
    {                                                                                              \
        container_t##_node * node;                                                                 \
        container_t##_node * next;                                                                 \
        container_t##_node * prev;                                                                 \
    };                                                                                             \
                                                                                                   \
    /* Deprecated - needed to keep compability with version 1 API */                               \
    typedef container_t##_Iterator container_t##_iterator;                                         \
                                                                                                   \
    declare_static_pool_t(container_t##_pool, container_t##_node, container_capacity);             \
                                                                                                   \
    typedef int (*container_t##_compare_t)(const member_t *, const member_t *);                    \
                                                                                                   \
    struct container_t                                                                             \
    {                                                                                              \
        container_t##_node * root;                                                                 \
        container_t##_pool pool;                                                                   \
        container_t##_compare_t compare_function;                                                  \
        size_t size;                                                                               \
    };                                                                                             \
                                                                                                   \
    void container_t##_Construct(container_t * const self, container_t##_compare_t compare);       \
    void container_t##_Destroy(container_t * const self);                                          \
    size_t container_t##_Size(const container_t * const self);                                     \
    bool container_t##_Empty(const container_t * const self);                                      \
    int container_t##_Insert(container_t * const self, member_t data);                             \
    int container_t##_Erase(container_t * const self, container_t##_Iterator * const iterator);    \
    container_t##_Iterator container_t##_Begin(const container_t * const self);                    \
    container_t##_Iterator container_t##_End(const container_t * const self);                      \
    member_t container_t##_Iterator_GetValue(const container_t##_Iterator * const self);           \
    void container_t##_Iterator_SetValue(container_t##_Iterator * const self, member_t value);     \
    bool container_t##_Iterator_Equal(const container_t##_Iterator * const first,                  \
                                      const container_t##_Iterator * const second);                \
    void container_t##_Iterator_Increment(container_t##_Iterator * const self);                    \
    void container_t##_Iterator_Decrement(container_t##_Iterator * const self);                    \
    container_t##_Iterator container_t##_Find(container_t * const self, member_t data);            \
    container_t##_Iterator container_t##_CustomFind(container_t * const self, const member_t data, \
                                                    container_t##_compare_t compare_function);

#define define_static_binary_tree_t(container_t, member_t, container_capacity)                                    \
                                                                                                                  \
    define_static_pool_t(container_t##_pool, container_t##_node, container_capacity);                             \
                                                                                                                  \
    static container_t##_node * __##container_t##_GetNextNode(container_t##_node * node)                          \
    {                                                                                                             \
        if(!node)                                                                                                 \
        {                                                                                                         \
            return NULL;                                                                                          \
        }                                                                                                         \
        if(node->right != NULL)                                                                                   \
        {                                                                                                         \
            node = node->right;                                                                                   \
            while(node->left != NULL)                                                                             \
            {                                                                                                     \
                node = node->left;                                                                                \
            }                                                                                                     \
            return node;                                                                                          \
        }                                                                                                         \
                                                                                                                  \
        while(1)                                                                                                  \
        {                                                                                                         \
            if(node->parent == NULL)                                                                              \
            {                                                                                                     \
                node = NULL;                                                                                      \
                return node;                                                                                      \
            }                                                                                                     \
            if(node->parent->left == node)                                                                        \
            {                                                                                                     \
                node = node->parent;                                                                              \
                return node;                                                                                      \
            }                                                                                                     \
            node = node->parent;                                                                                  \
        }                                                                                                         \
        return node;                                                                                              \
    }                                                                                                             \
                                                                                                                  \
    static container_t##_node * __##container_t##_GetPrevNode(container_t##_node * node)                          \
    {                                                                                                             \
        if(!node)                                                                                                 \
        {                                                                                                         \
            return NULL;                                                                                          \
        }                                                                                                         \
        if(node->left != NULL)                                                                                    \
        {                                                                                                         \
            node = node->left;                                                                                    \
            while(node->right != NULL)                                                                            \
            {                                                                                                     \
                node = node->right;                                                                               \
            }                                                                                                     \
            return node;                                                                                          \
        }                                                                                                         \
                                                                                                                  \
        while(1)                                                                                                  \
        {                                                                                                         \
            if(node->parent == NULL)                                                                              \
            {                                                                                                     \
                node = NULL;                                                                                      \
                return node;                                                                                      \
            }                                                                                                     \
            if(node->parent->right == node)                                                                       \
            {                                                                                                     \
                node = node->parent;                                                                              \
                return node;                                                                                      \
            }                                                                                                     \
            node = node->parent;                                                                                  \
        }                                                                                                         \
        return node;                                                                                              \
    }                                                                                                             \
                                                                                                                  \
    void container_t##_Construct(container_t * const self, container_t##_compare_t compare)                       \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        self->size             = 0;                                                                               \
        self->root             = NULL;                                                                            \
        self->compare_function = compare;                                                                         \
        container_t##_pool_Construct(&self->pool);                                                                \
        assert(container_t##_pool_Capacity(&self->pool) == container_capacity);                                   \
    }                                                                                                             \
                                                                                                                  \
    void container_t##_Destroy(container_t * const self)                                                          \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_pool_Destroy(&self->pool);                                                                  \
    }                                                                                                             \
                                                                                                                  \
    size_t container_t##_Size(const container_t * const self)                                                     \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        return self->size;                                                                                        \
    }                                                                                                             \
                                                                                                                  \
    bool container_t##_Empty(const container_t * const self)                                                      \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        return self->size == 0;                                                                                   \
    }                                                                                                             \
                                                                                                                  \
    int container_t##_Insert(container_t * const self, member_t data)                                             \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_node * node = container_t##_pool_Allocate(&self->pool);                                     \
        if(node)                                                                                                  \
        {                                                                                                         \
            node->value = data;                                                                                   \
                                                                                                                  \
            if(self->root)                                                                                        \
            {                                                                                                     \
                container_t##_node * parent_node = NULL;                                                          \
                container_t##_node * child_node  = self->root;                                                    \
                                                                                                                  \
                while(1)                                                                                          \
                {                                                                                                 \
                    parent_node = child_node;                                                                     \
                                                                                                                  \
                    int compare_value =                                                                           \
                        self->compare_function((const member_t *)&data, (const member_t *)&child_node->value);    \
                    if(compare_value < 0)                                                                         \
                    {                                                                                             \
                        child_node = child_node->left;                                                            \
                        if(!child_node)                                                                           \
                        {                                                                                         \
                            parent_node->left = node;                                                             \
                            break;                                                                                \
                        }                                                                                         \
                    }                                                                                             \
                    else if(compare_value > 0)                                                                    \
                    {                                                                                             \
                        child_node = child_node->right;                                                           \
                        if(!child_node)                                                                           \
                        {                                                                                         \
                            parent_node->right = node;                                                            \
                            break;                                                                                \
                        }                                                                                         \
                    }                                                                                             \
                    else                                                                                          \
                    {                                                                                             \
                        container_t##_pool_Release(&self->pool, node);                                            \
                        return ELEMENT_EXISTS;                                                                    \
                    }                                                                                             \
                }                                                                                                 \
                node->right  = NULL;                                                                              \
                node->left   = NULL;                                                                              \
                node->parent = parent_node;                                                                       \
            }                                                                                                     \
            else                                                                                                  \
            {                                                                                                     \
                node->right  = NULL;                                                                              \
                node->left   = NULL;                                                                              \
                node->parent = NULL;                                                                              \
                self->root   = node;                                                                              \
            }                                                                                                     \
            ++self->size;                                                                                         \
                                                                                                                  \
            return self->size;                                                                                    \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            return ALLOCATION_ERROR;                                                                              \
        }                                                                                                         \
    }                                                                                                             \
                                                                                                                  \
    int container_t##_Erase(container_t * const self, container_t##_Iterator * const iterator)                    \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_node * to_delete_node            = iterator->node;                                          \
        container_t##_node * to_delete_node_parent     = iterator->node->parent;                                  \
        container_t##_node * new_node_at_this_Iterator = NULL;                                                    \
        assert(to_delete_node);                                                                                   \
                                                                                                                  \
        if(!to_delete_node->left && !to_delete_node->right)                                                       \
        {                                                                                                         \
            /* The node has no children */                                                                        \
            new_node_at_this_Iterator = NULL;                                                                     \
        }                                                                                                         \
        else if(to_delete_node->left && to_delete_node->right)                                                    \
        {                                                                                                         \
            /* The node has 2 children */                                                                         \
                                                                                                                  \
            /* Find minimum in right subtree */                                                                   \
            container_t##_node * minimum_node_in_right_subtree = to_delete_node->right;                           \
            while(minimum_node_in_right_subtree->left)                                                            \
            {                                                                                                     \
                minimum_node_in_right_subtree = minimum_node_in_right_subtree->left;                              \
            }                                                                                                     \
            /* Copy mininum value into to delete node */                                                          \
            assert(minimum_node_in_right_subtree);                                                                \
            to_delete_node->value = minimum_node_in_right_subtree->value;                                         \
                                                                                                                  \
            /* Set to_delete_node pointer on new node */                                                          \
            to_delete_node        = minimum_node_in_right_subtree;                                                \
            to_delete_node_parent = to_delete_node->parent;                                                       \
                                                                                                                  \
            /* Verify if to delete node has right node */                                                         \
            if(to_delete_node->right)                                                                             \
            {                                                                                                     \
                new_node_at_this_Iterator = to_delete_node->right;                                                \
            }                                                                                                     \
        }                                                                                                         \
        else if(to_delete_node->left)                                                                             \
        {                                                                                                         \
            /* The node has only left child */                                                                    \
            new_node_at_this_Iterator    = to_delete_node->left;                                                  \
            to_delete_node->left->parent = to_delete_node_parent;                                                 \
        }                                                                                                         \
        else if(to_delete_node->right)                                                                            \
        {                                                                                                         \
            /* The node has only right child */                                                                   \
            new_node_at_this_Iterator     = to_delete_node->right;                                                \
            to_delete_node->right->parent = to_delete_node_parent;                                                \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            assert(0);                                                                                            \
        }                                                                                                         \
                                                                                                                  \
        if(to_delete_node == self->root)                                                                          \
        {                                                                                                         \
            self->root = new_node_at_this_Iterator;                                                               \
            if(self->root)                                                                                        \
            {                                                                                                     \
                self->root->parent = NULL;                                                                        \
            }                                                                                                     \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            const bool is_to_delete_node_left = to_delete_node_parent->left == to_delete_node;                    \
                                                                                                                  \
            if(is_to_delete_node_left)                                                                            \
            {                                                                                                     \
                to_delete_node_parent->left = new_node_at_this_Iterator;                                          \
            }                                                                                                     \
            else                                                                                                  \
            {                                                                                                     \
                to_delete_node_parent->right = new_node_at_this_Iterator;                                         \
            }                                                                                                     \
        }                                                                                                         \
                                                                                                                  \
        container_t##_pool_Release(&self->pool, to_delete_node);                                                  \
                                                                                                                  \
        --self->size;                                                                                             \
        return self->size;                                                                                        \
    }                                                                                                             \
                                                                                                                  \
    container_t##_Iterator container_t##_Begin(const container_t * const self)                                    \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_Iterator it = { 0 };                                                                        \
                                                                                                                  \
        /* Go to the last left node */                                                                            \
        container_t##_node * parent_node = NULL;                                                                  \
        container_t##_node * child_node  = self->root;                                                            \
        while(child_node)                                                                                         \
        {                                                                                                         \
            parent_node = child_node;                                                                             \
            child_node  = child_node->left;                                                                       \
        }                                                                                                         \
        it.node = parent_node;                                                                                    \
        if(parent_node)                                                                                           \
        {                                                                                                         \
            if(parent_node->right)                                                                                \
            {                                                                                                     \
                it.next = parent_node->right;                                                                     \
            }                                                                                                     \
            else                                                                                                  \
            {                                                                                                     \
                it.next = parent_node->parent;                                                                    \
            }                                                                                                     \
        }                                                                                                         \
        it.prev = NULL;                                                                                           \
        return it;                                                                                                \
    }                                                                                                             \
                                                                                                                  \
    container_t##_Iterator container_t##_End(const container_t * const self)                                      \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_Iterator it = { 0 };                                                                        \
                                                                                                                  \
        /* Go to the last right node */                                                                           \
        container_t##_node * parent_node = NULL;                                                                  \
        container_t##_node * child_node  = self->root;                                                            \
        while(child_node)                                                                                         \
        {                                                                                                         \
            parent_node = child_node;                                                                             \
            child_node  = child_node->right;                                                                      \
        }                                                                                                         \
        it.prev = parent_node;                                                                                    \
        return it;                                                                                                \
    }                                                                                                             \
                                                                                                                  \
    member_t container_t##_Iterator_GetValue(const container_t##_Iterator * const self)                           \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        member_t member = self->node->value;                                                                      \
        return member;                                                                                            \
    }                                                                                                             \
                                                                                                                  \
    void container_t##_Iterator_SetValue(container_t##_Iterator * const self, member_t value)                     \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        self->node->value = value;                                                                                \
    }                                                                                                             \
                                                                                                                  \
    bool container_t##_Iterator_Equal(const container_t##_Iterator * const first,                                 \
                                      const container_t##_Iterator * const second)                                \
    {                                                                                                             \
        return first->node == second->node;                                                                       \
    }                                                                                                             \
                                                                                                                  \
    void container_t##_Iterator_Increment(container_t##_Iterator * const self)                                    \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        self->prev = self->node;                                                                                  \
        self->node = self->next;                                                                                  \
        if(!self->next)                                                                                           \
        {                                                                                                         \
            return;                                                                                               \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            self->next = __##container_t##_GetNextNode(self->node);                                               \
        }                                                                                                         \
    }                                                                                                             \
                                                                                                                  \
    void container_t##_Iterator_Decrement(container_t##_Iterator * const self)                                    \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        self->next = self->node;                                                                                  \
        self->node = self->prev;                                                                                  \
        if(!self->prev)                                                                                           \
        {                                                                                                         \
            return;                                                                                               \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            self->prev = __##container_t##_GetPrevNode(self->node);                                               \
        }                                                                                                         \
    }                                                                                                             \
                                                                                                                  \
    container_t##_Iterator container_t##_Find(container_t * const self, member_t data)                            \
    {                                                                                                             \
        assert(self);                                                                                             \
                                                                                                                  \
        container_t##_Iterator it = { 0 };                                                                        \
                                                                                                                  \
        container_t##_node * node = self->root;                                                                   \
                                                                                                                  \
        while(node)                                                                                               \
        {                                                                                                         \
            int compare_result = self->compare_function((const member_t *)&data, (const member_t *)&node->value); \
            if(compare_result == 0)                                                                               \
            {                                                                                                     \
                break;                                                                                            \
            }                                                                                                     \
            else if(compare_result < 0)                                                                           \
            {                                                                                                     \
                node = node->left;                                                                                \
            }                                                                                                     \
            else                                                                                                  \
            {                                                                                                     \
                node = node->right;                                                                               \
            }                                                                                                     \
        }                                                                                                         \
        it.node = node;                                                                                           \
        it.next = __##container_t##_GetNextNode(node);                                                            \
        return it;                                                                                                \
    }                                                                                                             \
                                                                                                                  \
    container_t##_Iterator container_t##_CustomFind(container_t * const self, const member_t data,                \
                                                    container_t##_compare_t compare_function)                     \
    {                                                                                                             \
        assert(self);                                                                                             \
        assert(compare_function);                                                                                 \
                                                                                                                  \
        container_t##_Iterator end = container_t##_End(self);                                                     \
        container_t##_Iterator it  = container_t##_Begin(self);                                                   \
        for(; !container_t##_Iterator_Equal(&it, &end); container_t##_Iterator_Increment(&it))                    \
                                                                                                                  \
        {                                                                                                         \
            const member_t it_value = container_t##_Iterator_GetValue(&it);                                       \
            if(compare_function(&data, &it_value) == 0)                                                           \
            {                                                                                                     \
                break;                                                                                            \
            }                                                                                                     \
        }                                                                                                         \
        return it;                                                                                                \
    }
