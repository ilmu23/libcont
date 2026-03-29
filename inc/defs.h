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
	STRING,
	INTEGER
}	map_key_type;

typedef uint8_t	(*lc_swapper)(void * const, void * const);
typedef void	*(*lc_copyer)(const void * const);

extern const lc_swapper	lc_simple_swap;
extern const lc_copyer	lc_simple_copy;
