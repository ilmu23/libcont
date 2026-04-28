// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<defs.h>>

#pragma once

#include <stdint.h>

typedef struct _darray *		darray;
typedef const struct _darray *	cdarray;

typedef struct _map *		map;
typedef const struct _map *	cmap;

typedef struct _lst *		list;
typedef const struct _lst *	clist;

typedef struct _lst_node *	list_node;

struct _lst_node {
	uint8_t	*data;
};

typedef enum {
	LC_MKEY_STRING,
	LC_MKEY_INTEGER
}	map_key_type;

typedef uint8_t	(*lc_swapper)(void * const, void * const);
typedef void	*(*lc_copyer)(const void * const);

typedef void	(*lc_map_element_fn)(const uintptr_t, void *, void *);
typedef void	(*lc_darray_element_fn)(void *, void *);
typedef void	(*lc_list_element_fn)(void *, void *);

extern const lc_swapper	lc_simple_swap;
extern const lc_copyer	lc_simple_copy;
