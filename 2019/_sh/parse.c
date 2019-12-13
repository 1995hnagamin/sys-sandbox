#include "cell.h"
#include "chvec.h"
#include <ctype.h>

struct chvec *
parse_string(char **str) {
	char *p = *str;
	struct chvec *cv = chvec_create(10);
	while (isalnum(*p) || *p == '-') {
		chvec_push_back(cv, *p);
		++p;
	}
	chvec_push_back(cv, '\0');
	*str = p;
	return cv;
}

struct tr_cell *
parse_command(char **str) {

}
