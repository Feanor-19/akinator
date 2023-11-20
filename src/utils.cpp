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

