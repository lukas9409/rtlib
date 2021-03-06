#include "containers/dynamic_binary_tree.h"

/* Verify if compiles */

declare_dynamic_binary_tree_t(TestUnit, int);
define_dynamic_binary_tree_t(TestUnit, int);

static int compare_set_ints(const int * v1, const int * v2)
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