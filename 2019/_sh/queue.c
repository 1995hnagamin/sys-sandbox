#include "queue.h"
#include <assert.h>
#include <stdlib.h>

typedef struct stack {
	void **begin;
	size_t cap;
	void **end;
} stack_t;

stack_t *
stack_create(size_t cap) {
	void **buf = malloc(sizeof(void *) * cap);
	stack_t *stk = (stack_t *)malloc(sizeof(stack_t));
	stk->begin = buf;
	stk->cap = cap;
	stk->end = buf;
	return stk;
}

void
stack_free(stack_t *stk) {
	free(stk->begin);
	free(stk);
}

size_t
stack_size(stack_t *stk) {
	return stk->end - stk->begin;
}

int
stack_empty(stack_t *stk) {
	return stk->begin == stk->end;
}

static int
stack_full(stack_t *stk) {
	return stk->begin + stk->cap == stk->end;
}

static void
stack_extend(stack_t *stk, size_t newcap) {
	if (stk->cap >= newcap) {
		return;
	}
	void **const buf = malloc(sizeof(void *) * newcap);
	assert(buf);
	size_t const len = stack_size(stk);
	for (size_t i = 0; i < len; ++i) {
		buf[i] = stk->begin[i];
	}
	free(stk->begin);

	stk->begin = buf;
	stk->cap = newcap;
	stk->end = buf + len;
}

void *
stack_top(stack_t *stk) {
	assert(!stack_empty(stk));
	return *(stk->end - 1);
}

void
stack_pop(stack_t *stk) {
	assert(!stack_empty(stk));
	stk->end--;
}

void
stack_push(stack_t *stk, void *ptr) {
	if (stack_full(stk)) {
		stack_extend(stk, stk->cap * 2 + 1);
	}
	*(stk->end) = ptr;
	stk->end++;
}

typedef struct queue {
	stack_t *fst;
	stack_t *snd;
} queue_t;

queue_t *
queue_create(size_t cap) {
	stack_t *fst = stack_create(cap), *snd = stack_create(cap);
	queue_t *que = (queue_t *)malloc(sizeof(queue_t));
	que->fst = fst;
	que->snd = snd;
	return que;
}

void
queue_free(queue_t *que) {
	stack_free(que->fst);
	stack_free(que->snd);
	free(que);
}

size_t
queue_size(queue_t *que) {
	return stack_size(que->fst) + stack_size(que->snd);
}

int
queue_empty(queue_t *que) {
	return queue_size(que) == 0;
}

static void
queue_flush(queue_t *que) {
	while (!stack_empty(que->fst)) {
		void *ptr = stack_top(que->fst);
		stack_pop(que->fst);
		stack_push(que->snd, ptr);
	}
}

void *
queue_front(queue_t *que) {
	if (stack_empty(que->snd)) {
		queue_flush(que);
	}
	return stack_top(que->snd);
}

void
queue_pop(queue_t *que) {
	if (stack_empty(que->snd)) {
		queue_flush(que);
	}
	stack_pop(que->snd);
}

void
queue_push(queue_t *que, void *val) {
	stack_push(que->fst, val);
}
