#ifndef PCX_INC
#define PCX_INC

#include "allegro.h"
#include <stdio.h>

class PCX
{
public:
	struct PCXHeader {
		char manufacturer;                     // pcx header
		char version;
		char encoding;
		char bits_per_pixel;
		short int xmin,ymin;
		short int xmax,ymax;
		short int hres;
		short int vres;
		char palette[48];
		char reserved;
		char color_planes;
		short int bytes_per_line;
		short int palette_type;
		char filler[58];
	};
	PCXHeader header;
	unsigned short width;
	unsigned short height;
	unsigned char* data;
	unsigned char palette[3 * 256];

	~PCX();
	PCX();
	void decodeLine(int offset, FILE* pcxf);
	void encodeLine(int offset, FILE* pcxf);
	void capture(BITMAP* bitmap);
	void save(const char* filename);
	void load(const char* filename);
};

#endif // PCX_INC