#include <stdio.h>

#include "akinator.h"
#include "mystring.h"
#include "../../../mylibheaders/tree.h"
// TODO - поправить инклюды, чтобы их воспринимал компилятор (VS CODE все понимает уже)

int main()
{
    //Database database = init_and_load_database();

    size_t cap = 3;
    char *str = (char *) calloc(cap, sizeof(char));

    AkinatorStatus status = read_tree_node_data(stdin, &str, &cap);

    printf("chars: <%llu>, string: <%s>\n", cap, str);


    return 0;
}

