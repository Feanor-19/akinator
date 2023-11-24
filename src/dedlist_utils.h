#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <assert.h>

//! @details Inserts a copy of str_source into str_dest, starting
//! from index start_ind. If str_dest capacity is exceeded, str_source is partly copied
//! and res is set to 0. Otherwise res is set to 1.
//! @return Index of the char right after inserted string
//! (be careful, it might be index of the terminating symbol)
inline size_t str_insert(   char *str_dest_begin,
                            size_t str_dest_capacity,
                            const char *str_source,
                            size_t start_ind,
                            int *res )
{
    assert(str_dest_begin);
    assert(res);
    assert(str_source);

    size_t cnt = 0;
    int source_copied = 0;
    while ( cnt + start_ind < str_dest_capacity - 1 ) // leaving space for '\0'
    {
        str_dest_begin[cnt + start_ind] = str_source[cnt];

        cnt++;

        if ( str_source[cnt] == '\0' )
        {
            source_copied = 1;
            break;
        }

    }

    str_dest_begin[str_dest_capacity] = '\0';

    if ( source_copied )
        *res = 1;
    else
        *res = 0;

    return cnt + start_ind;
}

#endif /* UTILS_H */
