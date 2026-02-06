// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<map.c>>

#include <stdlib.h>
#include <string.h>

#include "map.h"

#define _DELETED	((void *)2)

#define _HASH_SALT	347

#define _GROW_THRESHOLD	80

#define in_use(p)	(p && p != _DELETED)

#define wraparound_increment(x, max)	(x = (x < max) ? x + 1 : 0)

typedef struct {
	uintptr_t	key;
	uint8_t		val[];
}	pair;

struct _map {
	void			(*free)(void *);
	size_t			element_size;
	size_t			elements;
	size_t			capacity;
	map_key_type	key_type;
	pair			**data;
};

static inline uint64_t	_upow(const uint64_t x, uint64_t y);
static inline uint64_t	_hash(const char *s);
static inline uint8_t	_grow(map map);
static inline size_t	_find_pair(cmap map, const uintptr_t key);

static void	__free(void **blk);

map	_map_new(const size_t size, const size_t count, const map_key_type type, void (*_free)(void *)) {
	map	out;

	out = (size) ? malloc(sizeof(*out)) : NULL;
	if (out) {
		out->data = calloc(count, sizeof(*out->data));
		if (!out->data) {
			free(out);
			return NULL;
		}
		out->free = (_free != free) ? _free : (void (*)(void *))__free;
		out->element_size = size;
		out->capacity = count;
		out->key_type = type;
		out->elements = 0;
	}
	return out;
}

void	_map_del(map map) {
	if (map) {
		_map_clr(map);
		free(map->data);
		free(map);
	}
}

void	*_map_get(cmap map, const uintptr_t key) {
	size_t	i;

	i = _find_pair(map, (map->key_type == STRING) ? _hash((const char *)key) : key);
	return (in_use(map->data[i])) ? map->data[i]->val : MAP_NOT_FOUND;
}

uint8_t	_map_set(map map, const uintptr_t key, const void *val) {
	uintptr_t	_key;
	size_t		i;

	if ((map->elements * 100) / map->capacity > _GROW_THRESHOLD && !_grow(map))
		return 0;
	_key = (map->key_type == STRING) ? _hash((const char *)key) : key;
	i = _key % map->capacity;
	while (in_use(map->data[i]))
		wraparound_increment(i, map->capacity - 1);
	if (!in_use(map->data[i])) {
		map->data[i] = malloc(sizeof(*map->data[i]) + map->element_size);
		if (!map->data[i])
			return 0;
	}
	map->data[i]->key = key;
	memcpy(map->data[i]->val, val, map->element_size);
	map->elements++;
	return 1;
}

uint8_t	_map_ers(map map, const uintptr_t key) {
	size_t	i;

	i = _find_pair(map, key);
	if (!in_use(map->data[i]))
		return 0;
	if (map->free)
		map->free(map->data[i]->val);
	free(map->data[i]);
	map->data[i] = _DELETED;
	map->elements--;
	return 1;
}

size_t	_map_sze(cmap map) {
	return map->elements;
}

uint8_t	_map_ety(cmap map) {
	return (map->elements == 0) ? 1 : 0;
}

void	_map_fea(map map, void (*fn)(void *)) {
	size_t	i;

	for (i = 0; i < map->capacity; i++) {
		if (in_use(map->data[i]))
			fn(&map->data[i]->val);
	}
}

void	_map_clr(map map) {
	size_t	i;

	for (i = 0; i < map->capacity; i++) {
		if (in_use(map->data[i])) {
			if (map->free)
				map->free(map->data[i]->val);
			free(map->data[i]);
			map->data[i] = _DELETED;
			if (!--map->elements)
				break ;
		}
	}
}

static inline uint64_t	_upow(const uint64_t x, uint64_t y) {
	uint64_t	out;

	if (y < 1)
		return 1;
	out = 1;
	while (y--)
		out *= x;
	return out;
}

static inline uint64_t	_hash(const char *s) {
	uint64_t	hash;
	size_t		len;
	size_t		i;

	for (i = hash = 0, len = strlen(s); i < len; i++) {
		hash += _upow(_HASH_SALT, len - i + 1) * s[i];
		hash %= UINTPTR_MAX;
	}
	return hash;
}

static inline uint8_t	_grow(map map) {
	size_t	i;
	size_t	j;
	size_t	new_capacity;
	pair	**new_data;

	new_capacity = map->capacity * 2;
	new_data = calloc(new_capacity, sizeof(*new_data));
	if (!new_data)
		return 0;
	for (i = 0; i < map->capacity; i++) {
		if (in_use(map->data[i])) {
			j = map->data[i]->key % new_capacity;
			while (new_data[j])
				wraparound_increment(j, new_capacity - 1);
			new_data[j] = map->data[i];
		}
	}
	free(map->data);
	map->capacity = new_capacity;
	map->data = new_data;
	return 1;
}

static inline size_t	_find_pair(cmap map, const uintptr_t key) {
	size_t	i;

	for (i = key % map->capacity; map->data[i]; i = (i < map->capacity - 1) ? i + 1 : 0)
		if (map->data[i] != _DELETED && map->data[i]->key == key)
			break ;
	return i;
}

static void	__free(void **blk) {
	free(*blk);
}
