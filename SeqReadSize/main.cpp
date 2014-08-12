//
// Sequential read ...
//
// Nick Zaborovsky
// Aug-2014
//
// + Ulrich!

#include <cstdio>
#include "stopwatch.h"

#define NPAD_POWER	3
#define NPAD ((1<<NPAD_POWER) - 1)

static FILE *h_log;

struct chunk_t {
	chunk_t *p;
#if (NPAD!=0)
	long int payload[NPAD];
#endif
};


volatile unsigned no_optimize = 0;
#define NO_OPTIMIZE(x)	no_optimize += unsigned(x);


void workCycle(unsigned logSize) {
	chunk_t *const mem = new chunk_t[1 << logSize];

	// Creates circle list
	chunk_t *first = mem;
	chunk_t *cur = first;

	for (unsigned i = 0; i < (1 << logSize) - 1; ++i) {
		cur->p = cur + 1;
		cur = cur->p;
	}
	cur->p = first;

	register chunk_t* x = first->p;
	register int n, m;
	swatch t;
	t.reset();
	for (n = 0; n < 128; ++n) {
		//x = first->p;
		//while(x != first) {
		//	x = x->p;
		//}
		for (m = 0; m < (1 << logSize); ++m) {
			NO_OPTIMIZE((first + m)->p->p);
		}
	}
	long long elapsed = t.get();
	printf("WS: %d KB, items: %d, time/128items: %d picosec\n",
		sizeof(chunk_t) * (1 << (logSize - 10)), 1 << logSize, 1000 * elapsed / (1 << logSize));
	fprintf(h_log, "%d\t%d\t%d\n",
		sizeof(chunk_t) * (1 << (logSize - 10)), 1 << logSize, 1000 * elapsed / (1 << logSize));

	delete[] mem;
}


int main() {
	if (NULL == (h_log = fopen("log.txt", "a+"))) {
		printf("Cannot open the log file...\n");
		getchar();
		return 0;
	}
	
	printf("--- Chunk size: %u ---\n", sizeof(chunk_t));
	fprintf(h_log, "--- Chunk size: %u ---\n", sizeof(chunk_t));
	fprintf(h_log, "WS: KB,\t items num,\t time/128items, picosec\n");
	for (unsigned i = 11; i < 29 - NPAD_POWER; ++i) {
		workCycle(i);
	}
	fclose(h_log);
	getchar();
	return 1;
}