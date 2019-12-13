#include "chvec.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct chvec {
	char *begin;
	size_t cap;
	char *end;
};

char *
chvec_ptr(struct chvec *cv) {
	return cv->begin;
}

size_t
chvec_size(struct chvec *cv) {
	return cv->end - cv->begin;
}

static int
chvec_full(struct chvec *cv) {
	return cv->begin + cv->cap == cv->end;
}

struct chvec *
chvec_create(size_t cap) {
	struct chvec *cv = (struct chvec *)malloc(sizeof(struct chvec *));
	if (!cv) {
		return NULL;
	}
	char *const buf = (char *)malloc(cap);
	assert(buf);
	cv->begin = buf;
	cv->cap = cap;
	cv->end = buf;
	return cv;
}

struct chvec *
chvec_clone(struct chvec *cv) {
	struct chvec *clone = chvec_create(cv->cap);
	size_t sz = chvec_size(cv);
	strncpy(clone->begin, cv->begin, sz);
	cv->end += sz;
	return clone;
}

void
chvec_free(struct chvec *cv) {
	free(cv->begin);
	free(cv);
}

static void
chvec_extend(struct chvec *cv, size_t newcap) {
	if (cv->cap >= newcap) {
		return;
	}
	char *const buf = (char *)malloc(newcap);
	assert(buf);
	size_t const sz = chvec_size(cv);
	strncpy(buf, cv->begin, sz);
	free(cv->begin);

	cv->begin = buf;
	cv->cap = newcap;
	cv->end = buf + sz;
}

void
chvec_push_back(struct chvec *cv, char c) {
	if (chvec_full(cv)) {
		chvec_extend(cv, cv->cap * 2 + 1);
	}
	*(cv->end) = c;
	cv->end++;
}
