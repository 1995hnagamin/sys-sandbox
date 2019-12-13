#ifndef CELL_H
#define CELL_H

#include <stddef.h>

enum tr_type {
	tr_tinvalid,
	tr_tcell,
	tr_tstr,
	tr_tint,
};

struct tr_cell {
	struct tr_object *car;
	struct tr_object *cdr;
};

struct tr_object {
	enum tr_type type;
	union {
		struct tr_cell cell;
		struct chvec *cv;
		int i;
	};
};


void tr_free(struct tr_object *);

void tr_dump(struct tr_object *);

struct tr_object *tr_create_cell(struct tr_object *car, struct tr_object *cdr);

struct chvec;
struct tr_object *tr_create_str(struct chvec *);

struct tr_object *tr_create_int(int);

struct tr_object *tr_clone(struct tr_object *);

/* list operation */
size_t tr_list_length(struct tr_object *);

#endif /* !CELL_H */
