#ifndef FGDO_SEARCH_MANAGER_H
#define FGDO_SEARCH_MANAGER_H

#include "../searches/search.h"
#include "../searches/search_parameters.h"


/********
	*	Registry for known searches.
 ********/
typedef int (*init_search_type)(char*, SEARCH*);

typedef struct registered_search {
	char*			search_qualifier;
	init_search_type	init_search;
} REGISTERED_SEARCH;


/********
	*	Search manager functions.
 ********/
int get_generation_rate();

void init_search_manager(int argc, char** argv);
void register_search(char* search_name, init_search_type is);
int manage_search(char* search_name);

int generate_search_parameters(SEARCH_PARAMETERS **sp);
int insert_search_parameters(SEARCH_PARAMETERS *sp);

int get_qualifier_from_name(char* search_name, char **search_qualifier);

#endif