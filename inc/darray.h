// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<darray.h>>

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct _darray *		darray;
typedef const struct _darray *	cdarray;

#define DARRAY_OUT_OF_BOUNDS	((void *)1)

#define darray(type, count, free)	(_dar_new(sizeof(type), count, free))
darray	_dar_new(const size_t size, const size_t count, void (*free)(void *));

#define darray_copy(darray, cpy)					(_dar_cpy(darray, 0, SIZE_MAX, cpy))
#define darray_copy_range(darray, start, end, cpy)	(_dar_cpy(darray, start, end, cpy))
darray	_dar_cpy(cdarray arr, const size_t start, const size_t end, void *(*cpy)(void *));

#define darray_delete(darray)	(_dar_del(darray))
void	_dar_del(darray arr);

#define darray_push(darray, value)	(_dar_psh(darray, (const void *)&value))
uint8_t	_dar_psh(darray arr, const void *val);

#define darray_pop(darray)	(_dar_pop(darray))
void	_dar_pop(darray arr);

#define darray_first(darray)	(_dar_get(darray, 0))
#define darray_start(darray)	(darray_first(darray))
#define darray_last(darray)	(_dar_get(darray, (size_t)-1))
#define darray_end(darray)	(darray_last(darray))
#define darray_get(darray)	(_dar_get(darray, i))
void	*_dar_get(cdarray arr, const size_t i);

#define darray_set(darray, i, value)		(_dar_set(darray, i, (const void *)&value, 0))
#define darray_replace(darray, i, value)	(_dar_set(darray, i, (const void *)&value, 1))
uint8_t	_dar_set(darray arr, const size_t i, const void *val, const uint8_t free);

#define darray_swap(darray, i1, i2, swapper)	(_dar_swp(darray, i1, i2, swapper))
uint8_t	_dar_swp(darray arr, const size_t i1, const size_t i2, uint8_t (*swap)(void *, void *));

#define darray_size(darray)		(_dar_sze(darray))
#define darray_length(darray)	(darray_size(darray))
size_t	_dar_sze(cdarray arr);

#define darray_capacity(darray)	(_dar_cap(darray))
size_t	_dar_cap(cdarray arr);

#define darray_resize(darray, size)	(_dar_rsz(darray, size))
uint8_t	_dar_rsz(darray arr, const size_t size);

#define darray_shrink_to_fit(darray)	(_dar_stf(darray))
uint8_t	_dar_stf(darray arr);

#define darray_clear(darray)	(_dar_clr(darray))
void	_dar_clr(darray arr);

#define darray_insert(darray, i, value)				(_dar_ins(darray, i, 1), (const void *)&value)
#define darray_insert_n(darray, start, n, values)	(_dar_ins(darray, start, n, (const void *)values))
uint8_t	_dar_ins(darray arr, const size_t i, const size_t n, const void *vals);

#define darray_erase(darray, i)				(_dar_ers(darray, i, 1))
#define darray_erase_n(darray, start, n)	(_dar_ers(darray, start, n))
uint8_t	_dar_ers(darray arr, const size_t i, const size_t n);
