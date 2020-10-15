#include "gtest/gtest.h"
#include "containers/static_binary_tree.h"
#include "containers/static_hash_table.h"
#include <map>

declare_static_binary_tree_t(SetType, int, 10);
define_static_binary_tree_t(SetType, int, 10);

declare_static_hash_table_t(HashTable, int, 10);
define_static_hash_table_t(HashTable, int, 10);

int compare_set_ints(const int* v1, const int* v2)
{
    if(*v1 > *v2)
    {
        return 1;
    }
    else if(*v1 < *v2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

unsigned int hash_function(const int* value)
{
    return *value;
}

void Init(SetType* const set_object)
{
    SetType_Init(set_object, compare_set_ints);
}

void Init(HashTable* const hash_table)
{
    HashTable_Init(hash_table, compare_set_ints, hash_function);
}

size_t Size(SetType* const set_object)
{
    return SetType_Size(set_object);
}

size_t Size(HashTable* const hash_table)
{
    return HashTable_Size(hash_table);
}

bool Empty(SetType* const set_object)
{
    return SetType_Empty(set_object);
}

bool Empty(HashTable* const set_object)
{
    return HashTable_Empty(set_object);
}

int Insert(SetType* const set_object, int value)
{
    return SetType_Insert(set_object, value);
}

int Insert(HashTable* const set_object, int value)
{
    return HashTable_Insert(set_object, value);
}

template<typename iterator_t>
int Erase(SetType* const set_object, iterator_t* it)
{
    return SetType_Erase(set_object, it);
}

template<typename iterator_t>
int Erase(HashTable* const set_object, iterator_t* it)
{
    return HashTable_Erase(set_object, it);
}

auto Begin(SetType* const set_object)
{
    return SetType_Begin(set_object);
}

auto Begin(HashTable* const set_object)
{
    return HashTable_Begin(set_object);
}

auto End(SetType* const set_object)
{
    return SetType_End(set_object);
}

auto End(HashTable* const set_object)
{
    return HashTable_End(set_object);
}

auto IteratorValue(SetType_iterator* const it)
{
    return SetType_Iterator_GetValue(it);
}

auto IteratorValue(HashTable_iterator* const it)
{
    return HashTable_Iterator_GetValue(it);
}

void IteratorInc(SetType_iterator* const it)
{
    return SetType_Iterator_Increment(it);
}

void IteratorInc(HashTable_iterator* const it)
{
    return HashTable_Iterator_Increment(it);
}

void IteratorDec(SetType_iterator* const it)
{
    return SetType_Iterator_Decrement(it);
}

void IteratorDec(HashTable_iterator* const it)
{
    return HashTable_Iterator_Decrement(it);
}

void IteratorSetValue(SetType_iterator* const it, int value)
{
    SetType_Iterator_SetValue(it, value);
}

void IteratorSetValue(HashTable_iterator* const it, int value)
{
    HashTable_Iterator_SetValue(it, value);
}

bool Iterator_Equal(SetType_iterator* const first, SetType_iterator* const second)
{
    return SetType_Iterator_Equal(first, second);
}

bool Iterator_Equal(HashTable_iterator* const first, HashTable_iterator* const second)
{
    return HashTable_Iterator_Equal(first, second);
}

auto Find(SetType* const vector, int value)
{
    return SetType_Find(vector, value);
}

auto Find(HashTable* const vector, int value)
{
    return HashTable_Find(vector, value);
}

template<typename T>
struct SetTest : public testing::Test
{};

using MyTypes = testing::Types<
    SetType
        >;
TYPED_TEST_CASE(SetTest, MyTypes);

TYPED_TEST(SetTest, Init)
{
    TypeParam container{};
    Init(&container);
}

TYPED_TEST(SetTest, IsEmptyAfterInit)
{
    TypeParam container{};
    Init(&container);

    ASSERT_EQ(Size(&container), 0);
    ASSERT_TRUE(Empty(&container));
}

TYPED_TEST(SetTest, InsertVerifyFromBeginToEnd)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    std::set<int> expected_set{temp1, temp2, temp3, temp4, temp5};

    ASSERT_EQ(Insert(&container, temp1), 1);
    ASSERT_EQ(Insert(&container, temp2), 2);
    ASSERT_EQ(Insert(&container, temp3), 3);
    ASSERT_EQ(Insert(&container, temp4), 4);
    ASSERT_EQ(Insert(&container, temp5), 5);

    std::set<int> to_compare_set{};

    auto it = Begin(&container);
    auto end = End(&container);

    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set, to_compare_set);
    ASSERT_EQ(Size(&container), 5);
    ASSERT_FALSE(Empty(&container));
}

TYPED_TEST(SetTest, InsertVerifyFromEndToBegin)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    std::set<int> expected_set{temp1, temp2, temp3, temp4, temp5};

    ASSERT_EQ(Insert(&container, temp1), 1);
    ASSERT_EQ(Insert(&container, temp2), 2);
    ASSERT_EQ(Insert(&container, temp3), 3);
    ASSERT_EQ(Insert(&container, temp4), 4);
    ASSERT_EQ(Insert(&container, temp5), 5);

    std::set<int> to_compare_set{};

    auto begin = Begin(&container);
    auto it = End(&container);

    while(!Iterator_Equal(&it, &begin))
    {   
        IteratorDec(&it);
        to_compare_set.insert(IteratorValue(&it));
    }    
    ASSERT_EQ(expected_set, to_compare_set);
    ASSERT_EQ(Size(&container), 5);
    ASSERT_FALSE(Empty(&container));
}

TYPED_TEST(SetTest, EraseVerifyFromBeginToEnd)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    Insert(&container, temp1);
    Insert(&container, temp2);
    Insert(&container, temp3);
    Insert(&container, temp4);
    Insert(&container, temp5);

    std::set<int> to_compare_set{};

    auto it = Begin(&container);
    auto end = End(&container);

    /* Remove temp4 ***************************************************/
    // Get temp4 iterator
    while(IteratorValue(&it) != temp4)
    {
        IteratorInc(&it);
    }

    // Remove temp4
    ASSERT_EQ(Erase(&container, &it), 4);

    std::set<int> expected_set_1{temp1, temp2, temp3, temp5};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_1, to_compare_set);
    ASSERT_EQ(Size(&container), 4);

    /* Remove temp2 ***************************************************/
    it = Begin(&container);
    end = End(&container);

    while(IteratorValue(&it) != temp2)
    {
        IteratorInc(&it);
    }

    ASSERT_EQ(Erase(&container, &it), 3);

    std::set<int> expected_set_2{temp1, temp3, temp5};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_2, to_compare_set);
    ASSERT_EQ(Size(&container), 3);

    /* Remove temp1 ***************************************************/
    it = Begin(&container);
    end = End(&container);
    
    while(IteratorValue(&it) != temp1)
    {
        IteratorInc(&it);
    }

    ASSERT_EQ(Erase(&container, &it), 2);

    std::set<int> expected_set_3{temp3, temp5};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_3, to_compare_set);
    ASSERT_EQ(Size(&container), 2);

    /* Remove temp5 ***************************************************/
    it = Begin(&container);
    end = End(&container);
    
    while(IteratorValue(&it) != temp5)
    {
        IteratorInc(&it);
    }

    ASSERT_EQ(Erase(&container, &it), 1);

    std::set<int> expected_set_4{temp3};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_4, to_compare_set);
    ASSERT_EQ(Size(&container), 1);

    /* Remove temp3 ***************************************************/
    it = Begin(&container);
    end = End(&container);
    
    while(IteratorValue(&it) != temp3)
    {
        IteratorInc(&it);
    }

    ASSERT_EQ(Erase(&container, &it), 0);

    std::set<int> expected_set_5{};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_5, to_compare_set);
    ASSERT_EQ(Size(&container), 0);

    /* Insert temp6 ****************************************/
    int temp6{ 4321 };
    ASSERT_EQ(Insert(&container, temp6), 1);
    std::set<int> expected_set_6{temp6};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_6, to_compare_set);
    ASSERT_EQ(Size(&container), 1);
}

TYPED_TEST(SetTest, ModifyContainerValues)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    Insert(&container, temp1);
    Insert(&container, temp2);
    Insert(&container, temp3);
    Insert(&container, temp4);
    Insert(&container, temp5);

    std::set<int> to_compare_set{};

    auto it = Begin(&container);
    auto end = End(&container);

    /* Change temp4 value */
    while(IteratorValue(&it) != temp4)
    {
        IteratorInc(&it);
    }

    int temp4_1{ 741 };

    IteratorSetValue(&it, temp4_1);

    std::set<int> expected_set_1{temp1, temp2, temp3, temp4_1, temp5};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_1, to_compare_set);

    /* Change temp1 value */
    it = Begin(&container);
    end = End(&container);
    while(IteratorValue(&it) != temp1)
    {
        IteratorInc(&it);
    }

    int temp1_1{ 8520 };

    IteratorSetValue(&it, temp1_1);

    std::set<int> expected_set_2{temp1_1, temp2, temp3, temp4_1, temp5};

    it = Begin(&container);
    end = End(&container);
    to_compare_set.clear();
    while(!Iterator_Equal(&it, &end))
    {   
        to_compare_set.insert(IteratorValue(&it));
        IteratorInc(&it);
    }    
    ASSERT_EQ(expected_set_2, to_compare_set);
}

TYPED_TEST(SetTest, FindExistedMember)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    Insert(&container, temp1);
    Insert(&container, temp2);
    Insert(&container, temp3);
    Insert(&container, temp4);
    Insert(&container, temp5);

    auto temp1It = Find(&container, temp1);
    ASSERT_EQ(IteratorValue(&temp1It), temp1);

    auto temp2It = Find(&container, temp2);
    ASSERT_EQ(IteratorValue(&temp2It), temp2);

    auto temp3It = Find(&container, temp3);
    ASSERT_EQ(IteratorValue(&temp3It), temp3);

    auto temp4It = Find(&container, temp4);
    ASSERT_EQ(IteratorValue(&temp4It), temp4);

    auto temp5It = Find(&container, temp5);
    ASSERT_EQ(IteratorValue(&temp5It), temp5);
}

TYPED_TEST(SetTest, FindNonExistedMember)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };
    int temp6{ 753 };

    Insert(&container, temp1);
    Insert(&container, temp2);
    Insert(&container, temp3);
    Insert(&container, temp4);
    Insert(&container, temp5);

    auto end = End(&container);

    auto temp6It = Find(&container, temp6);
    ASSERT_TRUE(Iterator_Equal(&temp6It, &end));
}

TYPED_TEST(SetTest, IncrementAndDecrementIterator)
{
    TypeParam container{};
    Init(&container);

    int temp1{ 3215 };
    int temp2{ 23587 };
    int temp3{ 980 };
    int temp4{ 1024 };
    int temp5{ 5005 };

    Insert(&container, temp1);
    Insert(&container, temp2);
    Insert(&container, temp3);
    Insert(&container, temp4);
    Insert(&container, temp5);

    auto it = Begin(&container);

    IteratorInc(&it);
    IteratorInc(&it);
    IteratorInc(&it);

    auto it1Value = IteratorValue(&it);

    IteratorInc(&it);
    IteratorDec(&it);
    auto it1_1Value = IteratorValue(&it);
    ASSERT_EQ(it1Value, it1_1Value);

    IteratorDec(&it);
    IteratorInc(&it);
    auto it1_2Value = IteratorValue(&it);
    ASSERT_EQ(it1Value, it1_2Value);
}