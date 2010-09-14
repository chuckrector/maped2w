#include "ops.h"

#include "maped.h"
//#include "allegro.h"
#include "pcx.h"
#include "vdriver.h"

#define MAX_TILES 65000

int InsertTiles(int here, int count) {
	if (here > numtiles) return 0;
	if (count < 1) return 0;
	if (numtiles > MAX_TILES) return 0;
	if (here < 0) here = numtiles;
	if (numtiles + count > MAX_TILES) {
		count = MAX_TILES - numtiles;
	}
	BITMAP** new_vsp = new BITMAP*[numtiles + count];
	int index = 0;
	// copy first half
	for (index = 0; index < here; ++index) {
		new_vsp[index] = vsp[index];
	}
	// copy last half
	for (; index < numtiles; ++index) {
		new_vsp[index + count] = vsp[index];
	}
	for (index = 0; index < count; ++index, ++here) {
		new_vsp[here] = create_bitmap(16,16);
		rectfill(new_vsp[here],0,0,16,16,0);
	}
	vsp = new_vsp;
	numtiles += count;
	return count;
}

int DeleteTiles(int here, int count) {
	if (here < 0 || here >= numtiles) return 0;
	if (count < 1) return 0;
	if (here + count > numtiles) {
		count = numtiles - here;
	}
	for (int index = 0; index < count; ++index) {
		destroy_bitmap(vsp[here + index]);
	}
	//memcpy(vsp+(z*256),vsp+((z+1)*256),((numtiles-z-1)*256));
	for (int index = here; index < numtiles - count; ++index) {
		//blit(vsp[index + count], vsp[index], 0, 0, 0, 0, 16, 16);
		vsp[index] = vsp[index + count];
	}
    numtiles-=count;
	return count;
}

void ScreenShot(char* fname) {
	PCX* pcx=new PCX();
	pcx->capture(get_offscreen());

	char sequenced_name[1024]={0};
	GetSequencedFilename(sequenced_name, "PCX");
	if (sequenced_name[0]==0) {
		strcpy(sequenced_name,"_000_.PCX");
	}
	pcx->save(sequenced_name);

	delete pcx;
}

void GetSequencedFilename(char* dest, char* extension) {
	char fname[1024];
	int index = 0;
	do {
		sprintf(fname, "%d.%s", index++, extension);
	} while (index<10000 && exists(fname));
	if (!exists(fname)) {
		strcpy(dest, fname);
	}
}