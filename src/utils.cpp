#include "utils.h"

#include "ctype.h"

void clear_inp(FILE *stream)
{
    while ( getc(stream) != '\n' )
        ;
    return;
}

void skip_spaces(FILE *stream)
{
    int c = 0;
    while(isspace(c = getc(stream)))
        ;
    ungetc(c, stream);
}

void skip_spaces(DataFile *data_file)
{
    while ( isspace(data_file->file_buf.buf[ data_file->pointer ]) )
        data_file->pointer++;
}

int is_dbl_zero(double x)
{
    return abs(x) < DBL_PRECISION;
}

int are_dbls_equal(double a, double b)
{
    return is_dbl_zero(a - b);
}
