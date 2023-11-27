#include <stdio.h>

#include "akinator.h"
#include "mystring.h"
#include "../../../mylibheaders/tree.h"

int main()
{
    Database database = init_and_load_database();

    main_loop(&database);

    database_dtor(&database);

    return 0;
}

