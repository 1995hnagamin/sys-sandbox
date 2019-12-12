#include "queue.h"
#include <assert.h>
#include <stdlib.h>

enum mm_type {
	mm_tnull = 0,
	mm_tstring,
	mm_tcell,
};

typedef struct mm_cell {
	struct mm_unit *car;
	struct mm_unit *cdr;
} mm_cell_t;

typedef struct mm_unit {
	enum mm_type type;
	int erc;	// exterior reference count
	int mark;
	union {
		char *str;
		mm_cell_t cell;
	};
} mm_unit_t;

enum {
	mm_pool_size = 100
};

typedef struct mm_pool {
	mm_unit_t units[mm_pool_size];
	int active;	// active units
	struct mm_pool *prev;
	struct mm_pool *next;
} mm_pool_t;

mm_pool_t *
mm_pool_create(void) {
	mm_pool_t *pool = (mm_pool_t *)malloc(sizeof(mm_pool_t));
	pool->active = 0;
	pool->next = pool->prev = NULL;
	mm_unit_t *const end = pool->units + mm_pool_size;
	for (mm_unit_t *u = pool->units; u != end; ++u) {
		u->type = mm_tnull;
		u->erc = 0;
	}
	return pool;
}

void
prepare_next_pool(mm_pool_t *curr) {
	if (curr->next) {
		return;
	}
	mm_pool_t *next = mm_pool_create();
	next->prev = curr;
	curr->next = next;
}

mm_unit_t *
mm_pool_alloc(mm_pool_t *pool) {
	if (pool->active >= mm_pool_size) {
		prepare_next_pool(pool);
		return mm_pool_alloc(pool->next);
	}
}

void
mm_collect(mm_pool_t *pool) {
	for (mm_pool_t *p = pool; p != NULL; p = p->next) {
		mm_unit_t *const end = p->units + mm_pool_size;
		for (mm_unit_t *u = p->units; u != end; ++u) {
			u->mark = 0;
		}
	}
	struct queue *que = queue_create(mm_pool_size);
	for (mm_pool_t *p = pool; p != NULL; p = p->next) {
		mm_unit_t *const end = p->units + mm_pool_size;
		for (mm_unit_t *u = p->units; u != end; ++u) {
			if (u->erc <= 0) {
				continue;
			}
			u->mark = 1;
			if (u->type == mm_tcell) {
				queue_push(que, u);
			}
		}
	}

	while (!queue_empty(que)) {
		mm_unit_t *u = (mm_unit_t *)queue_front(que);
		switch (u->type) {
			case mm_tnull:
				assert(0);
			case mm_tcell:
				if (u->cell.car) {
					u->cell.car->mark = 1;
					queue_push(que, u->cell.car);
				}
				if (u->cell.cdr) {
					u->cell.cdr->mark = 1;
					queue_push(que, u->cell.cdr);
				}
			default:
				; /* nop */
		}
	}
}
