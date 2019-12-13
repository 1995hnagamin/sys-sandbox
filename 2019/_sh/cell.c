#include "chvec.h"
#include "cell.h"
#include <assert.h>
#include <stdlib.h>

typedef struct tr_object tr_object_t;

void
tr_free(tr_object_t *p) {
	if (!p) {
		return;
	}
	switch (p->type) {
		case tr_tinvalid:
			assert(0);
		case tr_tint:
			break;
		case tr_tcell:
			tr_free(p->cell.car);
			tr_free(p->cell.cdr);
			break;
		case tr_tstr:
			chvec_free(p->cv);
			break;
	}
	free(p);
}

tr_object_t *
tr_create_cell(tr_object_t *car, tr_object_t *cdr) {
	tr_object_t *p = (tr_object_t *)malloc(sizeof(tr_object_t));
	p->type = tr_tcell;
	p->cell.car = car;
	p->cell.cdr = cdr;
	return p;
}

tr_object_t *
tr_create_str(struct chvec *cv) {
	tr_object_t *p = (tr_object_t *)malloc(sizeof(tr_object_t));
	p->type = tr_tstr;
	p->cv = cv;
	return p;
}

tr_object_t *
tr_create_int(int i) {
	tr_object_t *p = (tr_object_t *)malloc(sizeof(tr_object_t));
	p->type = tr_tint;
	p->i = i;
	return p;
}

tr_object_t *
tr_clone(tr_object_t *p) {
	if (!p) {
		return NULL;
	}
	switch (p->type) {
		case tr_tinvalid:
			assert(0);
		case tr_tint:
			return tr_create_int(p->i);
		case tr_tcell:
		{
			tr_object_t *car = tr_clone(p->cell.car);
			tr_object_t *cdr = tr_clone(p->cell.cdr);
			return tr_create_cell(car, cdr);
		}
		case tr_tstr:
		{
			struct chvec *cv = chvec_clone(p->cv);
			return tr_create_str(cv);
		}
	}
}
