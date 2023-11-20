#ifndef AKINATOR_H
#define AKINATOR_H

#include <memory.h>

#include "../../../mylibheaders/tree.h"

struct Database
{
    Tree tree = {};
    const char *file_name = NULL;
};

#define DEF_AKINATOR_STATUS(name, message) AKINATOR_STATUS_##name,
enum AkinatorStatus
{
    #include "akinator_statuses.h"
};
#undef DEF_AKINATOR_STATUS


const size_t FILE_NAME_DEFAULT_LEN = 64;

#define DEF_AKINATOR_STATUS(name, message) message,
const char * const akinator_status_messages[] =
{
    #include "akinator_statuses.h"
    "FICTIONAL MESSAGE!"
};
#undef DEF_AKINATOR_STATUS



Database init_and_load_database();

FILE *get_and_open_data_file();

Tree read_tree_from_opened_file(FILE *file);

TreeNode *read_tree_node(FILE *stream, AkinatorStatus *status);

void tree_data_dtor(void *data_ptr);

void tree_print_data(FILE *stream, void *data_ptr);

AkinatorStatus read_tree_node_data(FILE *stream, char **str_ptr, size_t *capacity_ptr);

#endif
