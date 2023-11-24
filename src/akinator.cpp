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
		printf( "Well, tell me than the name of the file, containing the database. "
			"The file must be created by me, for I'm not going to root around "
			"in someone's silly writings.\n" );
        char *data_file_name = get_data_file_name("r");

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
	assert(database_ptr);

	tree_dtor(&database_ptr->tree);

	if ( database_ptr->file_name )
		free(database_ptr->file_name);

	*database_ptr = {};
}

AkinatorStatus main_loop( Database *database )
{
	assert(database);

	printf(	"So, what exactly do you want to do? "
			"Play the [G]uess game with me, "
			"learn a d[e]finition of some object from the database, "
			"[c]ompare two objects, s[h]ow database "
			"or just ra[pi]dly leave?\nChoose wisely!\n");

	while (1)
	{
		UserAns ans = get_ans_main_loop();
		clear_inp(stdin);

		switch (ans)
		{
		case GUESS:
			choice_guess(database);
			break;
		case DEFINITION:
			choice_definition(database);
			break;
		case COMPARE:
			choice_compare(database);
			break;
		case SHOW_DATABASE:
			choice_show_database(database);
			break;
		case LEAVE:
			choice_leave(database);
			return AKINATOR_STATUS_OK;
			break;
		case NO:
		case YES:
		default:
			assert(0);
			break;
		}

		printf(	"So, what now? Choose from the same range of options. "
				"No, I won't remind them for you. I am not a parrot, "
				"but an advanced AI.\n");
	}

	assert(0);
	return AKINATOR_STATUS_OK;
}

UserAns get_ans_main_loop()
{
	const double ANS_G 	= 6.674E-11;
	const double ANS_E 	= 2.718;
	const double ANS_C	= 2.997E8;
	const double ANS_H 	= 6.626E-34;
	const double ANS_PI	= 3.141;

	while (1)
	{
		double ans = 0;
		if ( scanf("%lf", &ans) == 1)
		{
			if 		( are_dbls_equal(ans, ANS_G) )
				return GUESS;
			else if ( are_dbls_equal(ans, ANS_E) )
				return DEFINITION;
			else if ( are_dbls_equal(ans, ANS_C) )
				return COMPARE;
			else if ( are_dbls_equal(ans, ANS_H) )
				return SHOW_DATABASE;
			else if ( are_dbls_equal(ans, ANS_PI) )
				return LEAVE;
		}

		printf(	"Well, I see you aren't that smart. In case "
				"you still haven't got what to type to express your choice, "
				"I deign to explain it. Just type values of math and physics "
				"constants, written in corresponding square brackets. For example, "
				"if you wanted to choose [h], which is Planck constant, you would "
				"type 6.626E-34. Yes, always leave just three digits - truncated, "
				"not rounded. Good luck...\n");
		clear_inp(stdin);
	}

	assert(0);
	return NO;
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

char *get_data_file_name(const char *file_mode)
{

	char *file_name = (char*) calloc(FILE_NAME_DEFAULT_LEN, sizeof(char));
	size_t file_name_cap = FILE_NAME_DEFAULT_LEN;

	my_getline(&file_name, &file_name_cap, stdin);
	file_name[ strlen(file_name) - 1] = '\0';

	FILE *file = NULL;
	while ( !(file = fopen(file_name, file_mode)) )
	{
		printf( "Do not try to fool me, there is no file with such a name. "
				"Relieve me of your silly jokes. Try again.\n" );
		my_getline(&file_name, &file_name_cap, stdin);
		file_name[ strlen(file_name) - 1] = '\0';
	}

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
	assert(file);
	if ( file->pointer >= file->file_buf.buf_size )
		return -1;

	return file->file_buf.buf[ file->pointer++ ];
}

void datafile_ungetc(DataFile *file)
{
	assert(file);
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

	size_t cap = STR_DEFAULT_LEN;
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
		size_t cap = STR_DEFAULT_LEN;
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
	free( get_str_from_node_data(data_ptr) );
}

void tree_print_data(FILE *stream, void *data_ptr)
{
	assert(data_ptr);
	assert(stream);
	fprintf(stream, "\"%s\"", get_str_from_node_data(data_ptr) );
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

	for (int i = 5; i > 0; i--)
	{
		sleep(1);
		printf("%d\n", i);
		fflush(stdout);
	}

	printf("BOOM\n");
	fflush(stdout);

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

AkinatorStatus choice_show_database(Database *database)
{
	assert(database);

	FILE *file = create_and_open_dot_file();
	if (!file)
		return AKINATOR_STATUS_ERROR_CANT_CREATE_IMG_FILE;

	write_dot_file( file, database );

	fclose(file);

	run_dot_to_create_database_img();

	show_database_img();

	sleep(1);

	delete_tmp_files();

	return AKINATOR_STATUS_OK;
}

inline void create_new_database_file(Database *database)
{
	assert(database);

	printf( "Tell me the name of the file to be created. "
			"And don't be silly this time, I'm pretty "
			"tired of you.\n");
	database->file_name = get_data_file_name("w");
}

AkinatorStatus choice_leave(Database *database)
{
	assert(database);

	printf(	"Well, thats not a wise choice, but I "
			"can't decide for you! Do you want to save "
			"changes in the database? [y/n]\n");
	UserAns ans = get_ans_yes_or_no();
	if (ans)
	{
		if ( database->file_name )
		{
			printf( "Do you want to rewrite the file? [y/n]\n"
			"If you answer 'no', a new file will be created.\n");

			ans = get_ans_yes_or_no();
			if (!ans)
			{
				create_new_database_file(database);
			}
		}
		else
		{
			create_new_database_file(database);
		}

		write_database_to_file( database );

		printf("Done!\n");
	}

	database_dtor(database);
	print_err_message_and_exit(AKINATOR_STATUS_ERROR_CANT_LEAVE_RUNNING_TRAIN);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus choice_compare(Database *database)
{
	assert(database);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus choice_definition(Database *database)
{
	assert(database);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus choice_guess(Database *database)
{
	assert(database);

	printf( "So, the rules are simple, even YOU are able to "
			"understand them. You think of an object, I guess it, "
			"according to your database. I'll ask you questions, "
			"you'll answer just 'yes' or 'no'. Well, let's begin!\n");

	guess(database, tree_get_root(&database->tree) );

	return AKINATOR_STATUS_OK;
}

AkinatorStatus guess(Database *database, TreeNode *node_ptr)
{
	assert(database);
	assert(node_ptr);

	if ( !is_node_leaf(node_ptr) )
	{
		printf( "Is the following statement true about "
				"your object? \"The object %s\" [y/n]\n",
				get_str_from_node_data(node_ptr->data_ptr) );

		UserAns ans = get_ans_yes_or_no();
		if (ans)
			guess(database, tree_get_right_child( node_ptr ) );
		else
			guess(database, tree_get_left_child( node_ptr ) );
	}
	else
	{
		printf( "Is it \"%s\"? [y/n]\n", get_str_from_node_data(node_ptr->data_ptr));

		UserAns ans = get_ans_yes_or_no();
		if (ans)
		{
			printf( "It was very easy. You are so boring... "
					"Why did you come, and not someone smarter! "
					"Anyway, we're already on our way, is is too late "
					"to change anything.\n");
		}
		else
		{
			printf( "This pathetic database doesn't have any suitable records. "
					"Well, we can make it a little less miserable by adding "
					"the object you've thought of. Do you want to do it? [y/n]\n");

			ans = get_ans_yes_or_no();
			if (ans)
			{
				add_object_to_database(database, node_ptr);
			}
		}
	}

	return AKINATOR_STATUS_OK;
}

AkinatorStatus add_object_to_database(Database *database, TreeNode *node_ptr)
{
	assert(database);
	assert(node_ptr);

	printf( "So, name the object.\n");

	char *str = get_str_from_user();

	tree_insert_data_as_right_child(&database->tree, node_ptr, &str);

	str = (char*) calloc( strlen( get_str_from_node_data(node_ptr->data_ptr) ) + 1, sizeof(char) );
	strcpy(str, get_str_from_node_data(node_ptr->data_ptr) );

	tree_insert_data_as_left_child(&database->tree, node_ptr, &str);

	printf( "Listen carefully, it can be too hard for you to understand "
			"from the first time, but I'll try to explain it in a very easy way. "
			"Complete the following sentence in a such way that "
			"it unambiguously differs your object from \"%s\": "
			"\"The object...\"\n", get_str_from_node_data(node_ptr->data_ptr) );

	str = get_str_from_user();

	tree_change_data(&database->tree, node_ptr, &str);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus write_database_to_file(Database *database)
{
	assert(database);

	FILE *file = fopen(database->file_name, "w");
	if (!file)
		return AKINATOR_STATUS_ERROR_CANT_SAVE_DATABASE;

	write_tree_node_to_file( file, tree_get_root(&database->tree) );

	fclose(file);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus write_tree_node_to_file(FILE *file, TreeNode *node_ptr)
{
	if (!node_ptr)
	{
		fprintf(file, " nil ");
	}
	else
	{
		fprintf(file, "( \"%s\" ", get_str_from_node_data(node_ptr->data_ptr) );
		write_tree_node_to_file( file, tree_get_left_child(node_ptr) );
		write_tree_node_to_file( file, tree_get_right_child(node_ptr) );
		fprintf(file, " ) ");
	}

	return AKINATOR_STATUS_OK;
}

FILE *create_and_open_dot_file()
{
	return fopen(DATABASE_DOT_NAME, "w");
}

AkinatorStatus run_dot_to_create_database_img()
{
	system("dot " DATABASE_DOT_NAME " -Tjpg -o " DATABASE_IMG_NAME);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus write_dot_file( FILE *dot_file, Database *database )
{
	assert(dot_file);
	assert(database);

#define COLOR_BG            "#2D4059"
#define COLOR_NODE_COLOR    "#ECC237"
#define COLOR_NODE_FILL     "#EA5455"
#define COLOR_LABEL_COLOR   "#EA5455"
#define COLOR_LABEL_FILL    "#ECC237"
#define COLOR_EDGE_LEFT     "#F07B3F"
#define COLOR_EDGE_RIGHT    "#FFD460"
#define COLOR_EDGE          "#8ccb5e"


    // Dot header
    fprintf(dot_file,   "digraph{\n"
                        "splines=ortho;\n"
                        "bgcolor=\"" COLOR_BG "\";"
                        "\n\n\n");


    // Nodes with data
    TreeNode *curr_node = database->tree.head_of_all_nodes;
    TreeNode **nodes_arr = (TreeNode**) calloc( database->tree.nodes_count, sizeof(TreeNode) );
    for (size_t ind = 0; ind < database->tree.nodes_count; ind++)
    {
        if ( curr_node == NULL )
        {
            fprintf(stderr, "ERROR: something wrong with listing of nodes!\n");
            break;
        }

        fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                            "style=bold, style=filled,\n"
                            "color=\"" COLOR_NODE_COLOR "\", fillcolor=\"" COLOR_NODE_FILL "\",\n"
                            "label = ",
                            ind);
        database->tree.print_data_func_ptr(dot_file, curr_node->data_ptr);
        fprintf(dot_file,   "];\n\n");

        nodes_arr[ind] = curr_node;
        curr_node = curr_node->next;
    }


    // Edges
    for (size_t ind = 0; ind < database->tree.nodes_count; ind++)
    {
        size_t left = 0;
        size_t right = 0;
        for (size_t i = 0; i < database->tree.nodes_count; i++)
        {
            if ( nodes_arr[i] == nodes_arr[ind]->left )
            {
                left = i;
            }
            else if ( nodes_arr[i] == nodes_arr[ind]->right )
            {
                right = i;
            }
        }

        if ( nodes_arr[ind]->left )
        {
            fprintf(dot_file, "NODE_%llu->NODE_%llu"
							  "[color=\"" COLOR_EDGE_LEFT "\", "
							  "penwidth=2, fontcolor=\"" COLOR_EDGE_LEFT "\", "
							  "fontname=\"verdana bold\", xlabel=\"NO\"];\n",
                              ind, left);
        }

        if ( nodes_arr[ind]->right )
        {
            fprintf(dot_file, "NODE_%llu->NODE_%llu"
							  "[color=\"" COLOR_EDGE_RIGHT "\", "
							  "penwidth=2, fontcolor=\"" COLOR_EDGE_RIGHT "\", "
							  "fontname=\"verdana bold\", xlabel=\"YES\"];\n",
                              ind, right);
        }

        if ( nodes_arr[ind]->left && nodes_arr[ind]->right )
        {
            fprintf(dot_file,   "NODE_%llu->NODE_%llu[style=invis];\n"
                                "{rank=same NODE_%llu NODE_%llu}",
                                left, right, left, right);
        }
    }


    fprintf(dot_file, "\n}\n");

#undef COLOR_BG
#undef COLOR_OCCUP_NODE_COLOR
#undef COLOR_OCCUP_NODE_FILL
#undef COLOR_FREE_NODE_COLOR
#undef COLOR_FREE_NODE_FILL
#undef COLOR_LABEL_COLOR
#undef COLOR_LABEL_FILL
#undef COLOR_EDGE_PREV
#undef COLOR_EDGE_NEXT
#undef COLOR_EDGE_FREE

	free(nodes_arr);

    return AKINATOR_STATUS_OK;

}

AkinatorStatus show_database_img()
{
	system(DATABASE_IMG_NAME);

	return AKINATOR_STATUS_OK;
}

AkinatorStatus delete_tmp_files()
{
	system("del " DATABASE_DOT_NAME " " DATABASE_IMG_NAME);

	return AKINATOR_STATUS_OK;
}

char * get_str_from_node_data(void *data_ptr)
{
	assert(data_ptr);

	return *((char **) data_ptr);
}

char * get_str_from_user()
{
	size_t cap = STR_DEFAULT_LEN;
	char *str = (char*) calloc(cap, sizeof(char));
	while (1)
	{
		my_getline(&str, &cap, stdin);
		str[ strlen(str) - 1 ] = '\0';

		printf( "You entered \"%s\". Is that right? [y/n]\n", str);
		UserAns ans = get_ans_yes_or_no();
		if (ans)
			break;
		else
			printf( "Well, now don't be silly and enter what you want.\n");
	}
	return str;
}
