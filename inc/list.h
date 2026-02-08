// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<list.h>>

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct _lst *		list;
typedef const struct _lst *	clist;

typedef struct _lst_node *	list_node;

struct _lst_node {
	uint8_t	*data;
};

#define list(type, count, free)	(_lst_new(sizeof(type), count, free))
list	_lst_new(const size_t size, const size_t count, void (*free)(void *));

#define list_delete(list)	(_lst_del(list))
void	_lst_del(list list);

#define list_push_front(list, value)	(_lst_psh_f(list, (const void *)&value))
uint8_t	_lst_psh_f(list list, const void *val);

#define list_push_back(list, value)	(_lst_psh_b(list, (const void *)&value))
uint8_t	_lst_psh_b(list list, const void *val);

#define list_pop_front(list)	(_lst_pop_f(list))
void	_lst_pop_f(list list);

#define list_pop_back(list)	(_lst_pop_b(list))
void	_lst_pop_b(list list);

#define list_first(list)	(_lst_nth(list, 0))
#define list_start(list)	(list_first(list))
#define list_last(list)		(_lst_nth(list, SIZE_MAX))
#define list_end(list)		(list_last(list))
#define list_nth(list, i)	(_lst_nth(list, i))
list_node	_lst_nth(clist list, const size_t i);

#define list_next(list, node)	(_lst_nxt(list, node))
list_node	_lst_nxt(clist list, const list_node node);

#define list_prev(list, node)	(_lst_prv(list, node))
list_node	_lst_prv(clist list, const list_node node);

#define list_insert_before(list, ref, value)	(_lst_ins_b(list, ref, (const void *)&value))
uint8_t	_lst_ins_b(list list, const list_node ref, const void *val);

#define list_insert_after(list, ref, value)	(_lst_ins_a(list, ref, (const void *)&value))
uint8_t	_lst_ins_a(list list, const list_node ref, const void *val);

#define list_move_before(list, ref, node)	(_lst_mve_b(list, ref, node))
void	_lst_mve_b(list list, const list_node ref, const list_node node);

#define list_move_after(list, ref, node)	(_lst_mve_a(list, ref, node))
void	_lst_mve_a(list list, const list_node ref, const list_node node);

#define list_erase(list, node)	(_lst_ers(list, node))
void	_lst_ers(list list, const list_node node);

#define list_size(list)			(_lst_sze(list, 0))
#define list_real_size(list)	(_lst_sze(list, 1))
size_t	_lst_sze(clist list, const uint8_t real);

#define list_capacity(list)	(_lst_cap(list))
size_t	_lst_cap(clist list);

#define list_resize(list, size)	(_lst_rsz(list, size))
uint8_t	_lst_rsz(list list, const size_t size);

#define list_shrink_to_fit(list)	(_lst_stf(list))
uint8_t	_lst_stf(list list);

#define list_clear(list)	(_lst_clr(list))
void	_lst_clr(list list);
