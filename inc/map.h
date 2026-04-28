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

#include "defs.h"
#include "alloc.h"

#define MAP_NOT_FOUND	((void *)1)

#define map(type, count, key_type, free)	(_map_new(sizeof(type), count, key_type, free))
map	_map_new(const size_t size, const size_t count, const map_key_type type, const lc_freer free);

#define map_delete(map)	(_map_del(map))
void	_map_del(map map);

#define map_get(map, key)			(_map_get(map, (const uintptr_t)key))
#define map_get_t(type, map, key)	(*(type *)_map_get(map, (const uintptr_t)key))
void	*_map_get(cmap map, const uintptr_t key);

#define map_set(map, key, value)	(_map_set(map, (const uintptr_t)key, (const void *)&value))
uint8_t	_map_set(map map, const uintptr_t key, const void *val);

#define map_erase(map, key)	(_map_ers(map, (const uintptr_t)key))
uint8_t	_map_ers(map map, const uintptr_t key);

#define map_size(map)	(_map_sze(map))
size_t	_map_sze(cmap map);

#define map_empty(map)	(_map_ety(map))
uint8_t	_map_ety(cmap map);

#define map_foreach(map, fn, fn_arg)	(_map_fea(map, (lc_foreach_fn)fn, fn_arg))
void	_map_fea(map map, const lc_foreach_fn fn, void *fn_arg);

#define map_set_free(map, free)	(_map_fre(map, free))
void	_map_fre(map map, const lc_freer free);

#define map_clear(map)	(_map_clr(map))
void	_map_clr(map map);
