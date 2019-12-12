#include "chvec.h"
#include <stdio.h>

struct chvec *
ussh_read_line(void) {
	struct chvec *cv = chvec_create(10);
	for (;;) {
		char c = getchar();
		if (c == EOF || c == '\n') {
			chvec_push_back(cv, '\0');
			break;
		}
		chvec_push_back(cv, c);
	}
	return cv;
}

int
main(int argc, char* argv[]) {
	struct chvec *cv = ussh_read_line();
	printf("%s\n", chvec_ptr(cv));
	chvec_free(cv);
	return 0;
}
