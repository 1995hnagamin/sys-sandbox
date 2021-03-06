#include "chvec.h"
#include "cell.h"
#include <assert.h>
#include <stdio.h>
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
			tr_free(p->car);
			tr_free(p->cdr);
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
	p->car = car;
	p->cdr = cdr;
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
			tr_object_t *car = tr_clone(p->car);
			tr_object_t *cdr = tr_clone(p->cdr);
			return tr_create_cell(car, cdr);
		}
		case tr_tstr:
		{
			struct chvec *cv = chvec_clone(p->cv);
			return tr_create_str(cv);
		}
	}
}

size_t
tr_list_length(tr_object_t *list) {
	size_t sz = 0;
	while (list) {
		++sz;
		list = list->cdr;
	}
	return sz;
}

char **
tr_list_to_sarr(tr_object_t *list, size_t len) {
	char **sarr = (char **)malloc(sizeof(char *) * (len + 1));
	sarr[len] = NULL;
	tr_object_t *cell = list;
	for (size_t i = 0; i < len; ++i, cell = cell->cdr) {
		sarr[i] = chvec_ptr(cell->car->cv);
	}
	return sarr;
}

static void
tr_dump_helper(FILE *stream, tr_object_t *p) {
	if (!p) {
		fprintf(stream, "nil");
		return;
	}
	switch (p->type) {
		case tr_tinvalid:
			assert(0);
		case tr_tint:
			fprintf(stream, "%d", p->i);
			break;
		case tr_tstr:
			fprintf(stream, "\"%s\"", chvec_ptr(p->cv));
			break;
		case tr_tcell:
			fprintf(stream, "(");
			tr_dump_helper(stream, p->car);
			fprintf(stream, " . ");
			tr_dump_helper(stream, p->cdr);
			fprintf(stream, ")");
			break;
	}
}

void
tr_dump(tr_object_t *p) {
	tr_dump_helper(stderr, p);
	fprintf(stderr, "\n");
}
