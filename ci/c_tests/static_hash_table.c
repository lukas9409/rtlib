#include "containers/static_hash_table.h"

/* Verify if compiles */

declare_static_hash_table_t(TestUnit, int, 20);
define_static_hash_table_t(TestUnit, int, 20);

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

static unsigned int hash_function(const int* value)
{
    return *value;
}

int main()
{
    TestUnit test_unit;
    TestUnit_Construct(&test_unit, compare_set_ints, hash_function);
    TestUnit_Destroy(&test_unit);

    return 0;
}