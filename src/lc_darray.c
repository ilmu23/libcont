// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<lc_darray.c>>

#include <alloca.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "alloc.h"
#include "darray.h"

#include "internal/utils.h"

struct _darray {
	lc_freer	free;
	uint8_t		*data;
	size_t		element_size;
	size_t		capacity;
	size_t		elements;
};

#define min(x, y)	(((x) < (y)) ? (x) : (y))

#define index_arr(arr, i, esize)	((void *)((uintptr_t)(arr) + ((i) * (esize))))
#define index_darr(arr, i)			(index_arr((arr)->data, i, (arr)->element_size))

static inline void	_set_element(darray arr, const size_t i, const void *val);

darray	_dar_new(const size_t size, const size_t count, const lc_freer free) {
	darray	out;

	out = (size) ? lc_malloc(sizeof(*out)) : NULL;
	if (out) {
		out->data = lc_malloc(size * count);
		if (!out->data) {
			lc_free(out);
			return NULL;
		}
		_dar_fre(out, free);
		out->element_size = size;
		out->capacity = count;
		out->elements = 0;
	}
	return out;
}

darray	_dar_cpy(cdarray arr, const size_t start, const size_t end, const lc_copyer cpy) {
	darray	out;

	out = lc_malloc(sizeof(*out));
	if (out) {
		out->capacity = (end - start < arr->capacity) ? end - start : arr->capacity;
		out->data = lc_malloc(arr->element_size * ((out->capacity) ? out->capacity : 1));
		if (!out->data) {
			lc_free(out);
			return NULL;
		}
		out->elements = (cpy) ? 0 : (arr->elements > out->capacity) ? out->capacity : arr->elements;
		out->element_size = arr->element_size;
		out->free = arr->free;
		if (cpy) while (out->elements < arr->elements) {
			_set_element(out, out->elements, cpy(index_darr(arr, start + out->elements)));
			out->elements++;
		} else
			memcpy(out->data, index_darr(arr, start), out->element_size * out->elements);
		if (out->capacity == 0)
			out->capacity = 1;
	}
	return out;
}

void	*_dar_to_arr(cdarray arr, const lc_copyer cpy, const uint8_t zero) {
	struct _darray	tmp;

	if (arr->elements == 0)
		return (zero) ? lc_calloc(1, arr->element_size) : NULL;
	tmp.data = lc_malloc((arr->elements + ((zero) ? 1 : 0)) * arr->element_size);
	if (tmp.data) {
		tmp.element_size = arr->element_size;
		if (!cpy) {
			tmp.elements = arr->elements;
			memcpy(tmp.data, arr->data, tmp.elements * tmp.element_size);
		} else for (tmp.elements = 0; tmp.elements < arr->elements; tmp.elements++)
			_set_element(&tmp, tmp.elements, cpy(index_darr(arr, tmp.elements)));
		if (zero)
			memset(index_darr(&tmp, tmp.elements), 0, tmp.element_size);
	}
	return tmp.data;
}

list	_dar_to_lst(cdarray arr, const lc_copyer cpy) {
	lc_copyer	_cpy;
	size_t		i;
	list		out;

	out = _lst_new(arr->element_size, arr->elements, arr->free);
	if (out) {
		_cpy = (cpy) ? cpy : lc_simple_copy;
		for (i = 0; i < arr->elements; i++) {
			if (!_lst_psh_b(out, _cpy(index_darr(arr, i)))) {
				list_delete(out);
				return NULL;
			}
		}
	}
	return out;
}

darray	_dar_frm_arr(const size_t size, const size_t count, const void *arr, const lc_freer free, const lc_copyer cpy) {
	darray	out;

	if (count == 0)
		return NULL;
	out = lc_malloc(sizeof(*out));
	if (out) {
		out->data = lc_malloc(count * size);
		if (!out->data) {
			lc_free(out);
			return NULL;
		}
		out->element_size = size;
		out->capacity = count;
		_dar_fre(out, free);
		out->elements = 0;
		if (cpy) while (out->elements < count) {
			_set_element(out, out->elements, cpy(index_arr(arr, out->elements, size)));
			out->elements++;
		} else {
			memcpy(out->data, arr, count * size);
			out->elements = count;
		}
	}
	return out;
}

void	_dar_del(darray arr) {
	if (arr) {
		if (arr->elements)
			_dar_clr(arr);
		lc_free(arr->data);
		lc_free(arr);
	}
}

uint8_t	_dar_psh(darray arr, const void *val) {
	if (arr->elements == arr->capacity && !_dar_rsz(arr, arr->capacity * 2))
		return 0;
	_set_element(arr, arr->elements++, val);
	return 1;
}

void	_dar_pop(darray arr) {
	if (arr->elements) {
		arr->elements--;
		if (arr->free)
			arr->free(index_darr(arr, arr->elements));
	}
}

void	*_dar_get(cdarray arr, const size_t i) {
	return (i < arr->elements) ? index_darr(arr, i) : (i == (size_t)-1 && arr->elements) ? index_darr(arr, arr->elements - 1) : DARRAY_OUT_OF_BOUNDS;
}

uint8_t	_dar_set(darray arr, const size_t i, const void *val, const uint8_t free) {
	if (i >= arr->elements)
		return 0;
	if (free && arr->free)
		arr->free(index_darr(arr, i));
	_set_element(arr, i, val);
	return 1;
}

uint8_t	_dar_swp(darray arr, const size_t i1, const size_t i2, const lc_swapper swap) {
	if (i1 >= arr->elements || i2 >= arr->elements)
		return 0;
	simple_swap_esize = arr->element_size;
	return (i1 != i2) ? swap(index_darr(arr, i1), index_darr(arr, i2)) : 1;
}

size_t	_dar_sze(cdarray arr) {
	return arr->elements;
}

size_t	_dar_cap(cdarray arr) {
	return arr->capacity;
}

uint8_t	_dar_rsz(darray arr, const size_t size) {
	if (size < arr->elements && arr->free) do
		arr->free(index_darr(arr, --arr->elements));
	while (size < arr->elements);
	arr->capacity = size;
	arr->data = lc_realloc(arr->data, arr->capacity * arr->element_size);
	return (arr->data) ? 1 : 0;
}

uint8_t	_dar_stf(darray arr) {
	return _dar_rsz(arr, arr->elements);
}

void	_dar_fea(darray arr, const lc_foreach_fn fn, void *fn_arg) {
	size_t	i;

	for (i = 0; i < arr->elements; i++)
		fn(index_darr(arr, i), fn_arg);
}

void	_dar_fre(darray arr, const lc_freer free) {
	arr->free = (free != lc_free) ? free : (void (*)(void *))__free;
}

void	_dar_clr(darray arr) {
	if (arr->free && arr->elements) do
		arr->free(index_darr(arr, --arr->elements));
	while (arr->elements);
	arr->elements = 0;
}

uint8_t	_dar_ins(darray arr, const size_t i, const size_t n, const void *vals) {
	size_t	_i;

	if (n == 0)
		return 1;
	if (arr->elements + n > arr->capacity) { do
		arr->capacity *= 2;
		while (arr->elements + n > arr->capacity);
		if (!_dar_rsz(arr, arr->capacity))
			return 0;
	}
	_i = min(i, arr->elements);
	if (_i != arr->elements)
		memmove(index_darr(arr, _i + n), index_darr(arr, _i), (arr->elements - _i) * arr->element_size);
	memcpy(index_darr(arr, _i), vals, n * arr->element_size);
	arr->elements += n;
	return 1;
}

uint8_t	_dar_ers(darray arr, const size_t i, const size_t n) {
	size_t	_n;

	if (n == 0)
		return 1;
	if (i >= arr->elements)
		return 0;
	if (i != arr->elements - 1) {
		if (arr->free) for (_n = 0; _n < n && i + _n < arr->elements; _n++)
			arr->free(index_darr(arr, i));
		else
			_n = min(n, arr->elements - i);
		memmove(index_darr(arr, i), index_darr(arr, i + _n), (arr->elements - i - _n) * arr->element_size);
		arr->elements -= _n;
	} else
		_dar_pop(arr);
	return 1;
}

static inline void	_set_element(darray arr, const size_t i, const void *val) {
	memmove(index_darr(arr, i), val, arr->element_size);
}
