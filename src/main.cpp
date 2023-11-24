#include <stdio.h>

#include "akinator.h"
#include "mystring.h"
#include "../../../mylibheaders/tree.h"
// TODO - поправить инклюды, чтобы их воспринимал компилятор (VS CODE все понимает уже)

int main()
{
    // Tree tree = read_tree_from_file("database_silm.txt");
    // TREE_DUMP(&tree, 0);

    Database database = init_and_load_database();

    main_loop(&database);

    database_dtor(&database);

    return 0;
}

