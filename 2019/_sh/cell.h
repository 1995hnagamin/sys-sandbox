#ifndef CELL_H
#define CELL_H

enum tr_type {
	tr_tinvalid,
	tr_tcell,
	tr_tstr,
	tr_tint,
};

struct tr_object;

void tr_free(struct tr_object *);

struct tr_object *tr_create_cell(struct tr_object *car, struct tr_object *cdr);

struct chvec;
struct tr_object *tr_create_str(struct chvec *);

struct tr_object *tr_create_int(int);

struct tr_object *tr_clone(struct tr_object *);

enum tr_type tr_get_type(struct tr_object *);

int tr_get_int(struct tr_object *);

struct chvec *tr_get_chvec(struct tr_object *);

struct tr_object *tr_get_car(struct tr_object *);

struct tr_object *tr_get_cdr(struct tr_object *);

#endif /* !CELL_H */
