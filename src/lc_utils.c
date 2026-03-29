// ███████╗████████╗     ██████╗ ██╗   ██╗████████╗ ██████╗██╗  ██╗ █████╗ ██████╗
// ██╔════╝╚══██╔══╝     ██╔══██╗██║   ██║╚══██╔══╝██╔════╝██║  ██║██╔══██╗██╔══██╗
// █████╗     ██║        ██████╔╝██║   ██║   ██║   ██║     ███████║███████║██████╔╝
// ██╔══╝     ██║        ██╔═══╝ ██║   ██║   ██║   ██║     ██╔══██║██╔══██║██╔══██╗
// ██║        ██║███████╗██║     ╚██████╔╝   ██║   ╚██████╗██║  ██║██║  ██║██║  ██║
// ╚═╝        ╚═╝╚══════╝╚═╝      ╚═════╝    ╚═╝    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
//
// <<lc_utils.c>>

#include <alloca.h>
#include <string.h>

#include "defs.h"
#include "alloc.h"

#include "internal/utils.h"

static uint8_t	_simple_swap(void * const e1, void * const e2);
static void		*_simple_copy(const void * const p);

const lc_swapper	lc_simple_swap = _simple_swap;
const lc_copyer		lc_simple_copy = _simple_copy;

size_t	simple_swap_esize;

void	__free(void **blk) {
	lc_free(*blk);
}

static uint8_t	_simple_swap(void * const e1, void * const e2) {
	void	*buf;

	buf = alloca(simple_swap_esize);
	memcpy(buf, e1, simple_swap_esize);
	memcpy(e1, e2, simple_swap_esize);
	memcpy(e2, buf, simple_swap_esize);
	return 1;
}

static void	*_simple_copy(const void * const p) {
	return (void *)p;
}
