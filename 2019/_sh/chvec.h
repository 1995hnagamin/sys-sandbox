#ifndef CHVEC_H
#define CHVEC_H

#include <stddef.h>

struct chvec;

char *chvec_ptr(struct chvec *cv);
size_t chvec_size(struct chvec *cv);
struct chvec *chvec_create(size_t cap);
struct chvec *chvec_clone(struct chvec *cv);
void chvec_push_back(struct chvec *cv, char c);
void chvec_free(struct chvec *cv);


#endif /* !CHVEC_H */
