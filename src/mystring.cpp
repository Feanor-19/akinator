#include <assert.h>
#include <stdio.h>

#include "mystring.h"

size_t my_strlen(const char *s)
{
    assert(s != NULL);
    int len = 0;
    while (*(s++) != '\0') len++;
    return len;
}

int my_puts(const char *s)
{
    assert(s != NULL);
    int cnt = 0;
    while (*s != '\0')
    {
        if (putc(*s, stdout) == EOF)
        {
            return EOF;
        }
        s++;
        cnt++;
    }
    putc('\n', stdout);
    return cnt;
}

const char * my_strchr( const char * s, int symbol)
{
    assert(s != NULL);
    while ( *s != symbol && *s != '\0' ) s++;

    if (*s == '\0')
    {
        if (symbol == '\0')
        {
            return s;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return s;
    }
}

char * my_strchr( char * s, int symbol)
{
    assert(s != NULL);
    while ( *s != symbol && *s != '\0' ) s++;

    if (*s == '\0')
    {
        if (symbol == '\0')
        {
            return s;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return s;
    }
}

char * my_strcpy( char * destptr, const char * srcptr )
{
    assert(destptr != NULL);
    assert(srcptr != NULL);
    assert(destptr != srcptr);

    while (*srcptr != '\0') *(destptr++) = *(srcptr++);
    *destptr = '\0';

    return destptr;
}

char * my_strncpy( char * destptr, const char * srcptr, size_t num )
{
    assert(destptr != NULL);
    assert(srcptr != NULL);
    assert(destptr != srcptr);

    char * res = destptr;
    while (num-- && *srcptr != '\0' && *destptr != '\0')
    {
        *(destptr++) = *(srcptr++);
    }

    *destptr = '\0';

    if ( *srcptr == '\0' && num > 0)
    {
        *destptr = '\0';
        return res;
    }

    return res;
}

char * my_strcat( char * destptr, const char * srcptr )
{
    assert(destptr != NULL);
    assert(srcptr != NULL);
    assert(destptr != srcptr);

    char *res = destptr;

    destptr = my_strchr(destptr, '\0');

    while (*srcptr != '\0')
    {
        *(destptr++) = *(srcptr++);
    }

    *destptr = '\0';

    return res;
}

char * my_strncat( char * destptr, char * srcptr, size_t num )
{
    assert(destptr != NULL);
    assert(srcptr != NULL);
    assert(destptr != srcptr);

    char *res = destptr;

    destptr = my_strchr(destptr, '\0');

    while (*srcptr != '\0' && num--)
    {
        *(destptr++) = *(srcptr++);
    }

    *destptr = '\0';

    return res;
}

char * my_fgets( char * s, int num, FILE * filestream )
{
    assert(s != NULL);
    assert(filestream != NULL);
    assert(num >= 0);

    int c = '\0';

    char *res = s;

    while ( (c = getc(filestream) ) != EOF && c != '\n' && num--)
    {
        *(s++) = (char) c;
    }

    if (num == 0)
    {
        *s = '\0';
        return res;
    }

    if (c == '\n')
    {
        *s = '\n';
        s++;
    }

    *s = '\0';

    if (c == EOF && res == s)
    {
        return NULL;
    }

    return res;
}

char *my_strdup(const char *s)
{
    assert(s != NULL);

    size_t s_len = my_strlen(s);

    char *new_s = (char*) calloc( s_len + 1, sizeof(char) );

    if (new_s == NULL) return NULL;

    char *p = new_s;
    while (*s != '\0')
    {
        *(p++) = *(s++);
    }

    *p = '\0';

    return new_s;
}

size_t my_getline(char **lineptr, size_t *n, FILE *stream)
{
    assert(lineptr != NULL);
    assert(*lineptr != NULL);
    assert(n != NULL);
    assert(*n > 1);

    const size_t MEM_MULTIPLIER = 2;

    int c;
    size_t read_bytes = 0; //количество прочитанных "байтов" (символов)
    size_t free_mem = *n; //количество оставшейся свободной памяти в "байтах"
    char *p_char = *lineptr; //указатель на рассматриваемый сейчас символ в строке

    while ( (c = getc(stream)) != EOF && c != '\n' )
    {
        *(p_char++) = (char) c;
        read_bytes++;
        free_mem--;

        if (free_mem == 2) //нужно оставлять достаточно памяти для '\n' и '\0'
        {
            free_mem = MEM_MULTIPLIER * (*n) - (*n); // выделяем больше памяти
            *n = MEM_MULTIPLIER * (*n);
            char *new_mem = (char *) calloc( *n, sizeof(char) );

            // копируем старую строку на новое место
            my_strcpy(new_mem, *lineptr);

            // освобождаем старую память
            free(*lineptr);

            // теперь lineptr хранит указатель на указатель на новую память
            *lineptr = new_mem;

            // теперь надо p_char перенаправить на первый свободный символ новой памяти
            p_char = *lineptr + read_bytes;
        }
    }

    //на данный момент есть как минимум два свободных байта памяти

    if (c == '\n')
    {
        *(p_char++) = '\n';
        read_bytes++;

        *p_char = '\0';
        read_bytes++; // по ТЗ нуль-символ тоже должен учитываться

        return read_bytes;
    }
    else if (c == EOF)
    {
        if (read_bytes == 0) return -1;

        // По ТЗ если встретили EOF, но что-то прочитали, нужно вернуть без символа новой строки
        *p_char = '\0';
        read_bytes++;
        return read_bytes;
    }

    assert(0 && "Unreachable line!");

}
