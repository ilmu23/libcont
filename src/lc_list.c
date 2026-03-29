// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<list.c>>

#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "list.h"
#include "alloc.h"
#include "darray.h"

#include "internal/utils.h"

#define _INDEX_NONE	SIZE_MAX

#define _ALLOCA_STACK_FRACTION	64

#define index(arr, i, esize)	((void *)((uintptr_t)(arr) + ((i) * (esize))))

#define get_node(_node)	((__lst_node *)((uintptr_t)_node - offsetof(__lst_node, node)))

#define get_node_i(list, n)	((__lst_node *)darray_get(list->data, n))

#define remove_deleted(list, e)	if (e) do darray_pop(list->data); while(get_node_i(list, --list->highest_index)->delete)

typedef struct {
	struct _lst_node	node;
	lc_freer			free;
	size_t				index;
	size_t				next;
	size_t				prev;
	uint8_t				delete;
}	__lst_node;

struct _lst {
	lc_freer	free;
	size_t		highest_index;
	size_t		element_size;
	size_t		elements;
	size_t		first;
	size_t		last;
	darray		data;
};

static struct {
	size_t	max;
	uint8_t	set;
}	_alloca_size;

static inline void	_detach_node(list list, const __lst_node *node);

static inline void	_set_alloca_max(void);

static void	_free_node(__lst_node *node);

list	_lst_new(const size_t size, const size_t count, lc_freer free) {
	list	out;

	if (!_alloca_size.set)
		_set_alloca_max();
	out = (size) ? lc_malloc(sizeof(*out)) : NULL;
	if (out) {
		out->data = darray(__lst_node, count, (lc_freer)_free_node);
		if (!out->data) {
			lc_free(out);
			return NULL;
		}
		_lst_fre(out, free);
		out->element_size = size;
		out->highest_index = 0;
		out->elements = 0;
		out->first = 0;
		out->last = 0;
	}
	return out;
}

void	*_lst_to_arr(clist list, lc_copyer cpy, const uint8_t zero) {
	__lst_node	*node;
	size_t		i;
	void		*out;

	if (list->elements == 0)
		return (zero) ? lc_calloc(1, list->element_size) : NULL;
	out = lc_malloc((list->elements + ((zero) ? 1 : 0)) * list->element_size);
	if (out) {
		if (!cpy)
			cpy = lc_simple_copy;
		i = 0;
		node = darray_get(list->data, list->first);
		do {
			memcpy(index(out, i++, list->element_size), cpy(node->node.data), list->element_size);
			node = (node->next != _INDEX_NONE) ? darray_get(list->data, node->next) : NULL;
		} while (node);
		if (zero)
			memset(index(out, i, list->element_size), 0, list->element_size);
	}
	return out;
}

darray	_lst_to_dar(clist list, lc_copyer cpy) {
	__lst_node	*node;
	darray		out;

	out = _dar_new(list->element_size, list->elements, list->free);
	if (out && list->elements) {
		if (!cpy)
			cpy = lc_simple_copy;
		node = darray_get(list->data, list->first);
		do {
			if (!_dar_psh(out, cpy(node->node.data))) {
				darray_delete(out);
				return NULL;
			}
			node = (node->next != _INDEX_NONE) ? darray_get(list->data, node->next) : NULL;
		} while (node);
	}
	return out;
}

list	_lst_frm_arr(const size_t size, const size_t count, const void *arr, lc_freer free, lc_copyer cpy) {
	list	out;

	if (count == 0)
		return NULL;
	if (!_alloca_size.set)
		_set_alloca_max();
	out = lc_malloc(sizeof(*out));
	if (out) {
		out->data = darray(__lst_node, count, (lc_freer)_free_node);
		if (!out->data)
			goto __lst_frm_arr_alloc_err;
		out->element_size = size;
		out->highest_index = 0;
		_lst_fre(out, free);
		out->elements = 0;
		out->first = 0;
		out->last = 0;
		if (!cpy)
			cpy = lc_simple_copy;
		while (out->elements < count)
			if (!_lst_psh_b(out, cpy(index(arr, out->elements, size))))
				goto __lst_frm_arr_psh_err;
	}
	return out;
__lst_frm_arr_psh_err:
	darray_delete(out->data);
__lst_frm_arr_alloc_err:
	lc_free(out);
	return NULL;
}

void	_lst_del(list list) {
	if (list) {
		_lst_clr(list);
		darray_delete(list->data);
		lc_free(list);
	}
}

uint8_t	_lst_psh_f(list list, const void *val) {
	__lst_node	*first;
	__lst_node	new;
	uint8_t		rv;

	new = (__lst_node){
		.free = list->free,
		.delete = 0,
		.next = _INDEX_NONE,
		.prev = _INDEX_NONE,
		.node.data = lc_malloc(list->element_size)
	};
	if (!new.node.data)
		return 0;
	first = darray_get(list->data, list->first);
	memcpy(new.node.data, val, list->element_size);
	new.next = (first != DARRAY_OUT_OF_BOUNDS) ? first->index : _INDEX_NONE;
	new.index = darray_size(list->data);
	rv = darray_push(list->data, new);
	if (rv) {
		list->highest_index = new.index;
		if (new.next != _INDEX_NONE) {
			first = darray_get(list->data, new.next);
			first->prev = new.index;
		}
		list->first = new.index;
		list->elements++;
	} else
		lc_free(new.node.data);
	return rv;
}

uint8_t	_lst_psh_b(list list, const void *val) {
	__lst_node	*last;
	__lst_node	new;
	uint8_t		rv;

	new = (__lst_node){
		.free = list->free,
		.delete = 0,
		.next = _INDEX_NONE,
		.prev = _INDEX_NONE,
		.node.data = lc_malloc(list->element_size)
	};
	if (!new.node.data)
		return 0;
	last = darray_get(list->data, list->last);
	memcpy(new.node.data, val, list->element_size);
	new.prev = (last != DARRAY_OUT_OF_BOUNDS) ? last->index : _INDEX_NONE;
	new.index = darray_size(list->data);
	rv = darray_push(list->data, new);
	if (rv) {
		list->highest_index = new.index;
		if (new.prev != _INDEX_NONE) {
			last = darray_get(list->data, new.prev);
			last->next = new.index;
		}
		list->last = new.index;
		list->elements++;
	} else
		lc_free(new.node.data);
	return rv;
}

void	_lst_pop_f(list list) {
	__lst_node	*first;

	if (!list->elements)
		return ;
	first = darray_get(list->data, list->first);
	if (!--list->elements) {
		darray_pop(list->data);
		list->highest_index = 0;
		return ;
	}
	list->first = first->next;
	get_node_i(list, list->first)->prev = _INDEX_NONE;
	first->delete = 1;
	remove_deleted(list, first->index == list->highest_index);
}

void	_lst_pop_b(list list) {
	__lst_node	*last;

	if (!list->elements)
		return ;
	last = darray_get(list->data, list->last);
	if (!--list->elements) {
		darray_pop(list->data);
		list->highest_index = 0;
		return ;
	}
	list->last = last->prev;
	get_node_i(list, list->last)->next = _INDEX_NONE;
	last->delete = 1;
	remove_deleted(list, last->index == list->highest_index);
}

list_node	_lst_nth(clist list, const size_t i) {
	__lst_node	*out;
	size_t		_i;

	if (!list->elements)
		return NULL;
	if (i == 0)
		return &get_node_i(list, list->first)->node;
	if (i >= list->elements - 1)
		return &get_node_i(list, list->last)->node;
	_i = i;
	if (_i < list->elements / 2) {
		out = darray_get(list->data, list->first);
		while (_i--)
			out = darray_get(list->data, out->next);
	} else {
		out = darray_get(list->data, list->last);
		while (++_i < list->elements)
			out = darray_get(list->data, out->prev);
	}
	return &out->node;
}

list_node	_lst_nxt(clist list, const list_node node) {
	if (get_node(node)->next == _INDEX_NONE)
		return NULL;
	return &get_node_i(list, get_node(node)->next)->node;
}

list_node	_lst_prv(clist list, const list_node node) {
	if (get_node(node)->prev == _INDEX_NONE)
		return NULL;
	return &get_node_i(list, get_node(node)->prev)->node;
}

uint8_t	_lst_ins_b(list list, const list_node ref, const void *val) {
	__lst_node	*next;
	__lst_node	new;
	uint8_t		rv;

	new = (__lst_node){
		.free = list->free,
		.delete = 0,
		.node.data = lc_malloc(list->element_size)
	};
	if (!new.node.data)
		return 0;
	next = get_node(ref);
	memcpy(new.node.data, val, list->element_size);
	new.next = next->index;
	new.prev = next->prev;
	new.index = darray_size(list->data);
	rv = darray_push(list->data, new);
	if (rv) {
		list->highest_index = new.index;
		get_node_i(list, next->index)->prev = new.index;
		if (new.prev != _INDEX_NONE)
			get_node_i(list, new.prev)->next = new.index;
		else
			list->first = new.index;
		list->elements++;
	} else
		lc_free(new.node.data);
	return rv;
}

uint8_t	_lst_ins_a(list list, const list_node ref, const void *val) {
	__lst_node	*prev;
	__lst_node	new;
	uint8_t		rv;

	new = (__lst_node){
		.free = list->free,
		.delete = 0,
		.node.data = lc_malloc(list->element_size)
	};
	if (!new.node.data)
		return 0;
	prev = get_node(ref);
	memcpy(new.node.data, val, list->element_size);
	new.prev = prev->index;
	new.next = prev->next;
	new.index = darray_size(list->data);
	rv = darray_push(list->data, new);
	if (rv) {
		list->highest_index = new.index;
		get_node_i(list, prev->index)->next = new.index;
		if (new.next != _INDEX_NONE)
			get_node_i(list, new.next)->prev = new.index;
		else
			list->last = new.index;
		list->elements++;
	} else
		lc_free(new.node.data);
	return rv;
}

void	_lst_mve_b(list list, const list_node ref, const list_node node) {
	__lst_node	*_node;
	__lst_node	*_ref;
	__lst_node	*prev;

	if (ref == node)
		return ;
	_ref = get_node(ref);
	_node = get_node(node);
	if (_node->next == _ref->index)
		return ;
	_detach_node(list, _node);
	prev = (_ref->prev != _INDEX_NONE) ? darray_get(list->data, _ref->prev) : NULL;
	if (prev)
		prev->next = _node->index;
	else
		list->first = _node->index;
	_node->next = _ref->index;
	_node->prev = _ref->prev;
	_ref->prev = _node->index;
}

void	_lst_mve_a(list list, const list_node ref, const list_node node) {
	__lst_node	*_node;
	__lst_node	*_ref;
	__lst_node	*next;

	if (ref == node)
		return ;
	_ref = get_node(ref);
	_node = get_node(node);
	if (_node->prev == _ref->index)
		return ;
	_detach_node(list, _node);
	next = (_ref->next != _INDEX_NONE) ? darray_get(list->data, _ref->next) : NULL;
	if (next)
		next->prev = _node->index;
	else
		list->last =_node->index;
	_node->next = _ref->next;
	_node->prev = _ref->index;
	_ref->next = _node->index;
}

void	_lst_ers(list list, const list_node node) {
	__lst_node	*_node;

	_node = get_node(node);
	if (_node->next == _INDEX_NONE) {
		_lst_pop_b(list);
		return ;
	}
	if (_node->prev == _INDEX_NONE) {
		_lst_pop_f(list);
		return ;
	}
	get_node_i(list, _node->next)->prev = _node->prev;
	get_node_i(list, _node->prev)->next = _node->next;
	_node->delete = 1;
	list->elements--;
	remove_deleted(list, _node->index == list->highest_index);
}

size_t	_lst_sze(clist list, const uint8_t real) {
	return (real) ? darray_size(list->data) : list->elements;
}

size_t	_lst_cap(clist list) {
	return darray_capacity(list->data);
}

uint8_t	_lst_rsz(list list, const size_t size) {
	__lst_node	*tmp;
	__lst_node	*node;
	size_t		vec_size;
	size_t		tmp_size;
	size_t		i;

	vec_size = darray_size(list->data);
	if (size < vec_size) {
		tmp_size = vec_size * sizeof(*tmp);
		tmp = (tmp_size < _alloca_size.max) ? alloca(tmp_size) : lc_malloc(tmp_size);
		if (!tmp)
			return 0;
		for (i = 0, node = darray_get(list->data, list->first); i < vec_size; node = darray_get(list->data, node->next))
			tmp[i++] = *node;
		for (i = 0; i < vec_size; i++) {
			tmp[i].next = (i + 1 < size) ? i + 1 : _INDEX_NONE;
			tmp[i].prev = (i != 0) ? i - 1 : _INDEX_NONE;
			tmp[i].index = i;
			darray_set(list->data, i, tmp[i]);
		}
		list->highest_index = size - 1;
		list->elements = size;
		list->last = size - 1;
		list->first = 0;
	}
	return darray_resize(list->data, size);
}

uint8_t	_lst_stf(list list) {
	return _lst_rsz(list, list->elements);
}

void	_lst_fea(list list, void (*fn)(void *)) {
	__lst_node	*node;

	if (list->elements == 0)
		return ;
	node = darray_get(list->data, list->first);
	do {
		fn(node->node.data);
		node = (node->next != _INDEX_NONE) ? darray_get(list->data, node->next) : NULL;
	} while (node);
}

void	_lst_fre(list list, lc_freer free) {
	list->free = (free != lc_free) ? free : (void (*)(void *))__free;
}

void	_lst_clr(list list) {
	darray_clear(list->data);
	list->highest_index = 0;
	list->elements = 0;
	list->first = 0;
	list->last = 0;
}

static inline void	_detach_node(list list, const __lst_node *node) {
	__lst_node	*prev;
	__lst_node	*next;

	prev = (node->prev != _INDEX_NONE) ? darray_get(list->data, node->prev) : NULL;
	next = (node->next != _INDEX_NONE) ? darray_get(list->data, node->next) : NULL;
	if (prev)
		prev->next = node->next;
	else
		list->first = node->next;
	if (next)
		next->prev = node->prev;
	else
		list->last = node->prev;
}

static inline void	_set_alloca_max(void) {
	struct rlimit	limit;

	getrlimit(RLIMIT_STACK, &limit);
	_alloca_size.max =limit.rlim_cur / _ALLOCA_STACK_FRACTION;
	_alloca_size.set = 1;
}

static void	_free_node(__lst_node *node) {
	if (node->free)
		node->free(node->node.data);
	lc_free(node->node.data);
}
