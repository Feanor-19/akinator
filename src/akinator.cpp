#include "akinator.h"

#include <assert.h>
#include <unistd.h>

#include "utils.h"
#include "mystring.h"


Database init_and_load_database()
{
    printf( "Welcome! My name is Blaine the Mono, and I love to solve riddles. "
            "Do you want to load up an existing base of knowledge? [y/n]\n");

    UserAns ans = get_ans_yes_or_no();

    if ( ans )
    {
        char *data_file_name = get_data_file_name();

		Tree tree = read_tree_from_file(data_file_name);

		return { tree, data_file_name };
    }
    else
    {
		printf( "Well, then a new one will be created! It's always so thrilling "
				"to start something new...\n");

		return create_empty_database();
    }
}

void database_dtor( Database *database_ptr )
{
	tree_dtor(&database_ptr->tree);

	if ( database_ptr->file_name )
		free(database_ptr->file_name);

	*database_ptr = {};
}

Database create_empty_database()
{
	Tree tree = {};
	tree_ctor(&tree, sizeof(char *), tree_data_dtor, tree_print_data);

	char *str = (char*) calloc( strlen(UNKNOWN_WHO) + 1, sizeof(char) );
	strcpy(str, UNKNOWN_WHO);

	tree_insert_root(&tree, &str);

	return {tree, NULL};
}

char *get_data_file_name()
{
	printf( "Well, tell me than the name of the file, containing the database. "
			"The file must be created by me, for I'm not going to root around "
			"in someone's silly writings.\n" );
	char *file_name = (char*) calloc(FILE_NAME_DEFAULT_LEN, sizeof(char));
	size_t file_name_cap = FILE_NAME_DEFAULT_LEN;

	my_getline(&file_name, &file_name_cap, stdin);
	file_name[ strlen(file_name) - 1] = '\0';

	FILE *file = NULL;
	while ( !(file = fopen(file_name, "r")) )
	{
		printf( "Do not try to fool me, there is no file with such a name. "
				"Relieve me of your silly jokes. Try again.\n" );
		my_getline(&file_name, &file_name_cap, stdin);
		file_name[ strlen(file_name) - 1] = '\0';
	}
	//clear_inp(stdin);

	fclose(file);

	return file_name;
}

Tree read_tree_from_file(const char *file_name)
{
	assert(file_name);

	AkinatorStatus err = AKINATOR_STATUS_OK;

	ErrorCodes err_file = ERROR_NO;
	FileBuf file_buf = read_file_to_buf(file_name, &err_file);
	if (err_file)
		print_err_message_and_exit(AKINATOR_STATUS_ERROR_CANT_READ_DATA_FILE);

	DataFile data_file = {file_buf, 0};

	Tree tree = {};
	tree_ctor(&tree, sizeof(char*), tree_data_dtor, tree_print_data);

	read_root_node(&data_file, &tree, &err);
	if (err)
		print_err_message_and_exit(err);

	read_tree_node(&data_file, &tree, tree_get_root(&tree), CHILD_LEFT, &err);
	read_tree_node(&data_file, &tree, tree_get_root(&tree), CHILD_RIGHT, &err);
	if (err)
		print_err_message_and_exit(err);

	return tree;
}

int datafile_getchar(DataFile *file)
{
	if ( file->pointer >= file->file_buf.buf_size )
		return -1;

	return file->file_buf.buf[ file->pointer++ ];
}

void datafile_ungetc(DataFile *file)
{
	file->pointer--;
}

void read_root_node(DataFile *file, Tree *tree_ptr, AkinatorStatus *status)
{
	assert(file);
	assert(tree_ptr);
	assert(status);

	skip_spaces(file);
	int c = 0;
	if ( ( c = datafile_getchar(file) ) != '(' )
	{
		*status = AKINATOR_STATUS_ERROR_IN_DATABASE;
		return;
	}

	size_t cap = TREE_NODE_DATA_DEFAULT_LEN;
	char *str = (char *) calloc(cap, sizeof(char));
	*status = read_tree_node_data(file, &str, &cap);
	if ((*status))
	{
		free(str);
		return;
	}

	skip_spaces(file);

	TreeStatus tree_status = tree_insert_root(tree_ptr, &str);
	if (tree_status)
	{
		*status = AKINATOR_STATUS_ERROR_TREE_ERROR;
		return;
	}
}

inline void skip_word(DataFile *file)
{
	assert(file);
	int c = 0;
	while ( (c = datafile_getchar(file)) != ' ' && c != -1 )
		;
}

void read_tree_node(DataFile *file,
                    Tree *tree_ptr,
                    TreeNode *parent_node,
                    WhichChild child,
                    AkinatorStatus *err)
{
	assert(file);
	assert(tree_ptr);
	assert(parent_node);
	assert(err);

	skip_spaces(file);
	if (datafile_getchar(file) == '(')
	{
		skip_spaces(file);
		size_t cap = TREE_NODE_DATA_DEFAULT_LEN;
    	char *str = (char *) calloc(cap, sizeof(char));

		*err = read_tree_node_data(file, &str, &cap);
		if (*err)
		{
			free(str);
			return;
		}

		switch (child)
		{
		case CHILD_LEFT:
			tree_insert_data_as_left_child(tree_ptr, parent_node, &str);
			read_tree_node(file, tree_ptr, tree_get_left_child(parent_node), CHILD_LEFT, err);
			if (*err)
				return;
			read_tree_node(file, tree_ptr, tree_get_left_child(parent_node), CHILD_RIGHT, err);
			if (*err)
				return;
			break;
		case CHILD_RIGHT:
			tree_insert_data_as_right_child(tree_ptr, parent_node, &str);
			read_tree_node(file, tree_ptr, tree_get_right_child(parent_node), CHILD_LEFT, err);
			if (*err)
				return;
			read_tree_node(file, tree_ptr, tree_get_right_child(parent_node), CHILD_RIGHT, err);
			if (*err)
				return;
			break;
		default:
			assert(0);
			break;
		}

		skip_spaces(file);

		datafile_getchar(file); // ')'

		skip_spaces(file);

		return;
	}
	else
	{
		// if not a '(', than it is certaily 'nil'
		skip_word(file);
		return;
	}

	assert(0 && "Unreacheable line!");

	return;
}

void tree_data_dtor(void *data_ptr)
{
	assert(data_ptr);
	free( *((char **) (data_ptr)) );
}

void tree_print_data(FILE *stream, void *data_ptr)
{
	assert(data_ptr);
	fprintf(stream, "\"%s\"", *((char **) (data_ptr)));
}

AkinatorStatus read_tree_node_data(DataFile *file, char **str_ptr, size_t *capacity_ptr)
{
	assert(file);
	assert(str_ptr);
	assert(*str_ptr);
	assert(*capacity_ptr > 2);

	int c = 0;

	skip_spaces(file);

	c = datafile_getchar(file);
	if (c != '"')
		return AKINATOR_STATUS_ERROR_IN_DATABASE;

	char *char_ptr = *str_ptr;
	while ( 1 )
	{
		*char_ptr = (char) datafile_getchar(file);
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

	skip_spaces(file);

	return AKINATOR_STATUS_OK;
}

void print_err_message_and_exit(AkinatorStatus err)
{
	printf("Something went wrong: <%s>.\n Starting preparations for self-destruction...\n",
			akinator_status_messages[(int) err]);

	for (int i = 5; i <= 0; i--)
	{
		sleep(1);
		printf("%d\n", i);
	}

	abort();
}

UserAns get_ans_yes_or_no()
{
	int ans = 0;
    while ( (ans = getchar()) != 'y' && ans != 'n' )
    {
        if (ans != '\n')
			clear_inp(stdin);
        printf( "That's neither a clear answer nor a good riddle, so "
                "I will ignore it. You can certainly try again, but I've "
                "already lost my faith in you.\n" );
    }
    clear_inp(stdin);

	return (ans == 'y' ? YES : NO);
}
