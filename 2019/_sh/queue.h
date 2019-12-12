#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

struct queue;

struct queue *queue_create(size_t cap);

void queue_free(struct queue *que);

size_t queue_size(struct queue *que);

int queue_empty(struct queue *que);

void queue_push(struct queue *que, void *val);

void *queue_front(struct queue *que);

void queue_pop(struct queue *que);

#endif /* !QUEUE_H */
