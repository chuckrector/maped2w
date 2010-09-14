#include "browse.h"
#include "config.h"
#include "maped.h"
#include "guicomp.h"
#include "mouse.h"
#include "error.h"
#include "vdriver.h"
#include "smalfont.h"
#include "keyboard.h"
#include "log.h"
#include "gui.h"
#include "render.h"
#include "tilesel.h" // RenderSelector
#include "timer.h" // systemtime

#include <cstring>
#include <stack>
#include <direct.h>
#include <io.h>

char cwd_prefix[1024];

int bxsize=130, bysize=170;
int bxofs=80, byofs=25;
int bmode=0;

static EntryList* get_entries(char n, char* m1, char* m2, char* m3, char* m4) {
	long hFile;
	struct _finddata_t f;
	char *d;
	EntryList* entries=new EntryList();

	char buffer[1024];
	
	if (cwd_prefix[0]!='\0') {
		sprintf(buffer,"%s*.",cwd_prefix);
	} else {
		sprintf(buffer,"*.");
	}
	Log("Getting directories for: %s",buffer);

	// get all directories
	if ((hFile = _findfirst(buffer, &f)) != -1L) {
		if (f.attrib==_A_SUBDIR && strcmp(f.name,".")) {
			sprintf(buffer,"./%s",f.name);
			entries->add(new Entry(buffer));
		}
		//while (!_dos_findnext(f)) {
		while (_findnext(hFile, &f) == 0) {
			if (f.attrib==_A_SUBDIR && strcmp(f.name,".")) {
				sprintf(buffer,"./%s",f.name);
				entries->add(new Entry(buffer));
			}
		}
		_findclose(hFile);
	}

	for (int i=0; i<n; i++) {
		switch (i) {
			case 0: d="AJDLMMZZ.GAH"; break;
			case 1: d=m1; break;
			case 2: d=m2; break;
			case 3: d=m3; break;
			case 4: d=m4; break;
			default: {
				err("uhh...");
				break;
			}
		}

		if (cwd_prefix[0]!='\0') {
			sprintf(buffer,"%s%s",cwd_prefix,d);
		} else {
			sprintf(buffer,"%s",d);
		}
    	Log("Getting entries for: %s",buffer);
		if ((hFile = _findfirst(buffer, &f)) == -1L) {
			continue;
		}
		entries->add(new Entry(f.name));
		while (_findnext(hFile, &f) == 0) {
			entries->add(new Entry(f.name));
		}
		_findclose(hFile);
	}

	return entries;
}

void Browse(char n, char *m1, char *m2, char *m3, char *m4, char *dest) {
	int i;
	//int mode;
	char *blah;
	int ofs=0, cfs=0, a, b;
	int done=0, moving=0;
	int mxo, myo, cursorblink;
	int ctf=0, cb=0, c, t;
	char *str;
	//int zx,zy,zw;
	int dragging=0;

	char cwd_buf[_MAX_PATH];
	char newdir_buf[_MAX_PATH];
	//cwd_prefix[0]='\0';
	Entry* entry=0;

	//f=(struct find_t *) valloc(sizeof *f, "browse() tmp",0);
	blah=(char *) valloc(13*256,"browse()tmp2", 0);
	//memset(blah, 0, 13*256);
	n++;

	EntryList* entries=get_entries(n,m1,m2,m3,m4);

	//vfree(f);
	last_pressed=0;
	a=entries->count<22?117:2574/entries->count;
	//errn("%d",fc);
	do {
		if (!bmode) {
			RenderMap();
			RenderGUI();
		} else {
			RenderSelector();
			RenderMessages();
		}
		if (ctf && (int)systemtime >= cursorblink) {
			cb ^= 1;
			cursorblink = systemtime+40;
		}
		if (moving) {
			bxofs = (mouse_x-mxo);
			byofs = (mouse_y-myo);
			if (!mouse_b) moving=0;
			if (bxofs<0) bxofs = 0;
			if (byofs<0) byofs = 0;
			if (bxofs+bxsize>sx) bxofs = sx-bxsize;
			if (byofs+bysize>sy) byofs = sy-bysize;
		}

		Window(bxofs, byofs, bxofs+bxsize, byofs+bysize, "Browse");
		Button(bxofs+65, byofs+157, "OK");
		Button(bxofs+96, byofs+157, "Cancel");
		FilledBox(bxofs+20, byofs+15, 80, 135, black);
		FilledBox(bxofs+100, byofs+15, 10, 135, 2);
		HLine(bxofs+20, byofs+15, 90, darkw);
		VLine(bxofs+20, byofs+15, 135, darkw);
		VLine(bxofs+99, byofs+16, 134, white);
		VLine(bxofs+109, byofs+16, 134, white);
		HLine(bxofs+21, byofs+149, 89, white);
		stdwindow(bxofs+100, byofs+16, bxofs+109, byofs+25);
		stdwindow(bxofs+100, byofs+140, bxofs+109, byofs+149);
		b=byofs+24+(entries->count != 22 ? ((117-a)*ofs)/(entries->count-22) : 0);

		//draws the drag box
		stdwindow(bxofs+100, b, bxofs+109, b+a);

		FilledBox(bxofs+21, byofs+((cfs-ofs)*6)+16, 78, 6, darkw);

		// Render actual filename thingies
		i=0;
		while (1) {
			if (i+ofs<entries->count && i<22) {
				GotoXY(bxofs+22, byofs+17+(i*6));
				entry=entries->get(i+ofs);
				if (entry!=NULL) {
					printstring(entry->name);//blah+((i+ofs)*13));
					//Log("printed i=%d ofs=%d entries->count=%d",i,ofs,entries->count);
				}
			} else
				break;
			i++;
		}

		DrawMouse();
		ShowPage();
		CheckTimerStuff();

		// input phase
		if (dragging && !mouse_b) {
			dragging=0;
		}

		if ((mouse_b==1
		&& mouse_x>bxofs+100 && mouse_x<bxofs+110
		&& mouse_y>byofs+25 && mouse_y<byofs+135)
		|| dragging)
		{
			if (mouse_y>b && mouse_y<b+a) {
				dragging=1;
			} else {
				if (mouse_y<b) {
					if (ofs<22) ofs=0;
					else ofs-=22;
					cfs=ofs;
				}
				if (mouse_y>b+a) {
					if (ofs+45>entries->count) ofs=entries->count-22;
					else ofs+=22;
					cfs=ofs;
				}
			}
		}
		if (!dragging) {
			if (mouse_b==1
			&& mouse_x>bxofs+20 && mouse_x<bxofs+100
			&& mouse_y>byofs+15 && mouse_y<byofs+149)
			{
				i=(mouse_y-byofs-16)/6;
				i+=ofs;

				entry=entries->get(i);
				if (entry==NULL) {
					WaitRelease();
					continue;
				}
				cwd_buf[0]='\0';
				newdir_buf[0]='\0';
				if (entry->name[0]=='.'&& entry->name[1]=='/') {
					if (strcmp(entry->name+2,"..")==0) {
						Log("parent dir selected...");
						int idx=strlen(cwd_prefix)-1;
						for (; idx>0 && cwd_prefix[idx]!='\0' && cwd_prefix[idx]!='/' && cwd_prefix[idx]!='\\'; --idx)
							; // empty
						if (cwd_prefix[idx]=='/'||cwd_prefix[idx]=='\\') {
							for (--idx; idx>0 && cwd_prefix[idx]!='\0' && cwd_prefix[idx]!='/' && cwd_prefix[idx]!='\\'; --idx)
								; // empty
							if (cwd_prefix[idx]=='/'||cwd_prefix[idx]=='\\') {
								cwd_prefix[idx+1]='\0';
							} else if (idx==0) {
								cwd_prefix[0]='\0';
							}
						}
					} else {
						Log("subdir selected");
						int len=strlen(cwd_prefix);
						strcat(cwd_prefix,entry->name+2);
						strcat(cwd_prefix,"/");
						//memcpy(cwd_prefix+len,entry->name+2,strlen(entry->name+2)+1);
					}
					Log("cwd_prefix: %s",cwd_prefix);
					// free old entries
					entries->free();
					delete entries;
					// get new entries
					entries=get_entries(n,m1,m2,m3,m4);
					ofs=0;
					WaitRelease();
					continue;
				} else {
					memcpy(dest,entry->name,strlen(entry->name)+1);
					//memcpy(dest,blah+(i*13),13);
					done=1;
				}
				break;
			}
			if (mouse_b==1
			&& mouse_x>bxofs+100 && mouse_x<bxofs+110
			&& mouse_y>byofs+15 && mouse_y<byofs+25)
			{
				WaitRelease();
				key[KEY_UP]=1;
				last_pressed=KEY_UP<<8;
			}
			if (mouse_b==1
			&& mouse_x>bxofs+100 && mouse_x<bxofs+110
			&& mouse_y>byofs+139 && mouse_y<byofs+149)
			{
				WaitRelease();
				key[KEY_DOWN]=1;
				last_pressed=KEY_DOWN<<8;
			}
			if (mouse_b==1
			&& mouse_x>bxofs+65 && mouse_x<bxofs+30+65
			&& mouse_y>byofs+157 && mouse_y<byofs+10+157
			&& !moving)
			{
				Log("here");
				entry=entries->get(0);
				memcpy(dest, entry->name, strlen(entry->name)+1);
				//memcpy(dest, blah, 13);
				done=1;
				ButtonPressed(bxofs+65, byofs+157, "OK");
				DrawMouse();
				while (mouse_b) {
					ReadMouse();
					ShowPage();
					ReadMouse();
				}
				WaitRelease();
				continue;
			}

			if (mouse_b==1
			&& mouse_x>bxofs+96 && mouse_x<bxofs+30+96
			&& mouse_y>byofs+157 && mouse_y<byofs+10+157
			&& !moving)
			{
				*dest=0; done=1;
				ButtonPressed(bxofs+96, byofs+157, "Cancel");
				DrawMouse();
				while (mouse_b) {
					ReadMouse();
					ShowPage();
					ReadMouse();
				}
				WaitRelease();
				continue;
			}

			if (mouse_b==1
			&& mouse_x>(bxofs+bxsize-9) && (mouse_x<bxofs+bxsize-2)
			&& mouse_y>(byofs+1) && (mouse_y<byofs+8)
			&& !moving)
			{
				Log("done A");
				done=1;
				WaitRelease();
				break;
			}

			if (mouse_b==1
			&& mouse_x>bxofs && mouse_x<bxofs+bxsize
			&& mouse_y>byofs && mouse_y<(byofs+9)
			&& !moving)
			{
				moving=1;
				mxo=mouse_x-bxofs;
				myo=mouse_y-byofs;
				continue;
			}

			if (mouse_b==1 && !moving) {
				ctf=0;
				cb=0;
				str=0;
			}

			if (last_pressed) {
				//if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				//	c=key_shift_tbl[last_pressed];
				//else
				//	c=key_ascii_tbl[last_pressed];
				c=(char)(last_pressed & 0xff);

				if ((last_pressed>>8)==KEY_ESC) {
					key[KEY_ESC]=0;
					done=1;
					break;
				}
				if (key[KEY_PGUP]) {
					if (ofs<22) ofs=0;
					else ofs-=22;
					cfs=ofs;
					key[KEY_PGUP]=0;
					continue;
				}
				if (key[KEY_PGDN]) {
					if (ofs+45>entries->count) ofs=entries->count-22;
					else ofs+=22;
					if (entries->count<22) ofs=0;
					cfs=ofs;
					key[KEY_PGDN]=0;
					continue;
				}
				if (key[KEY_UP]) {
					if (cfs) cfs--;
					if (ofs>cfs) ofs--;
					key[KEY_UP]=0;
					last_pressed=0;
					continue;
				}
				if (key[KEY_DOWN]) {
					if (cfs<entries->count-1) cfs++;
					if (cfs-ofs>21) ofs++;
					key[KEY_DOWN]=0;
					last_pressed=0;
					continue;
				}
				if ((last_pressed>>8)==KEY_ENTER) {
					//memcpy(dest,blah+(cfs*13),13);
					entry=entries->get(cfs);
					cwd_buf[0]='\0';
					newdir_buf[0]='\0';
					if (entry->name[0]=='.'&& entry->name[1]=='/') {
						if (strcmp(entry->name+2,"..")==0) {
							Log("parent dir selected...");
							int idx=strlen(cwd_prefix)-1;
							for (; idx>0 && cwd_prefix[idx]!='\0' && cwd_prefix[idx]!='/' && cwd_prefix[idx]!='\\'; --idx)
								; // empty
							if (cwd_prefix[idx]=='/'||cwd_prefix[idx]=='\\') {
								for (--idx; idx>0 && cwd_prefix[idx]!='\0' && cwd_prefix[idx]!='/' && cwd_prefix[idx]!='\\'; --idx)
									; // empty
								if (cwd_prefix[idx]=='/'||cwd_prefix[idx]=='\\') {
									cwd_prefix[idx+1]='\0';
								} else if (idx==0) {
									cwd_prefix[0]='\0';
								}
							}
						} else {
							//Log("subdir selected");
							int len=strlen(cwd_prefix);
							memcpy(cwd_prefix+len,entry->name+2,strlen(entry->name+2)+1);
						}
						//Log("cwd_prefix: %s",cwd_prefix);
						// free old entries
						entries->free();
						delete entries;
						// get new entries
						entries=get_entries(n,m1,m2,m3,m4);
						ofs=0;
						cfs=0;
						WaitRelease();
						continue;
					} else {
						memcpy(dest,entry->name,strlen(entry->name)+1);
						//memcpy(dest,blah+(i*13),13);
						done=1;
					}

					key[KEY_ENTER]=0;
					//done=1;
					ctf=0;
					str=0;
					last_pressed=0;
					break;
				}

				if ((last_pressed>>8)==KEY_TAB) {
					switch (ctf) {
						case 0: {
							ctf=0;
							str=0;
							break;
						}
					}
					key[KEY_TAB]=0;
					last_pressed=0;
					cb=1;
					cursorblink=systemtime+40;
					continue;
				}

				if (!ctf)
					continue;

				if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str)) {
					str[strlen(str)-1]=0;
					key[KEY_BACKSPACE]=0;
					last_pressed=0;
					continue;
				} else if ((last_pressed>>8)==KEY_BACKSPACE) {
					last_pressed=0;
					continue;
				}

				t=strlen(str);
				str[t]=(char)c;
				str[t+1]=0;
				last_pressed=0;
			}
		}
	} while (!done);

	entries->free();
	delete entries;

	vfree(blah);
}