#include <stdio.h>
#include <string.h>


#define NAO 32
#define NDIO 1

struct VO {
	short ao[NAO];
	unsigned dio[NDIO];
};

#define NSAM 65536
#define DOCOUNT_MASK 0x0f
#define DOVALUE		0xdead5550


int main(int argc, char* argv[])
{
	VO* data = new VO[NSAM];
	memset(data, 0, NSAM*sizeof(VO));

	for (int ii = 0; ii < NSAM; ++ii){
		data[ii].ao[0] = ii;
		data[ii].dio[0] = DOVALUE | (ii&DOCOUNT_MASK);
	}

	fwrite(data, sizeof(struct VO), NSAM, stdout);
}


