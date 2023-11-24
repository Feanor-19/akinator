#ifndef AKINATOR_H
#define AKINATOR_H

#include <memory.h>

#include "..\..\..\mylibheaders\tree.h"
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
    GUESS,
    DEFINITION,
    COMPARE,
    SHOW_DATABASE,
    LEAVE,
};

struct ObjDefinition
{
    int *path               = NULL;
    size_t path_len         = 0;
    TreeNode **nodes_path    = NULL;
};

#define DEF_AKINATOR_STATUS(name, message) AKINATOR_STATUS_##name,
enum AkinatorStatus
{
    #include "akinator_statuses.h"
};
#undef DEF_AKINATOR_STATUS


const size_t FILE_NAME_DEFAULT_LEN  = 64;
const size_t STR_DEFAULT_LEN        = 32;
const char * const UNKNOWN_WHO      = "Unknown who";

#define DATABASE_IMG_NAME                 "tmp.jpg"
#define DATABASE_DOT_NAME                 "tmp.dot"

#define DEF_AKINATOR_STATUS(name, message) message,
const char * const akinator_status_messages[] =
{
    #include "akinator_statuses.h"
    "FICTIONAL MESSAGE!"
};
#undef DEF_AKINATOR_STATUS



Database init_and_load_database();

void database_dtor( Database *database_ptr );

void main_loop( Database *database );

Database create_empty_database();

char *get_data_file_name(const char *file_mode);

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

UserAns get_ans_main_loop();

AkinatorStatus choice_guess(Database *database);

AkinatorStatus guess(Database *database, TreeNode *node_ptr);

AkinatorStatus add_object_to_database(Database *database, TreeNode *node_ptr);

AkinatorStatus choice_definition(Database *database);

ObjDefinition find_obj(Database *database, const char *str_to_find);

TreeNode *find_node(Database *database, const char *str_to_find);

AkinatorStatus choice_compare(Database *database);

AkinatorStatus choice_show_database(Database *database);

void choice_leave(Database *database);

AkinatorStatus write_database_to_file(Database *database);

AkinatorStatus write_tree_node_to_file(FILE *file, TreeNode *node_ptr);

FILE *create_and_open_dot_file();

AkinatorStatus write_dot_file( FILE *dot_file, Database *database );

AkinatorStatus run_dot_to_create_database_img();

AkinatorStatus show_database_img();

AkinatorStatus delete_tmp_files();

char * get_str_from_node_data(void *data_ptr);

char * get_str_from_user();

void free_obj_def(ObjDefinition *def_ptr);

#endif
