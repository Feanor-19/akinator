#ifndef AKINATOR_H
#define AKINATOR_H

#include <memory.h>

#include "../../../mylibheaders/tree.h"
#include "..\..\..\mylibheaders\onegin.h"

struct Database
{
    Tree tree = {};
    char *file_name = NULL;
};

struct DataFile
{
    FileBuf file_buf = {};
    size_t pointer = 0;
};

enum WhichChild
{
    CHILD_LEFT,
    CHILD_RIGHT,
};

enum UserAns
{
    NO,
    YES,
};

#define DEF_AKINATOR_STATUS(name, message) AKINATOR_STATUS_##name,
enum AkinatorStatus
{
    #include "akinator_statuses.h"
};
#undef DEF_AKINATOR_STATUS


const size_t FILE_NAME_DEFAULT_LEN      = 64;
const size_t TREE_NODE_DATA_DEFAULT_LEN = 32;
const char * const UNKNOWN_WHO = "Unknown who";

#define DEF_AKINATOR_STATUS(name, message) message,
const char * const akinator_status_messages[] =
{
    #include "akinator_statuses.h"
    "FICTIONAL MESSAGE!"
};
#undef DEF_AKINATOR_STATUS



Database init_and_load_database();

void database_dtor( Database *database_ptr );

Database create_empty_database();

char *get_data_file_name();

Tree read_tree_from_file(const char *file_name);

void read_root_node(DataFile *file, Tree *tree_ptr, AkinatorStatus *status);

void read_tree_node(DataFile *file,
                    Tree *tree_ptr,
                    TreeNode *parent_node,
                    WhichChild child,
                    AkinatorStatus *status);

void tree_data_dtor(void *data_ptr);

void tree_print_data(FILE *stream, void *data_ptr);

AkinatorStatus read_tree_node_data(DataFile *file, char **str_ptr, size_t *capacity_ptr);

void print_err_message_and_exit(AkinatorStatus err);

int datafile_getchar(DataFile *file);

void datafile_ungetc(DataFile *file);

UserAns get_ans_yes_or_no();

#endif
