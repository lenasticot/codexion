
#ifndef CODEXION_H
# define FT_PRINTF_H

# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <stdio.h>
# include <stdint.h>
# include <stdio.h>
# include <stdarg.h>
# include <unistd.h>
# include <stdlib.h>
# include <limits.h>

// What do i need my coder to be
// need to learn about the heap


// need to work also on time and managing milliseconds

typedef struct coder {
	// Just the number of the coder but to see if this is interesting or not
	int nb;
	// maybe need to do a hierachy between them especially to know which one has the priority
	int rank;
	// for status could be equivalent to compile debug and refactor
	// and to be equal to 0 1 or 2
	int status;
	// or they have individual debug, refac and compile
	// 0 when off
	// 1 when on
	// always need to check if the other is on 
	// need to see what could be the most efficient
	int debug;
	int refac;
	int comp;
	int burnout_time;
	int cooldown;
	// to know how many time the coder has compiled already
	// to be decreased until 0
	int compiled;
	// to check how many dongle they have in their hand
	int dongle;

}

struct dongle {
	int rank;
}



#endif