// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<map.h>>

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct _map *		map;
typedef const struct _map *	cmap;

typedef enum {
	STRING,
	INTEGER
}	map_key_type;

#define MAP_NOT_FOUND	((void *)1)

#define map(type, count, key_type, free)	(_map_new(sizeof(type), count, key_type, free))
map	_map_new(const size_t size, const size_t count, const map_key_type type, void (*free)(void *));

#define map_delete(map)	(_map_del(map))
void	_map_del(map map);

#define map_get(map, key)	(_map_get(map, (const uintptr_t)key))
void	*_map_get(cmap map, const uintptr_t key);

#define map_set(map, key, value)	(_map_set(map, (const uintptr_t)key, (const void *)&value))
uint8_t	_map_set(map map, const uintptr_t key, const void *val);

#define map_erase(map, key)	(_map_ers(map, (const uintptr_t)key))
uint8_t	_map_ers(map map, const uintptr_t key);

#define map_size(map)	(_map_sze(map))
size_t	_map_sze(cmap map);

#define map_empty(map)	(_map_ety(map))
uint8_t	_map_ety(cmap map);

#define map_foreach(map, fn)	(_map_fea(map, fn))
void	_map_fea(map map, void (*fn)(void *));

#define map_clear(map)	(_map_clr(map))
void	_map_clr(map map);
