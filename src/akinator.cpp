#include "akinator.h"

#include <assert.h>

#include "utils.h"
#include "mystring.h"


Database init_and_load_database()
{
    printf( "Welcome! My name is Blaine the Mono, and I love to solve riddles. "
            "Do you want to load up an existing base of knowledge? [y/n]\n");

    int ans = 0;
    while ( (ans = getchar()) != 'y' && ans != 'n' )
    {
        clear_inp(stdin);
        printf( "That's neither a clear answer nor a good riddle, so "
                "I will ignore it. You can certainly try again, but I've "
                "already lost my faith in you.\n" );
    }
    clear_inp(stdin);

    if ( ans == 'y' )
    {
        FILE *data_file = get_and_open_data_file();



    }
    else if ( ans == 'n')
    {

    }
    else
        assert(0 && "Invalid input sneaked through");
}

FILE *get_and_open_data_file()
{
	printf( "Well, tell me than the name of the file, containing the database. "
			"The file must be created by me, for I'm not going to root around "
			"in someone's silly writings.\n" );
	char *file_name = (char*) calloc(FILE_NAME_DEFAULT_LEN, sizeof(char));
	size_t file_name_cap = FILE_NAME_DEFAULT_LEN;

	my_getline(&file_name, &file_name_cap, stdin);

	FILE *file = NULL;
	while ( !(file = fopen(file_name, "r")) )
	{
		printf( "Do not try to fool me, there is no file with such a name. "
				"Relieve me of your silly jokes. Try again.\n" );
		clear_inp(stdin);
		my_getline(&file_name, &file_name_cap, stdin);
	}
	clear_inp(stdin);

	free(file_name);

	return file;
}

Tree read_tree_from_opened_file(FILE *file)
{
	assert(file);

	Tree tree = {};
	tree_ctor(&tree, sizeof(char*), tree_data_dtor, tree_print_data);

	AkinatorStatus status = AKINATOR_STATUS_OK;
	TreeNode* root = read_tree_node(stdin, &status);
	if (!status)
	{
		printf( "You base of knowledge is corrupted, and so my "
				"patience has ended. Leave. Now.\n" );
		exit(0);
	}

	tree.root = root;

	return tree;
}

TreeNode *read_tree_node(FILE *stream, AkinatorStatus *status)
{
	skip_spaces(stream);
	if (getc(stream) == '(')
	{

	}
	else
	{
		ungetc('(', stream);

		// if not a '(', than it is certaily 'nil'
		fscanf(stream, "%*s");
		return NULL;
	}

	assert(0 && "Unreacheable line!");

	return NULL;
}

void tree_data_dtor(void *data_ptr)
{
	assert(data_ptr);
	free(data_ptr);
}

void tree_print_data(FILE *stream, void *data_ptr)
{
	assert(data_ptr);
	fprintf(stream, "\"%s\"", (char *) data_ptr);
}

AkinatorStatus read_tree_node_data(FILE *stream, char **str_ptr, size_t *capacity_ptr)
{
	assert(stream);
	assert(str_ptr);
	assert(*str_ptr);
	assert(*capacity_ptr > 2);

	int c = 0;

	c = getc(stream);
	if (c != '"')
		return AKINATOR_STATUS_ERROR_IN_DATABASE;

	char *char_ptr = *str_ptr;
	while ( 1 )
	{
		*char_ptr = (char) getc(stream);
		if ( *char_ptr == '"' )
		{
			*char_ptr = '\0';
			return AKINATOR_STATUS_OK;
		}

		char_ptr++;
		if ( (size_t) (char_ptr - *str_ptr) >= *capacity_ptr )
		{
			char *new_mem = (char*) realloc(*str_ptr, *capacity_ptr * 2);
			if (!new_mem)
				return AKINATOR_STATUS_ERROR_MEM_ALLOC;

			*str_ptr = new_mem;
			*capacity_ptr = *capacity_ptr * 2;
			char_ptr = *str_ptr + *capacity_ptr / 2;
		}
	}

	return AKINATOR_STATUS_OK;
}
