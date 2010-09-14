
#include "maped.h"
#include "a_memory.h"
//#include "config.h"
#include "gui.h"
#include "keyboard.h"
#include "mouse.h"
#include "render.h"
#include "timer.h"
#include "vdriver.h"
#include "smalfont.h"
#include "tilesel.h"
#include "guicomp.h"
#include "config.h"
#include "log.h"
#include "minimap.h"
#include "ssaver.h"
#include "error.h"
#include "browse.h"
//
//#include <cstdio>

//#include <cstdlib>
#include <cstring>
#include <stack>
#include <direct.h>
//#include <cstdarg>

//#include "cdrom.h"
#include "compress.h"

#include "controls.h"

const char* ME2_VERSION = "2.4.21 (windows)";

// Includes for a_memory.c

//void *valloc(int amount, char *desc, int owner);
//void *qvalloc(int amount);
//int  vfree(void *pointer);
//void qvfree(void *pointer);
//void FreeByOwner(int owner);
//void MemReport(void);
//void CheckCorruption(void);
//
//void ReadCompressedLayer1(unsigned char *dest, int len, FILE *f);
//void WriteCompressedLayer1(unsigned char *p, int len);
//
//void ShellMAP();
//void CompileAll();
//void ShellVERGE();
//void ShellToDOS();
//void ShellEditMAP();
//void ShellEditSystem();
//void CompileMAP();
//
//void ProcessEntity(int xw, int yw);
//int EntityThere(int xw, int yw);
//void DeleteEntity(int xw, int yw);

// ================================= Data ====================================

//int pageflipping=0;
int windowed=0;

zoneinfo zones[256];                 // zone stuff.
layer_r layer[6];                    // Array of layer data
vspanim_r vspanim[100];              // tile animation data
unsigned short vadelay[100];         // Tile animation delay ctr
oldzone tzone;                       // zone for oldmap-to-v2 converting
entity_r entity[256];                // Entity records.
oldent_r oldent;                     // entity for oldmap-to-v2 converting
unsigned char entities=0;                     // number of allocated entities.
chrlist_r chrlist[100];              // Max. 100 CHRs per map.
movescript ms[100];                  // move scripts arrays
unsigned char nmchr=0;                        // number of active/loaded CHRs.
unsigned char nms=0;                          // number of movement scripts

char mapname[60];                    // MAP filename
char vspname[60];                    // VSP filemap
char rstring[20];                    // render-order string
char musname[60];                    // MAP default music
char numlayers;                      // number of layers in map
short xstart, ystart;                // MAP start locations
char *obstruct, *zone;               // obstruction and zone buffers
int numzones=0;                      // Number of active zones.
unsigned short *layers[6];           // Raw layer data
char wrap, soundokay=0;

// -- vsp related data --

unsigned short numtiles;             // number of allocated tiles in VSP
BITMAP** vsp;                  // VSP ptr
char vsp_locked=0;
char vsp_pw[8];

// -- editing related data --

int xwin=0, ywin=0;                  // Editing viewplane
short lt=0, rt=0;                    // left-button tile, right-button tile
unsigned char curzone=0;                      // current zone.
char scrollmode=0;                   // tile or pixel scroll modes.
char mouse_scroll=1;
char layertoggle[10];                // layer visible toggles
char el=0;                           // current editing layer.
char modified=0;                     // Current buffer modified or not.

// -- music stuff --

char is_playing=0;
char cmname[60];
//UNIMOD *mf=0;

// -- copy/paste & block fill --

unsigned short *copybuf=0;
int copybuf_wide=0,copybuf_deep=0;
int selx1=0, sely1=0, selx2=0, sely2=0;
int shifted=0, pasting=0;
extern char curtrack;

char vcedprog[80];

int moving_up=0,moving_down=0,moving_left=0,moving_right=0; // aen

// ================================= Code ====================================

RGB_MAP rgb_table;
COLOR_MAP trans_table;

void LoadTransTable() {
	FILE *fp=fopen("trans.tbl","rb");
	if (fp) {
		for (int y=0; y<256; y++) {
			for (int x=0; x<256; x++) {
				trans_table.data[y][x]=fgetc(fp);
			}
		}
		fclose(fp);
	} else {
		create_rgb_table(&rgb_table, pal, NULL);
		rgb_map = &rgb_table;

		create_trans_table(&trans_table, pal, 128, 128, 128, NULL);
	}
	color_map=&trans_table;
	set_trans_blender(0,0,0,128);
}

int rnd(int lo, int hi)
{
  int range=hi-lo+1;
  int i=rand() % range;
  return i+lo;
}

void PlayMusic(char *fname)
{
  fname=fname;
  /*
  if (!soundokay) return;
  if (is_playing)
  {
    MD_PlayStop();
    ML_Free(mf);
    is_playing=0;
  }
  mf=ML_LoadFN(fname);
  if (!mf)
  {
    Message("Could not load specified music.",500);
    return;
  }
  MP_Init(mf);
  md_numchn=mf->numchn; mp_loop=1;
  MD_PlayStart();
  is_playing=1;
  */
}

void StopMusic()
{
  /*
  if (!soundokay) return;
  MD_PlayStop();
  ML_Free(mf);
  is_playing=0;
  */
}

void LoadVSP(char *fname)
{ FILE *f;
  short ver;

  char bb[1024];
  sprintf(bb,"%s%s",cwd_prefix,fname);

  f=fopen(bb,"rb");
  if (!f)
     err("VSP file %s not found.",bb);

  fread(&ver, 1, 2, f);
  if (ver!=2 && ver!=3)
     err("VSP file %s is not the correct VSP version, reported %d",bb,ver);
  if (ver==2)
  {
    unsigned char p[768];
    fread(p, 1, 768, f);
	for (int index=0; index<256; index++) {
		pal[index].r=p[3*index+0];
		pal[index].g=p[3*index+1];
		pal[index].b=p[3*index+2];
	}
    set_intensity(63);
    fread(&numtiles, 1, 2, f);
    vsp=new BITMAP*[numtiles];//(unsigned char *) valloc(256*numtiles, "vsp data", 0);
	for (int index=0; index<numtiles; index++)
		vsp[index]=create_bitmap(16,16);
	unsigned char* data=new unsigned char[256*numtiles];
    fread(data, 256, numtiles, f);
	for (int index=0; index<numtiles; index++)
		for (int y=0; y<16; y++)
			for (int x=0; x<16; x++)
				putpixel(vsp[index],x,y, data[256*index + y*16+x]);
	delete[] data;
    //fread(vspanim, 8, 100, f);
	for (int index=0; index<100; index++) {
		fread(&vspanim[index].start,1,2,f);
		fread(&vspanim[index].finish,1,2,f);
		fread(&vspanim[index].delay,1,2,f);
		fread(&vspanim[index].mode,1,2,f);
	}
    fclose(f);
  }
  if (ver==3)
  {
    unsigned char p[768];
    fread(p, 1, 768, f);
	for (int index=0; index<256; index++) {
		pal[index].r=p[3*index+0];
		pal[index].g=p[3*index+1];
		pal[index].b=p[3*index+2];
	}
    set_intensity(63);
    fread(&numtiles, 1, 2, f);
	unsigned char* data = new unsigned char[256*numtiles];
    //vsp=(unsigned char *) valloc(256*numtiles, "vsp data", 0);
	vsp = new BITMAP*[numtiles];
	for (int index=0; index<numtiles; index++)
		vsp[index]=create_bitmap(16,16);
    fread(&bufsize, 1, 4, f);
    ReadCompressedLayer1(data, 256*numtiles, f);
	for (int index=0; index<numtiles; index++)
		for (int y=0; y<16; y++)
			for (int x=0; x<16; x++)
				putpixel(vsp[index],x,y, data[256*index + y*16+x]);
	delete[] data;
    fread(vspanim, 8, 100, f);
    fclose(f);
  }
  //for (int index=0; index<100; index++) {
	 // Log("start %d, finish %d, delay %d, mode %d",vspanim[index].start,vspanim[index].finish,vspanim[index].delay,vspanim[index].mode);
  //}
  
  black=makecol(0,0,0);
  white=makecol(255,255,255);
  darkred=makecol(128,0,0);
  darkw=makecol(128,128,128);
  winbg=makecol(175,175,175);
  titlebg=makecol(192,192,192);
  RegenMouseCursor();
  RegenSmallFont();
}

void SaveVSP(char *fname)
{ FILE *f;
  short ver;

  char bb[1024];
  sprintf(bb,"%s%s",cwd_prefix,fname);

  f=fopen(bb,"wb");
  ver=2; //3;
  fwrite(&ver, 1, 2, f);
 unsigned char p[768];
  for (int index=0; index<256; index++) {
	  p[3*index+0]=pal[index].r;
	  p[3*index+1]=pal[index].g;
	  p[3*index+2]=pal[index].b;
  }
  fwrite(p, 1, 768, f);
  fwrite(&numtiles, 1, 2, f);
  cb=(char *) valloc(numtiles*256, "vsp tempbuf", 0);
  bufsize=0;
  unsigned char* data = new unsigned char[256*numtiles];
  for (int index=0; index<numtiles; index++)
	  for (int y=0; y<16; y++)
		  for (int x=0; x<16; x++)
			  data[256*index + y*16+x] = getpixel(vsp[index],x,y);
  //Buffer* dest=new Buffer();
  //WriteCompressedLayer1(dest,data,numtiles*256);
  //delete[] data;
  //int length=dest->length(); fwrite(&length,1,4,f);
  //fwrite(dest->uchar_array(),1,length,f);
  //delete dest;
  fwrite(data,1,256*numtiles,f);

  //free(bufsize); // what the hell??? @_@
  fwrite(vspanim, 8, 100, f);
  fclose(f);
}

void CountUsedZones()
{ int i;

  i=255;
  while (i)
  {
     if (zones[i].script) break;
     if (zones[i].percent) break;
     if (zones[i].delay) break;
     if (zones[i].aaa) break;
     if (zones[i].entityscript) break;
     if (strlen(zones[i].name)) break;
     i--;
  }
  numzones=i+1;
}

void CountMoveScripts()
{ int i;

  i=99;
  while (i)
  {
     if (strlen(ms[i].t)) break;
     i--;
  }
  nms=(unsigned char)(i+1);
}

void CountCHRs()
{ int i;

  i=99;
  while (i)
  {
     if (strlen(chrlist[i].t)) break;
     i--;
  }
  nmchr=(unsigned char)(i+1);
}

void SaveMAP(char *fname)
{ FILE *f;
  int i;
  int ofstbl[100];
  int ct, t;

	char bb[1024];
	sprintf(bb,"%s%s",cwd_prefix,fname);
  f=fopen(bb, "wb");

  char* signature="MAPù5\0";
  fwrite(signature,1,6,f);
  
  char empty[4]={0,0,0,0}; // script offset
  fwrite(empty,1,4,f);

  fwrite(vspname, 1, 60, f);
  fwrite(musname, 1, 60, f);
  fwrite(rstring, 1, 20, f);
  fwrite(&xstart, 1, 2, f);
  fwrite(&ystart, 1, 2, f);
  fwrite(&wrap, 1, 1, f);
  
  char junk[50];
  memset(junk,0,50);
  fwrite(junk,1,50,f);

  fwrite(&numlayers, 1, 1, f);
  for (i=0; i<numlayers; i++)
      fwrite(&layer[i], 1, 12, f);

  for (i=0; i<numlayers; i++)
  {
     cb=(char *) valloc(layer[i].sizex*layer[i].sizey*2,"cmprs buf", 0);
     WriteCompressedLayer2(layers[i],(layer[i].sizex*layer[i].sizey));
     fwrite(&bufsize, 1, 4, f);
     fwrite(cb, 1, bufsize, f);
     vfree(cb);
  }
  // Compress and write Obstruction data

  cb=(char *) valloc(layer[0].sizex*layer[0].sizey*2, "cmprs buf", 0);
  WriteCompressedLayer1((unsigned char *)obstruct, (layer[0].sizex*layer[0].sizey));
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  WriteCompressedLayer1((unsigned char *)zone, (layer[0].sizex*layer[0].sizey));
  fwrite(&bufsize, 1, 4, f);
  fwrite(cb, 1, bufsize, f);
  vfree(cb);

  CountUsedZones();
  fwrite(&numzones, 1, 4, f);
  for (int index=0; index<numzones; index++) {
	  fwrite(zones[index].name,1,40,f);
	  fwrite(&zones[index].script,1,2,f);
	  fwrite(&zones[index].percent,1,2,f);
	  fwrite(&zones[index].delay,1,2,f);
	  fwrite(&zones[index].aaa,1,2,f);
	  fwrite(&zones[index].entityscript,1,2,f);
  }

  CountCHRs();
  fwrite(&nmchr, 1, 1, f);
  fwrite(chrlist, 60, nmchr, f);

  fwrite(&entities, 1, 1, f);
  //fwrite(entity, sizeof(entity)/256, entities, f);
  for (int index=0; index<entities; index++) {
	  entity_r* e=&entity[index];
	  fwrite(&e->x,1,4,f);
	  fwrite(&e->y,1,4,f);
	  fwrite(&e->tx,1,2,f);
	  fwrite(&e->ty,1,2,f);
	  fwrite(&e->facing,1,1,f);
	  fwrite(&e->moving,1,1,f);
	  fwrite(&e->movecnt,1,1,f);
	  fwrite(&e->frame,1,1,f);
	  fwrite(&e->specframe,1,1,f);
	  fwrite(&e->chrindex,1,1,f);
	  fwrite(&e->reset,1,1,f);
	  fwrite(&e->obsmode1,1,1,f);
	  fwrite(&e->obsmode2,1,1,f);
	  fwrite(&e->speed,1,1,f);
	  fwrite(&e->speedct,1,1,f);

	  unsigned char padding[2]={0xff,0xff};
	  fwrite(&padding[0],1,1,f);

	  int empty=0;
	  fwrite(&empty,1,4,f); //fwrite(e->animofs,1,4,f);
	  fwrite(&empty,1,4,f); //fwrite(e->moveofs,1,4,f);
	  
	  fwrite(&e->face,1,1,f);
	  fwrite(&e->actm,1,1,f);
	  fwrite(&e->movecode,1,1,f);
	  fwrite(&e->movescript,1,1,f);
	  fwrite(&e->ctr,1,1,f);
	  fwrite(&e->mode,1,1,f);

	  fwrite(padding,1,2,f);

	  fwrite(&e->step,1,2,f);
	  fwrite(&e->delay,1,2,f);
	  fwrite(&e->stepctr,1,2,f);
	  fwrite(&e->delayct,1,2,f); // ???
	  fwrite(&e->data1,1,2,f);
	  fwrite(&e->data2,1,2,f);
	  fwrite(&e->data3,1,2,f);
	  fwrite(&e->data4,1,2,f);
	  fwrite(&e->data5,1,2,f);
	  fwrite(&e->data6,1,2,f);
	  fwrite(&e->actscript,1,4,f);
	  fwrite(&e->expand1,1,4,f); // ???
	  fwrite(&e->expand2,1,4,f); // ???
	  fwrite(&e->expand3,1,4,f); // ???
	  fwrite(&e->expand4,1,4,f); // ???
	  fwrite(&e->desc,1,20,f);
  }

  CountMoveScripts();
  fwrite(&nms, 1, 1, f);     // Calc offset buffer
  ct=0;
  for (i=0; i<nms; i++)
  {
    ofstbl[i]=ct;
    t=(int)strlen(ms[i].t)+1;
    ct+=t;
  }
  fwrite(&ct, 1, 4, f);        // string table length
  fwrite(ofstbl, nms, 4, f);  // write offset buffer
  for (i=0; i<nms; i++)        // write string table
  {
    fwrite(ms[i].t, 1, strlen(ms[i].t)+1, f);
  }

  ct=0;  // 0 Things
  fwrite(&ct, 1, 4, f);
  ct=ftell(f);

  i=1; fwrite(&i, 1, 4, f);
  i=0; fwrite(&i, 1, 4, f);
  i=1; fwrite(&i, 1, 4, f);
  i=9; fwrite(&i, 1, 1, f);

  fseek(f,6,0);
  fwrite(&ct, 1, 4, f);
  fclose(f);
}

void LoadOldMAP(FILE *f)
{ int i, j, ct;
  int ofstbl[100];
  char pm,pd,pc,c;

  // Reads a VERGE 1 format version 4 MAP file.
  fseek(f, 1, 0);
  fread(vspname, 1, 13, f);
  fread(musname, 1, 13, f);
  fread(&pc, 1, 1, f);             // }
  fread(&pm, 1, 1, f);             // } Parallax controls
  fread(&pd, 1, 1, f);             // }
  fread(strbuf, 1, 32, f);         // unused - misc
  fread(&xstart, 1, 2, f);
  fread(&ystart, 1, 2, f);
  fread(strbuf, 1, 2, f);          // unused - misc
  fread(&layer[0].sizex, 1, 2, f);
  fread(&layer[0].sizey, 1, 2, f);
  layer[1].sizex=layer[0].sizex;
  layer[1].sizey=layer[0].sizey;
  fread(strbuf, 1, 28, f);         // unused - header pad

  layers[0]=(unsigned short *) valloc((layer[0].sizex*(layer[0].sizey+1)*2)+2, "layer data", 0);
  layers[1]=(unsigned short *) valloc((layer[1].sizex*(layer[1].sizey+1)*2)+2, "layer data", 1);
   obstruct=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "obstruct map", 0);
       zone=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "zone map", 0);
         cb=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "comprs buf", 0);

  fread(layers[0], 2, layer[0].sizex * layer[0].sizey, f);
  fread(layers[1], 2, layer[1].sizex * layer[1].sizey, f);
  fread(cb,        1, layer[0].sizex * layer[0].sizey, f);

  // Convert MAP Properties layer to respective Zone and Obstruction layers

  for (j=0; j<layer[0].sizey; j++)
    for (i=0; i<layer[0].sizex; i++)
    {
      if ((cb[(j*layer[0].sizex)+i] & 1)==1)
         c=1; else c=0;
       obstruct[(j*layer[0].sizex)+i]=c;
       c=(char)( cb[(j*layer[0].sizex)+i] >> 1 );
       zone[(j*layer[0].sizex)+i]=c;
    }
  vfree(cb);

  // Load and convert zone data records.

  for (i=0; i<128; i++)
  {
    fread(&tzone, 1, sizeof tzone, f);
    zones[i].script=tzone.callevent;
    zones[i].percent=tzone.percent;
    zones[i].delay=tzone.delay;
    zones[i].aaa=tzone.aaa;
    zones[i].entityscript=0;
    memcpy(zones[i].name, tzone.zonename, 16);
  }

  for (i=0; i<100; i++)                  // Load and convert CHR list
    fread(chrlist[i].t, 1, 13, f);
//  DoCHRdealy();

  fread(&entities, 1, 1, f); fseek(f, 3, 1);
  memset(entity, 0, sizeof entity);
  Log("old entities:");
  for (i=0; i<entities; i++)             // Load and convert entity records
	{
		oldent_r* o=&oldent;
		fread(&o->x,1,2,f);				Log("  x=%d",o->x);
		fread(&o->y,1,2,f);				Log("  y=%d",o->y);
		fread(&o->facing,1,1,f);		Log("  facing=%d",o->facing);
		fread(&o->moving,1,1,f);
		fread(&o->movcnt,1,1,f);
		fread(&o->framectr,1,1,f);
		fread(&o->specframe,1,1,f);		Log("  specframe=%d",o->specframe);
		fread(&o->chrindex,1,1,f);		Log("  chrindex=%d",o->chrindex);
		fread(&o->movecode,1,1,f);		Log("  movecode=%d",o->movecode);
		fread(&o->activmode,1,1,f);		Log("  activmode=%d",o->activmode);
		fread(&o->obsmode,1,1,f);		Log("  obsmode=%d",o->obsmode);
		fread(&o->v1,1,1,f);
		fread(&o->v2,1,1,f);
		fread(&o->v3,1,1,f);
		fread(&o->actscript,1,4,f);
		fread(&o->movescript,1,4,f);
		fread(&o->speed,1,1,f);			Log("  speed=%d",o->speed);
		fread(&o->speedct,1,1,f);
		fread(&o->step,1,2,f);
		fread(&o->delay,1,2,f);
		fread(&o->data1,1,2,f);			Log("  data1=%d",o->data1);
		fread(&o->data2,1,2,f);			Log("  data2=%d",o->data2);
		fread(&o->data3,1,2,f);			Log("  data3=%d",o->data3);
		fread(&o->data4,1,2,f);			Log("  data4=%d",o->data4);
		fread(&o->delayct,1,2,f);
		fread(&o->adjactv,1,2,f);		Log("  adjactv=%d",o->adjactv);
		fread(&o->x1,1,2,f);			Log("  x1=%d",o->x1);
		fread(&o->y1,1,2,f);			Log("  y1=%d",o->y1);
		fread(&o->x2,1,2,f);			Log("  x2=%d",o->x2);
		fread(&o->y2,1,2,f);			Log("  y2=%d",o->y2);
		fread(&o->curcmd,1,1,f);
		fread(&o->cmdarg,1,1,f);
		int temp=0;
		fread(&temp,1,4,f); // offset in script parsing
		fread(&o->face,1,1,f);			Log("  face=%d",o->face);
		fread(&o->chasing,1,1,f);		Log("  chasing=%d",o->chasing);
		fread(&o->chasespeed,1,1,f);	Log("  chasespeed=%d",o->chasespeed);
		fread(&o->chasedist,1,1,f);		Log("  chasedist=%d",o->chasedist);
		fread(&o->cx,1,2,f);			Log("  cx=%d",o->cx);
		fread(&o->cy,1,2,f);			Log("  cy=%d",o->cy);
		fread(&o->expand1,1,4,f);
		fread(o->entitydesc,1,20,f);	Log("  entitydesc=%s", o->entitydesc);
		//fread(&oldent, 1, 88, f);

		entity[i].x=o->x;
		entity[i].y=o->y;
		entity[i].chrindex=(unsigned char)( o->chrindex-5 );
		entity[i].obsmode1=(unsigned char)( o->obsmode^1 );
		entity[i].obsmode2=1;
		entity[i].movecode=o->movecode;
		if (entity[i].movecode==3) entity[i].movecode=2;
		else if (entity[i].movecode==2) entity[i].movecode=3;
		entity[i].speed=o->speed;
		entity[i].face=o->face;
		entity[i].actm=o->activmode;
		entity[i].movescript=(char)o->movescript;
		entity[i].step=o->step;
		entity[i].delay=o->delay;
		entity[i].data2=0;
		entity[i].data3=0;
		entity[i].data5=0;
		entity[i].data6=0;
		entity[i].actscript=o->actscript;
		if (entity[i].movecode==2) entity[i].data2=o->data3;
		if (entity[i].movecode==3)
		{
			entity[i].data2=o->x1;
			entity[i].data3=o->y1;
			entity[i].data5=o->x2;
			entity[i].data6=o->y2;
		}
		memcpy(entity[i].desc, o->entitydesc, 20);
  }

  fread(&nms, 1, 1, f);
  fread(&ct, 1, 4, f);
  fread(ofstbl, nms, 4, f);
  ofstbl[nms]=ct;
  for (i=0; i<nms; i++)
   fread(ms[i].t, 1, (ofstbl[i+1]-ofstbl[i]), f);
  fclose(f);

  numlayers=2;
  layertoggle[0]=1; layertoggle[1]=1; layertoggle[2]=0; layertoggle[3]=0;
  layertoggle[4]=0; layertoggle[5]=0; layertoggle[6]=0; layertoggle[7]=0;
  memcpy(rstring,"1E2",3);
  switch (pc)
  {
    case 0:
    case 1: layer[0].pmultx=1; layer[0].pmulty=1; layer[0].pdivx=1; layer[0].pdivy=1;
            layer[1].pmultx=1; layer[1].pmulty=1; layer[1].pdivx=1; layer[1].pdivy=1; break;
    case 2: layer[0].pmultx=pm; layer[0].pmulty=pm; layer[0].pdivx=pd; layer[0].pdivy=pd;
            layer[1].pmultx=1; layer[1].pmulty=1; layer[1].pdivx=1; layer[1].pdivy=1; break;
    case 3: layer[0].pmultx=1; layer[0].pmulty=1; layer[0].pdivx=1; layer[0].pdivy=1;
            layer[1].pmultx=pm; layer[1].pmulty=pm; layer[1].pdivx=pd; layer[1].pdivy=pd; break;
  }

  LoadVSP(vspname);
  sprintf(strbuf, "MapEd %s Copyright (C)1998 vecna", ME2_VERSION);
  Message(strbuf,700);
  Message("Old VERGE format 4 MAP loaded.",300);
  if (strlen(musname))
    PlayMusic(musname);
}

void LoadMAP(char *fname)
{ FILE *f;
  int i, ct;
  int ofstbl[100];

  char bb[1024];
  sprintf(bb,"%s%s",cwd_prefix,fname);

  Log("Loading map: %s", bb);
  f=fopen(bb, "rb");
  if (!f)
        err("Could not find %s.",bb);
  char signature[6];
  fread(signature, 1, 6, f);
  if (strcmp(signature,"MAPù5"))
  {
     if (signature[0]==4)
     {
        LoadOldMAP(f);
        return;
     }
     err("%s is not a recognized MAP file.",bb);
  }

  fread(&i, 1, 4, f);			Log("  map vc script offset: %d", i);
  fread(vspname, 1, 60, f);		Log("  vspname: %s", vspname);
  fread(musname, 1, 60, f);		Log("  musicnname: %s", musname);
  fread(rstring, 1, 20, f);		Log("  rstring: %s", rstring);
  fread(&xstart, 1, 2, f);		Log("  xstart: %d", xstart);
  fread(&ystart, 1, 2, f);		Log("  ystart: %d", ystart);
  fread(&wrap, 1, 1, f);		Log("  wrap: %d", wrap);
  char junk[50];
  fread(junk, 1, 50, f);
  fread(&numlayers, 1, 1, f);	Log("  numlayers: %d", numlayers);

  for (i=0; i<numlayers; i++) {
	  fread(&layer[i].pmultx,1,1,f);
	  fread(&layer[i].pdivx,1,1,f); if (layer[i].pdivx==0) layer[i].pdivx=1;
	  fread(&layer[i].pmulty,1,1,f);
	  fread(&layer[i].pdivy,1,1,f); if (layer[i].pdivy==0) layer[i].pdivy=1;
	  fread(&layer[i].sizex,1,2,f);
	  fread(&layer[i].sizey,1,2,f);
	  fread(&layer[i].trans,1,1,f);
	  fread(&layer[i].hline,1,1,f);
	  
	  char pad[2];
		fread(pad,1,2,f);
  }

  for (i=0; i<numlayers; i++)
  {
     fread(&bufsize, 1, 4, f);
     layers[i]=(unsigned short *) valloc(layer[i].sizex*(layer[i].sizey+2)*2, "layer data", i);
     ReadCompressedLayer2(layers[i],(layer[i].sizex * layer[i].sizey),f);
     layertoggle[i]=1;
  }
  for (; i<10; i++)
     layertoggle[i]=0;

  obstruct=(char *) valloc(layer[0].sizex*(layer[0].sizey+2),"obstruct map", 0);
  zone=(char *) valloc(layer[0].sizex*(layer[0].sizey+2), "zone map", 0);

  fread(&bufsize, 1, 4, f);
  ReadCompressedLayer1((unsigned char *)obstruct,(layer[0].sizex * layer[0].sizey), f);
  fread(&bufsize, 1, 4, f);
  ReadCompressedLayer1((unsigned char *)zone,(layer[0].sizex * layer[0].sizey), f);

  //memset(zones, 0, sizeof zones);
  fread(&numzones, 1, 4, f);
  Log("numzones: %d", numzones);
  //fread(zones, numzones, 50, f);
  for (int index=0; index<numzones; index++) {
	  fread(zones[index].name,1,40,f);
	  fread(&zones[index].script,1,2,f);
	  fread(&zones[index].percent,1,2,f);
	  fread(&zones[index].delay,1,2,f);
	  fread(&zones[index].aaa,1,2,f);
	  fread(&zones[index].entityscript,1,2,f);
  }

  memset(chrlist, 0, sizeof chrlist);
  fread(&nmchr, 1, 1, f);
  Log("num chars: %d", nmchr);
  fread(chrlist, 60, nmchr, f);
//  DoCHRdealy();

  fread(&entities, 1, 1, f);
  Log("# entities: %d", entities);
  //fread(entity, sizeof(entity)/256, entities, f);
  for (int index=0; index<entities; index++) {
	  entity_r* e=&entity[index];
	  fread(&e->x,1,4,f);
	  fread(&e->y,1,4,f);
	  fread(&e->tx,1,2,f);
	  fread(&e->ty,1,2,f);
	  fread(&e->facing,1,1,f);
	  fread(&e->moving,1,1,f);
	  fread(&e->movecnt,1,1,f);
	  fread(&e->frame,1,1,f);
	  fread(&e->specframe,1,1,f);
	  fread(&e->chrindex,1,1,f);
	  fread(&e->reset,1,1,f);
	  fread(&e->obsmode1,1,1,f);
	  fread(&e->obsmode2,1,1,f);
	  fread(&e->speed,1,1,f);
	  fread(&e->speedct,1,1,f);

	  char padding[2];
	  fread(&padding[0],1,1,f);

	  int temp;
	  fread(&temp,1,4,f);//fread(e->animofs,1,4,f);
	  fread(&temp,1,4,f);//fread(e->moveofs,1,4,f);
	  
	  fread(&e->face,1,1,f);
	  fread(&e->actm,1,1,f);
	  fread(&e->movecode,1,1,f);
	  fread(&e->movescript,1,1,f);
	  fread(&e->ctr,1,1,f);
	  fread(&e->mode,1,1,f);

	  fread(padding,1,2,f);

	  fread(&e->step,1,2,f);
	  fread(&e->delay,1,2,f);
	  fread(&e->stepctr,1,2,f);
	  fread(&e->delayct,1,2,f); // ???
	  fread(&e->data1,1,2,f);
	  fread(&e->data2,1,2,f);
	  fread(&e->data3,1,2,f);
	  fread(&e->data4,1,2,f);
	  fread(&e->data5,1,2,f);
	  fread(&e->data6,1,2,f);
	  fread(&e->actscript,1,4,f);
	  fread(&e->expand1,1,4,f); // ???
	  fread(&e->expand2,1,4,f); // ???
	  fread(&e->expand3,1,4,f); // ???
	  fread(&e->expand4,1,4,f); // ???
	  fread(&e->desc,1,20,f);
	  Log("%d: description=%s",index,e->desc);
  }

  fread(&nms, 1, 1, f);
  Log("# movescripts: %d",nms);
  fread(&ct, 1, 4, f);
  fread(ofstbl, nms, 4, f);
  ofstbl[nms]=ct;
  for (i=0; i<nms; i++)
    fread(ms[i].t, 1, (ofstbl[i+1]-ofstbl[i]), f);
  fclose(f);

  LoadVSP(vspname);
  sprintf(strbuf, "MapEd %s Copyright (C)1998 vecna", ME2_VERSION);
  Message(strbuf,700);
  Message("V2 MAP loaded.",300);
  if (strlen(musname))
    PlayMusic(musname);
}

void InitTileIDX()
{ short i;

  memset(vadelay, 0, 200);
  for (i=0; i<MAXTILES; i++)
      tileidx[i]=i;
}

void CheckTimerStuff()
{ static inss=0;

  if (backupct>30000)
  {
    Message("Generating Auto-backup",150);
    SaveMAP("$$BACKUP.MAP");
    SaveVSP("$$BACKUP.VSP");
    backupct=0;
  }
  if (idlect>100*60*3 && !inss)
  {
    inss=1;
    ScreenSaver();
    idlect=0;
    inss=0;
  }
}

extern int difficulty;

void SaveNewCFG()
{ FILE *f;

  f=fopen("maped.cfg","w");
  //if (nocdaudio) fprintf(f,"nocdaudio\n");
  fprintf(f,"vidmode %d \n", vm);
  fprintf(f,"pad %d \n", pad);
  fprintf(f,"scrollmode %d \n",scrollmode);
  fprintf(f,"mouse_scroll %d \n",mouse_scroll&1); // aen
  fprintf(f,"black %d \n",black);
  fprintf(f,"white %d \n",brightw);
  fprintf(f,"winbg %d \n",winbg);
  fprintf(f,"shadow %d \n",darkw);
  fprintf(f,"darkred %d \n",darkred);
  fprintf(f,"titlebg %d \n",titlebg);
  fprintf(f,"th %d \n", th);
  fprintf(f,"mh %d \n", mh);
  //fprintf(f,"md_device %d \n", md_device==3?3:0);
  fprintf(f,"amxofs %d \n",amxofs);
  fprintf(f,"amyofs %d \n",amyofs);
  fprintf(f,"mmxofs %d \n",mmxofs);
  fprintf(f,"mmyofs %d \n",mmyofs);
  fprintf(f,"cnxofs %d \n",cnxofs);
  fprintf(f,"cnyofs %d \n",cnyofs);
  fprintf(f,"lmxofs %d \n",lmxofs);
  fprintf(f,"lmyofs %d \n",lmyofs);
  fprintf(f,"lvxofs %d \n",lvxofs);
  fprintf(f,"lvyofs %d \n",lvyofs);
  fprintf(f,"mpxofs %d \n",mpxofs);
  fprintf(f,"mpyofs %d \n",mpyofs);
  fprintf(f,"vaxofs %d \n",vaxofs);
  fprintf(f,"vayofs %d \n",vayofs);
  fprintf(f,"zexofs %d \n",zexofs);
  fprintf(f,"zeyofs %d \n",zeyofs);
  fprintf(f,"exofs %d \n",exofs);
  fprintf(f,"eyofs %d \n",eyofs);
  fprintf(f,"prxofs %d \n",prxofs);
  fprintf(f,"pryofs %d \n",pryofs);
  fprintf(f,"rsxofs %d \n",rsxofs);
  fprintf(f,"rsyofs %d \n",rsyofs);
  fprintf(f,"vcedprog %s \n",vcedprog);
  fprintf(f,"difficulty %d \n",difficulty);
//  fprintf(f,"pageflipping %d \n",pageflipping);
  fprintf(f,"windowed %d \n",windowed);
  fclose(f);
}

//#include "controls.cpp"

void ShellToDOS()
{
  //ShutdownVideo();
  //ShutdownKeyboard();
  //ShutdownTimer();
  ///*
  //MD_PlayStop();
  //MD_Exit();
  //*/

  //system("COMMAND.COM");

  ///*
  //if (!MD_Init())
  //{
  //  printf("Couldn't initialize sound: %s.\n", myerr);
  //  delay(500);
  //  soundokay=0;
  //}
  //if (strlen(musname)) PlayMusic(musname);
  //*/
  //if (1==vm)
  //InitVideo(640,480,120,false);
  //else InitVideo(320,240,120,false);
  //set_intensity(63);
  //InitMouse();
  //InitKeyboard();
  //InitTimer();
}

void ShellVERGE()
{
 // ShutdownVideo();
 // ShutdownKeyboard();
 // ShutdownTimer();
 // /*
 // MD_PlayStop();
 // MD_Exit();
 // */

 // system("VERGE");

 // /*
 // if (!MD_Init())
 // {
 //   printf("Couldn't initialize sound: %s.\n", myerr);
 //   delay(500);
 //   soundokay=0;
 // }
 // if (strlen(musname)) PlayMusic(musname);
 // */
 // if (1==vm)
	//InitVideo(640,480,120,false);
 // else InitVideo(320,240,120,false);
 // set_intensity(63);
 // InitMouse();
 // InitKeyboard();
 // InitTimer();
}

void ShellMAP()
{
 // ShutdownVideo();
 // ShutdownKeyboard();
 // ShutdownTimer();
 // /*
 // MD_PlayStop();
 // MD_Exit();
 // */

 // sprintf(strbuf,"verge %s",mapname);
 // system(strbuf);

 // /*
 // if (!MD_Init())
 // {
 //   printf("Couldn't initialize sound: %s.\n", myerr);
 //   delay(500);
 //   soundokay=0;
 // }
 // if (strlen(musname)) PlayMusic(musname);
 // */
 // if (1==vm)
	//InitVideo(640,480,120,false);
 // else InitVideo(320,240,120,false);
 // set_intensity(63);
 // InitMouse();
 // InitKeyboard();
 // InitTimer();
}

void ShellEditMAP()
{
 // char fn[80];
 // char *p;

 // key[KEY_ALT]=0;
 // key[KEY_V]=0;
 // ShutdownVideo();
 // ShutdownKeyboard();
 // ShutdownTimer();
 // /*
 // MD_PlayStop();
 // MD_Exit();
 // */

 // sprintf(fn,"%s",mapname);
 // p=fn;
 // while (*p)
 // {
 //   if (*p=='.') *p=0;
 //   p++;
 // }
 // sprintf(strbuf,"%s %s.vc", vcedprog, fn);
 // system(strbuf);

 // /*
 // if (!MD_Init())
 // {
 //   printf("Couldn't initialize sound: %s.\n", myerr);
 //   delay(500);
 //   soundokay=0;
 // }
 // if (strlen(musname)) PlayMusic(musname);
 // */
	//if (1==vm)
	//InitVideo(640,480,120,false);
	//else InitVideo(320,240,120,false);
 // set_intensity(63);
 // InitMouse();
 // InitKeyboard();
 // InitTimer();
}

void ShellEditSystem()
{
 // key[KEY_ALT]=0;
 // key[KEY_S]=0;
 // ShutdownVideo();
 // ShutdownKeyboard();
 // ShutdownTimer();
 // /*
 // MD_PlayStop();
 // MD_Exit();
 // */

 // sprintf(strbuf,"%s system.vc", vcedprog);
 // system(strbuf);

 // /*
 // if (!MD_Init())
 // {
 //   printf("Couldn't initialize sound: %s.\n", myerr);
 //   delay(500);
 //   soundokay=0;
 // }
 // if (strlen(musname)) PlayMusic(musname);
 // */
 // if (1==vm)
	//InitVideo(640,480,120,false);
 // else InitVideo(320,240,120,false);
 // set_intensity(63);
 // InitMouse();
 // InitKeyboard();
 // InitTimer();
}

char s[256];

void CompileAll()
{
  //FILE *f;
  //char *p;

  ///*
  //MD_PlayStop();
  //MD_Exit();
  //*/

  //sprintf(strbuf,"vcc all q",mapname);
  //system(strbuf);

  ///*
  //if (!MD_Init())
  //{
  //  printf("Couldn't initialize sound: %s.\n", myerr);
  //  delay(500);
  //  soundokay=0;
  //}
  //if (strlen(musname)) PlayMusic(musname);
  //*/

  //f=fopen("error.txt","r");
  //if (!f)
  //{
  //  Message("All VC scripts sucessfully compiled.",300);
  //  return;
  //}

  //fgets(s,99,f);
  //fclose(f);
  //p=s;
  //while (*p)
  //{
  //  if (*p==13 || *p==10) *p=0;
  //  p++;
  //}
  //VCNotify(s);
}

void CompileMAP()
{
  //FILE *f;
  //char *p;

  ///*
  //MD_PlayStop();
  //MD_Exit();
  //*/

  //sprintf(strbuf,"vcc %s q",mapname);
  //system(strbuf);

  ///*
  //if (!MD_Init())
  //{
  //  printf("Couldn't initialize sound: %s.\n", myerr);
  //  delay(500);
  //  soundokay=0;
  //}
  //if (strlen(musname)) PlayMusic(musname);
  //*/

  //f=fopen("error.txt","r");
  //if (!f)
  //      return;

  //fgets(s,99,f);
  //fclose(f);
  //p=s;
  //while (*p)
  //{
  //  if (*p==13 || *p==10) *p=0;
  //  p++;
  //}
  //VCNotify(s);
}

void tickhandler(void)
{
  /*
  MP_HandleTick();
  MD_SetBPM(mp_bpm);
  */
}

void RenderHighlight()
{ int zx, zy;
  int xw, yw;

  if (mh)
  {
    xw=xwin&15;
    yw=ywin&15;
    zx=((mouse_x+xw)&~15)-xw;
    zy=((mouse_y+yw)&~15)-yw;

    DrawHighlight(zx-1, zy-1);
  }
}

int EntityThere(int xw, int yw)
{ int i;

  for (i=0; i<entities; i++)
  {
    if (entity[i].x==xw && entity[i].y==yw) return i+1;
  }
  return 0;
}

void AllocateEntity(int xw, int yw)
{
  memset(&entity[entities], 0, sizeof(entity) / 256);
  entity[entities].x=xw;
  entity[entities].y=yw;
  entities++;
}

void ProcessEntity(int xw, int yw)
{ int a;

  a=EntityThere(xw,yw);
  if (!a)
  {
     AllocateEntity(xw,yw);
     while (mouse_b)
       ReadMouse();
     mouse_b=0;
     return;
  }
  EntityEditor(a-1);
}

void DeleteEntity(int xw, int yw)
{ int i,a;
  char t[60];

  a=EntityThere(xw,yw)-1;
  sprintf(t,"Delete entity %d?",a);
  if (!Confirm(t)) return;
  for (i=a; i<entities; i++)
  {
    entity[i]=entity[i+1];
  }
  entities--;
}


#include "vergepal.h"

void experiment() {
	InitVideo(640,480,60,false);
	install_keyboard();

   BITMAP* background = load_bitmap("c:\\bmp2map\\island.pcx", pal);
   set_palette(pal);
	LoadTransTable();

   do {
	   poll_mouse();
	   rectfill(get_offscreen(),0,0,SCREEN_W,SCREEN_H,0);
	   draw_sprite(get_offscreen(),background,0,0);
	   draw_trans_sprite(get_offscreen(),background,50,50);
	   ShowPage();

   } while (!keypressed());
   clear_keybuf();

	allegro_exit();
	exit(-1);
}

int main(int argc, char *argv[]) {
	cwd_prefix[0]='\0';
	allegro_init();
	//experiment();

	if (argc==2 && !exists(argv[1])) {
		allegro_message("File does not exist: %s\n",argv[1]);
		exit(-1);
	}

	remove(LOGFILE);

	memset(chrs, 0, sizeof chrs);

	Log("GetConfig");
	GetConfig("maped.cfg");

	Log("Initialize timer");
	InitTimer();
	Log("Initialize keyboard IRQ handler");
	InitKeyboard();

	Log("Initialize video");
	if (1==vm)
	InitVideo(640,480,120,windowed!=0);
	else InitVideo(320,240,120,windowed!=0);
	Log("Initialize mouse");
	InitMouse();

	Log("Initializing map data");
	if (argc==1 && !exists("untitled.map")) {
		PALETTE vpal;
		for (int index=0; index<256; index++) {
			vpal[index].r=vergepal[3*index+0];
			vpal[index].g=vergepal[3*index+1];
			vpal[index].b=vergepal[3*index+2];
		}
		SetPalette(vpal);//vergepal);
		//memcpy(pal, vergepal, 768);
		memcpy(mapname,"UNTITLED.MAP",13);
		memcpy(vspname,"UNTITLED.VSP",13);
		memcpy(rstring,"1E",2);
		numlayers=1;

		// aen: default newmap dimensions set to 100x100
		layer[0].pmultx=1;  layer[0].pmulty=1;
		layer[0].pdivx=1;   layer[0].pdivy=1;
		layer[0].sizex=100; layer[0].sizey=100;
		layer[0].trans=0;   layer[0].hline=0;

		layers[0]=(unsigned short *) valloc((layer[0].sizex*layer[0].sizey*2)+4,"layer data",0);
		//memset(layers[0],0,(layer[0].sizex*layer[0].sizey)*2);
		obstruct=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"obstruct map",0);
		zone=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"zone map", 0);

		// aen: default number of tiles set to 100
		numtiles=100;
		vsp=new BITMAP*[numtiles];//(unsigned char *) valloc(256 * numtiles,"vsp data", 0);
		for (int index=0; index<numtiles; index++) {
			vsp[index]=create_bitmap(16,16);
			rectfill(vsp[index],0,0,16,16,0);
		}
		wrap=0; el=0; layertoggle[0]=1;
		InitTileIDX();
	} else {
		if (argc==1)
			memcpy(mapname, "untitled.map",13);
		else
			memcpy(mapname, argv[1], strlen(argv[1]));
		LoadMAP(mapname);
		InitTileIDX();
	}
	Log("LoadTransTbl");
	LoadTransTable();

	Log("Entering main loop");

thingy:
	while (!(key[KEY_ALT] && key[KEY_X])) {
		//Log("ProcessControls()");
		ProcessControls();
		while (tick) {
			tick--;
			//Log("PollMovement()");
			PollMovement();
			//Log("CheckTileAnimation()");
			CheckTileAnimation();
		}
		//Log("RenderMap()");
		RenderMap();
		//Log("RenderHighlight()");
		RenderHighlight();
		//Log("RenderGUI()");
		RenderGUI();
		//Log("ShowPage()");
		ShowPage();
	}
	//Log("Exiting main loop");

	if (modified) {
		if (!Confirm("Lose unsaved changes?")) {
			key[KEY_ALT]=0;
			key[KEY_X]=0;
			goto thingy;
		}
	}
	//Log("Exited main loop");

	ShutdownVideo();
	ShutdownKeyboard();
	ShutdownTimer();
	SaveNewCFG();
	remove("$$BACKUP.MAP");
	remove("$$BACKUP.VSP");

        /*
        if (curtrack != 1)
		CD_Stop();
        CD_DeInit();
        MD_PlayStop();
        ML_Free(mf);
        MD_Exit();
        */

	return 0;
}
END_OF_MAIN()