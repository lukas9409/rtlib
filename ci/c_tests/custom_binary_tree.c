#include "memory/dynamic_allocator.h"
#include "containers/custom_allocator_binary_tree.h"

/* Verify if compiles */
declare_dynamic_allocator_t(DynamicAllocator);
define_dynamic_allocator_t(DynamicAllocator);

declare_custom_allocator_binary_tree_t(TestUnit, int, DynamicAllocator);
define_custom_allocator_binary_tree_t(TestUnit, int, DynamicAllocator);

static int compare_set_ints(const int* v1, const int* v2)
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

int main()
{
    TestUnit test_unit;
    TestUnit_Construct(&test_unit, compare_set_ints);
    TestUnit_Destroy(&test_unit);

    return 0;
}