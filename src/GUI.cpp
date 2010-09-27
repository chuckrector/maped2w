// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                            MapEd v.2.01                             ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                  Graphical User Interface module                    ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <aen, apr 21>
// + added lots of prototypes to GUI.H and made a few things static
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>

#include "gui.h"
//#include "cdrom.h"
#include "config.h"
#include "guicomp.h"
#include "maped.h"
#include "mouse.h"
#include "render.h"
#include "timer.h"
#include "vdriver.h"
#include "keyboard.h"

#include "smalfont.h"
//#include "columns.h"
#include "tilesel.h"
#include "pcx.h"
#include "log.h"

#include "controls.h"

// ================================= Data ====================================

#define MESSAGE_MAX_LENGTH 1019
struct message
{
  char text[MESSAGE_MAX_LENGTH + 1];                     // message text
  int exptime;                       // message expire time
};

struct message messages[6];          // 5-message buffer
char num_msgs=0;                     // number of active messages
char playingmod[80];                 // currently playing mod.
extern char scrollmode;

// ================================= Code ====================================

static void RemoveMessage(int index) {
	for (; index<num_msgs && index<5; index++) {
		memcpy(messages[index].text,messages[index+1].text,MESSAGE_MAX_LENGTH+1);
		messages[index].exptime=messages[index+1].exptime;
	}
	--num_msgs;
}

void CheckMessageExpirations() {
	for (int index=0; index<num_msgs; index++) {
		if ((int)systemtime>messages[index].exptime) {
			RemoveMessage(index);
		}
	}
}

void Message(char *text, int duration) {
	if (num_msgs<5) {
		int len=strlen(text);
		if (len>MESSAGE_MAX_LENGTH) len=MESSAGE_MAX_LENGTH;
		memcpy(messages[num_msgs].text,text,len);
		messages[num_msgs].text[len]='\0';
		messages[num_msgs].exptime=systemtime+duration;
		++num_msgs;
	} else {
		RemoveMessage(0);
		Message(text,duration);
	}
}

void DrawScreenControls() {

	//Log("DrawScreenControls()::P");
	if (el<6 && pasting)
		printstring(200-5,sy-6,"Paste Mode");

	//Log("DrawScreenControls()::current-tiles");
  // Put current-tile boxes if applicable.
	if (el<6) {
		int lx= sx - 16 - 2 - 16 - 2 - 1;
		int y = sy - 16 - 2;
		int rx= lx + 16 + 2 + 1;
		FilledBox(lx - 1, y - 1, 18, 18, white);
		FilledBox(rx - 1, y - 1, 18, 18, white);
		if (lt<0 || lt>=numtiles) lt=0;
		if (rt<0 || rt>=numtiles) rt=0;
		CopySprite(lx, y, 16, 16, vsp[lt]);//(char*)vsp + 256*lt);
		CopySprite(rx, y, 16, 16, vsp[rt]);//(char*)vsp + 256*rt);
	}

	//Log("DrawScreenControls()::zones");
	if (el==7) {
		int max_width=0;
		char buffer[1024];
		sprintf(buffer,"Zone %d",curzone);
		int pix_a=pixels(buffer);
		max_width=pix_a;
		int pix_b=pixels(zones[curzone].name);
		if (pix_b>max_width) max_width=pix_b;
		FilledBox(sx-max_width-1,sy-13,max_width+1,13,makecol(0,0,0));
		VLine(sx-max_width-2,sy-13,sy,makecol(255,255,255));
		HLine(sx-max_width-2,sy-14,sx,makecol(255,255,255));
		printstring(sx-pix_a,sy-12,"Zone %d",curzone);
		printstring(sx-pix_b,sy-6,zones[curzone].name);

		char c=zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+mouse_y)/16) *
				layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mouse_x)/16)];\
				//Log("DrawScreenControls()::zones::current-zone");
		if (c) {
			sprintf(buffer,"Zone %d",c);
			pix_a=pixels(buffer);
			max_width=pix_a;
			//printstring(sx-pix,1,"Zone %d",c);
			pix_b=pixels(zones[c].name);
			if (pix_b>max_width) max_width=pix_b;
			//printstring(sx-pix,7,zones[c].name);
			FilledBox(sx-max_width-1,0,max_width+1,13,makecol(0,0,0));
			VLine(sx-max_width-2,0,13,makecol(255,255,255));
			HLine(sx-max_width-2,13,sx,makecol(255,255,255));
			printstring(sx-pix_a,1,"Zone %d",c);
			printstring(sx-pix_b,7,zones[c].name);
		}
  }

	//Log("DrawScreenControls()::entities");
	if (el==8) {
		char buffer[1024];
		sprintf(buffer,"%d entities",entities);
		int pix=pixels(buffer);
		printstring(sx-pix,1,"%d entities",entities);
		char c=(unsigned char)EntityThere((mouse_x+xwin)/16,(mouse_y+ywin)/16);
		if (c) {
			//Log("DrawScreenControls()::entities::current-entity");
			pix=pixels(entity[c-1].desc);
			printstring(sx-pix,sy-10,entity[c-1].desc);
		}
	}

	//Log("DrawScreenControls()::current-tile-mouse-thingy");
	// Put the current-tile mouse thingy.
	char tmp_buffer[1024];
	//Log("DrawScreenControls()::sprintf");
	int tx=(xwin+mouse_x)/16;
	int ty=(ywin+mouse_y)/16;
	if (tx<layer[0].sizex && ty<layer[0].sizey) {
		sprintf(tmp_buffer,"%d, %d",(xwin+mouse_x)/16,(ywin+mouse_y)/16);
		//Log("DrawScreenControls()::tmp_pix");
		int tmp_pix=pixels(tmp_buffer);
		//Log("DrawScreenControls()::FilledBox");
		FilledBox(0,0,tmp_pix+1,7,makecol(0,0,0));
		//Log("DrawScreenControls()::HLine");
		HLine(0,7,tmp_pix+2,makecol(255,255,255));
		//Log("DrawScreenControls()::VLine");
		VLine(tmp_pix+1,0,7,makecol(255,255,255));
		//Log("DrawScreenControls()::printstring");
		printstring(1,1,tmp_buffer);
	}
	//printstring(1,10,"xwin %d ywin %d", xwin, ywin);

	//Log("DrawScreenControls()::layer-editor-selector");
	// Put the layer editor/selector.
	for (int index=0; index<6; index++) {
		char c;
		if (el==index) c=white; else c=darkw;
		FilledBox(1+(index*10),sy-7,9,7,c); //tsy-7,9,7,c);
		if (layertoggle[index]) c=darkred; else c=0;
		FilledBox(2+(index*10),sy-6,7,6,c); //tsy-6,7,6,c);
		if (index<numlayers)
			printstring(4+(index*10),sy-5,"%d",index+1);
	}

	//Log("DrawScreenControls()::layers-zones-obs");
	for (int index=6; index<10; index++) {
		char c;
		if (el==index) c=white; else c=darkw;
		FilledBox(1+(index*10),sy-7,9,7,c); // tsy-7
		if (layertoggle[index]) c=darkred; else c=0;
		FilledBox(2+(index*10),sy-6,7,6,c); // tsy-6
		GotoXY(4+(index*10),sy-5); //tsy-5);
		switch (index) {
			case 6: printstring("O"); break;
			case 7: printstring("N"); break;
			case 8: printstring("E"); break;
			case 9: printstring("T"); break;
		}
	}
}

void RenderMessages() {
	//Log("RenderMessages()::CheckMessageExpirations()");
	CheckMessageExpirations();
	//Log("RenderMessages()::loop");
	for (int index=0; index<num_msgs; index++)
		printstring(1,9+(index*6),messages[index].text);
}

void RenderGUI() {
	//Log("RenderMessages()");
	RenderMessages();
	//Log("DrawScreenControls()");
	DrawScreenControls();
	//printstring(100,10,"Undo: %d",undo::UndoCount());
	//Log("DrawMouse()");
	DrawMouse();
}

// ============================= Dialog Code =================================

int mmxofs=60, mmyofs=50;

void DoMainMenu()
{
  key[KEY_ESC]=0;
  //MainMenu(mmxofs, mmyofs);
  newMainMenu(mmxofs,mmyofs);
}

struct GUI_Component {
	int x, y;
	int w, h;
	static int pressed;
	GUI_Component* parent;
	GUI_Component* prev;
	GUI_Component* next;
	GUI_Component(int x, int y, int w, int h)
		: x(x), y(y), w(w), h(h), parent(0), prev(0), next(0) {
			//keep_onscreen();
		}
	virtual ~GUI_Component() {}
	virtual void draw(int x, int y) = 0;
	virtual int process() = 0;
	void set_parent(GUI_Component* parent) {
		this->parent = parent;
	}
	int mouse_over() {
		int testx = mouse_x;
		int testy = mouse_y;
	// if we have a parent, test relative to the parent's upper-left
		if (parent) {
			return (testx >= parent->x + x && testx < parent->x + x + w
				&&	testy >= parent->y + y && testy < parent->y + y + h);
		}
		return (testx >= x && testx < x + w && testy >= y && testy < y + h);
	}
	void keep_onscreen() {
		if (x + w > sx) x = sx - w;
		if (y + h > sy) y = sy - h;
		if (x < 0) x = 0;
		if (y < 0) y = 0;
	}
	void move(int xdelta, int ydelta) {
		x += xdelta;
		y += ydelta;
		//keep_onscreen();
	}
	virtual char* name() = 0;
	void handle_mouse() {
		if (!pressed && mouse_over() && mouse_b) {
			pressed = 1;
		} else if (pressed && mouse_b) {
			pressed++;
		} else if (!mouse_b) {
			pressed = 0;
		}
	}
	int get_mb() { return (1 == pressed); }
	int is_pressed() { return pressed; }
};
int GUI_Component::pressed = 0;

struct GUI_ComponentList {
	GUI_Component* head;
	GUI_Component* tail;
	int count;
	GUI_ComponentList(): head(0), tail(0), count(0) {}
	void add(GUI_Component* node) {
		if (!node) return;
	// will be first node in list?
		if (!head) {
			head = node;
			head->prev = 0;
			head->next = 0;
			tail = head;
	// otherwise, additional node
		} else {
			node->prev = tail;
			node->next = 0;
			tail->next = node;
			tail = node;
		}
		count++;
	}
	void remove(GUI_Component* node) {
		if (!node) return;
		GUI_Component* iter = head;
		while (iter && iter != node) {
			iter = iter->next;
		}
		if (!iter) return;
	// unlink list from object
		if (node->prev) node->prev->next = node->next; else head = node->next;
		if (node->next) node->next->prev = node->prev; else tail = node->prev;
	// unlink object from list
		node->prev = 0;
		node->next = 0;
		count--;
	}
};

struct GUI_Label: public GUI_Component {
	char* text;
	GUI_Label(int x, int y, int w, int h, char* text): GUI_Component(x, y, w, h), text(text) {}
	char* name() { return "GUI_Label"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		printstring(x,y,text);
	}
	int process() { return 0; }
};

struct GUI_MenuItem: public GUI_Label {
	int number;
	int chosen;
	GUI_MenuItem(int x, int y, int w, int h, char* text, int number)
		: GUI_Label(x, y, w, h, text), chosen(0), number(number) {}
	char* name() { return "GUI_MenuItem"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		if (mouse_over() && !is_pressed()) {
			FilledBox(x, y, w, h, 254); //darkw);
		}
		printstring(x+1,y+1,text);
	}
	int process() {
		if (mouse_over()) {
			if (get_mb()) {
				chosen = 1;
			}
		}
		return 0;
	}
	int is_chosen() { return chosen; }
};

struct GUI_Button: public GUI_Label {
	int down;
	GUI_Button(int x, int y, int w, int h, char* text): GUI_Label(x, y, w, h, text), down(0) {}
	char* name() { return "GUI_Button"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		if (down) {
			ButtonPressedSize(x, y, w, h, text);
		} else {
			ButtonSize(x, y, w, h, text);
		}
	}
	int process() {
		if (!down && mouse_over() && get_mb()) {
			down = 1;
		} else if (!is_pressed()) {
			down = 0;
		}
		return 0;
	}
};

struct GUI_TextField: public GUI_Component {
	char text[1024];
	int counter;
	int first;
	int cursor;
	int left;
	int selecting;
	int select_start;
	int select_end;
	GUI_TextField(int x, int y, int w, int h, char* text)
		: GUI_Component(x, y, w, h), counter(0), first(0), left(0), cursor(0)
		, selecting(0), select_start(0), select_end(0) {
			if (!text) text = "";
			set_text(text);
		}
	char* name() { return "GUI_TextField"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		TextFieldCursorSel(w, x, y, w,
			text + left, cursor, first ? counter%20 < 10 : 0, select_start, select_end);
		printstring(32,32,"%2d %2d",select_start,select_end);
	}
	int view_pixels() {
		return pixels(text + left);
	}
	void move_cursor(int by) {
		cursor += by;
		if (cursor < 0) cursor = 0;
		if (cursor > 1023) cursor = 1023;
		if (cursor > (int)strlen(text)) cursor = strlen(text);
	}
	int process() {
		counter++;
		if (counter >= 20) {
			counter = 0;
		}
		if (!first) {
			if (mouse_over() && get_mb()) {
				first = 1;
			} else {
				return 0;
			}
		} else {
			if (!mouse_over() && is_pressed()) {
				first = 0;
				return 0;
			}
		}

		/*
		int shifted = 0;
		if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
			shifted = 1;
		}
		if (!shifted) {
			if (key[KEY_LEFT] + key[KEY_RIGHT] + key[KEY_HOME] + key[KEY_END] > 0) {
				if (!selecting) {
					selecting = 1;
					select_start = cursor;
				} else {
					select_end = cursor;
				}
			}
		} else {
			//if (key[KEY_LEFT] + key[KEY_RIGHT] + key[KEY_HOME] + key[KEY_END] > 0) {
				selecting = 0;
				select_start = 0;
				select_end = 0;
			//}
		}
		*/

		if (keypressed()) {
			int ch = 0;
			int r = readkey();
			FILE* fp;
			fopen_s(&fp, "crap.txt", "a");
			fprintf(fp, "r=%d\n", r);
			fclose(fp);
			int shifted = 0;
			//if ((key_shifts & KB_SHIFT_FLAG)) { // || key_table[KEY_RSHIFT]) {
			//	shifted = 1;
			//	ch = (char)(r&0xff);//key_shift_tbl[r];
			//} else {
			//	ch = (char)(r&0xff);//key_ascii_tbl[r];
			//}
			ch=(char)r&0xff;

			if (KEY_LEFT == r) {
				if (shifted && !selecting) {
					selecting = 1;
					select_start = cursor;
				}
				move_cursor(-1);
				if (shifted && selecting) {
					select_end = cursor;
				} else {
					selecting = 0;
					select_start = 0;
					select_end = 0;
				}
			} else if (KEY_RIGHT == r) {
				if (shifted && !selecting) {
					selecting = 1;
					select_start = cursor;
				}
				move_cursor(+1);
				if (shifted && selecting) {
					select_end = cursor;
				} else {
					selecting = 0;
					select_start = 0;
					select_end = 0;
				}
			} else if (KEY_HOME == r) {
				if (shifted && !selecting) {
					selecting = 1;
					select_start = cursor;
				}
				cursor = 0;
				if (shifted && selecting) {
					select_end = cursor;
				} else {
					selecting = 0;
					select_start = 0;
					select_end = 0;
				}
			} else if (KEY_END == r) {
				if (shifted && !selecting) {
					selecting = 1;
					select_start = cursor;
				}
				cursor = strlen(text);
				if (shifted && selecting) {
					select_end = cursor;
				} else {
					selecting = 0;
					select_start = 0;
					select_end = 0;
				}
			} else {
				if (KEY_BACKSPACE == r || KEY_DEL == r) {
					int temp = 0;
					if (select_start > select_end) {
						temp = select_start;
						select_start = select_end;
						select_end = temp;
					}
				// if there's a selection, zap that
					if (select_end - select_start > 0) {
						int w = select_end - select_start;
						memmove(text + select_start, text + select_end, strlen(text) - select_end);
						int R = 1024 - (strlen(text) - w);
						memset(text + strlen(text) - w, 0, R);
						cursor = select_start;
						selecting = 0;
						select_start = select_end = 0;
					} else {
						delete_char(KEY_BACKSPACE == r ? 1 : 0);
					}
				} else {
					if (!ch) return 0;
					insert_char(ch);
				}
			}
		// unpress whatever was pressed
			//key_table[r] = 0;
			key[r>>8]=0;
		}
		return 0;
	}
	void set_text(char* text) {
		strncpy(this->text, text, 1023);
		this->text[1023] = 0;
		cursor = strlen(this->text);
	}
	void insert_char(int ch) {
		int L = strlen(text);
		if (L > 1023) L = 1023;
		int n;
		for (n = L; n > cursor; n--) {
			text[n] = text[n - 1];
		}
		text[n] = (char)ch;
		if (n >= L) {
			text[n + 1] = 0;
		}
		move_cursor(+1);
	}
// zap the last character, if any
	void delete_char(int backsp) {
		int L = strlen(text) - 1;
		if (L < 0) L = 0;
		if (L > 1023) {
			L = 1023;
		}
		if (cursor < 1 && backsp) return;
		move_cursor(-backsp);
		int n = cursor;
		for (; n < L; n++) {
			text[n] = text[n + 1];
		}
		text[n] = 0;
	}
};

struct GUI_CheckBox: public GUI_Component {
	int checked;
	GUI_CheckBox(int x, int y): GUI_Component(x, y, 0, 0), checked(0) {}
	char* name() { return "GUI_CheckBox"; }
	void toggle() {
		checked = 1 - checked;
	}
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		CheckBox(x, y, checked);
	}
	int process() {
		if (mouse_over() && get_mb()) {
			toggle();
		}
		return 0;
	}
};

struct GUI_Divider: public GUI_Component {
	GUI_Divider(int x, int y, int w): GUI_Component(x, y, w, 1) {}
	char* name() { return "GUI_Divider"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		HLine(x, y, w, white);
	}
	int process() { return 0; }
};

struct GUI_Grip: public GUI_Component {
	GUI_Grip(int x, int y, int w, int h): GUI_Component(x, y, w, h) {}
	char* name() { return "GUI_Grip"; }
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		ColorGridSize(x, y, w, h, 20);
	}
	int process() {
		static int anchor_x = 0;
		static int anchor_y = 0;
		static int first = 0;
		if (parent) {
			if (!first) {
				if (mouse_over() && get_mb()) {
					anchor_x = mouse_x - parent->x;
					anchor_y = mouse_y - parent->y;
					first = 1;
				}
			} else {
				if (is_pressed()) {
					parent->x = mouse_x - anchor_x;
					parent->y = mouse_y - anchor_y;
				} else {
					first = 0;
				}
			}

			if (parent->x < 0) parent->x = 0;
			if (parent->y <	0) parent->y = 0;
			if (parent->x + parent->w > sx) parent->x = sx - parent->w;
			if (parent->y + parent->h > sy) parent->y = sy - parent->h;
		}
		return 0;
	}
};

/**
 * A panel is responsible for the memory of the component objects which it holds.
 */
struct GUI_Panel: public GUI_Component {
	GUI_ComponentList list;
	GUI_Component* focus;	// this guy gets processed
	GUI_Panel(int x, int y, int w, int h): GUI_Component(x, y, w, h), focus(0) {}
	~GUI_Panel() {
		GUI_Component* iter = list.head;
		while (iter) {
			GUI_Component* after = iter->next;
			delete iter;
			iter = after;
		}
	}
	char* name() { return "GUI_Panel"; }
	void draw() {
		draw(0, 0);
	}
	void draw(int x, int y) {
		x += this->x;
		y += this->y;
		stdwindow(x, y, x + w - 1, y + h - 1);
	// draw all components in the panel
		GUI_Component* iter = list.head;
		while (iter) {
			iter->draw(x, y);
			iter = iter->next;
		}
	}
	int process() {
		GUI_Component* iter = list.head;
		focus = 0; // no focus until we determine otherwise
		while (iter) {
			if (iter->mouse_over()) {
				focus = iter;
			}
			iter->handle_mouse();
			iter->process();
			iter = iter->next;
		}
//		if (focus) {
//			focus->process();
//		}
	// something was chosen?
		if (get_chosen()) {
			return 1;
		}
		return 0;
	}
	void add(GUI_Component* node) {
		node->set_parent(this);
		list.add(node);
	}
	void remove(GUI_Component* node) {
		list.remove(node);
		node->set_parent(0);
	}
	GUI_MenuItem* get_chosen() {
		GUI_Component* iter = list.head;
		while (iter) {
			if (!strcmp(iter->name(), "GUI_MenuItem")) {
				GUI_MenuItem* item = (GUI_MenuItem*)iter;
                if (item->is_chosen())
                	return item;
			}
			iter = iter->next;
		}
		return (GUI_MenuItem*)0;
	}
};

extern void Columns();

void newMainMenu(int x, int y) {
// compose the GUI
	GUI_Panel* panel = new GUI_Panel(x, y, 79, 139-30-(7*3));//);
	panel->x=sx/2-panel->w/2; // center it
	panel->y=sy/2-panel->h/2;
	y = 2;
	panel->add(new GUI_Grip(2, y, 75, 6)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "New MAP",			 1)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Load MAP",		 2)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Save MAP",		 3)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Load VSP",		 4)); y += 7+1;
	panel->add(new GUI_Divider (2, y, 75)); y += 2;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "MAP Properties",	 5)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Layers",			 6)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Options",			 7)); y += 7;
	//panel->add(new GUI_MenuItem(2, y, 75, 7, "MOD Player",		 8)); y += 7;
	//panel->add(new GUI_MenuItem(2, y, 75, 7, "CD Player",		 9)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Columns",			10)); y += 7;
	//panel->add(new GUI_MenuItem(2, y, 75, 7, "Execute",			11)); y += 7;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "Exit",		12)); y += 7+1;
	panel->add(new GUI_Divider (2, y, 75)); y += 2;
	panel->add(new GUI_MenuItem(2, y, 75, 7, "About MapEd",		13)); y += 7+1;
	//panel->add(new GUI_TextField(2+1, y, 75-2, 9, "Empty.")); y += 10;
	//panel->add(new GUI_Button(2+1, y, 75-2, 20-1, "Option")); y += 20;

// user interaction w/ GUI
	int done = 0;
	key[KEY_ESC] = 0;
	while (!panel->process()) {
		RenderMap();
		RenderGUI();
		panel->draw();
		DrawMouse();
		ShowPage();
		CheckTimerStuff();
		if (key[KEY_ESC]) {
			key[KEY_ESC]=0;
			break;
		}
	}

	printf("done");
// act upon chosen menu item
	GUI_MenuItem* item = panel->get_chosen();
	int chosen = 0;
	if (item) chosen = item->number;

// done with this dude
	delete panel;

	switch (chosen) {
//		FILE* fp = fopen("log.log", "a");
		case 1: WaitRelease(); NewMAP(); break;
		case 2: WaitRelease(); LoadMAPDialog(); break;
		case 3: WaitRelease(); key[KEY_F10]=1; break;
		case 4: WaitRelease(); LoadVSPDialog(); break;
		case 5: WaitRelease(); MapPropertiesDialog(); break;
		case 6: WaitRelease(); LayersSubMenu(x, y); break;
		case 7: WaitRelease(); OptionsDialog(); break;
		//case 8: MPDialog(); break;
		//case 9: CDDialog(); break;
		case 10:WaitRelease(); Columns(); break;
		//case 11:ExecuteSubMenu(0,0); break;
		case 12:WaitRelease(); key[KEY_X]=1; key[KEY_ALT]=1; break;
		case 13:WaitRelease(); About(); break;
//		fprintf(fp, ">>> Chose '%s' <<<\n", item->text);
//		fclose(fp);
	}

	WaitRelease();
}

void MainMenu(int dx, int dy)
{ int done=0;
  //int mxo, myo, cursorblink;

  mmxofs=dx;
  mmyofs=dy;
  do
  {
    if (dx<0) dx=0;
    if (dy<0) dy=0;
    if (dx+80>sx) dx=sx-80;
    if (dy+89>sy) dy=sy-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+89);

    if (mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (mouse_y>dy+8 && mouse_y<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (mouse_y>dy+14 && mouse_y<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
       if (mouse_y>dy+20 && mouse_y<dy+26) FilledBox(dx+2, dy+20, 76, 7, darkw);
       if (mouse_y>dy+29 && mouse_y<dy+36) FilledBox(dx+2, dy+29, 76, 7, darkw);
       if (mouse_y>dy+35 && mouse_y<dy+42) FilledBox(dx+2, dy+35, 76, 7, darkw);
       if (mouse_y>dy+41 && mouse_y<dy+48) FilledBox(dx+2, dy+41, 76, 7, darkw);
       if (mouse_y>dy+47 && mouse_y<dy+54) FilledBox(dx+2, dy+47, 76, 7, darkw);
       if (mouse_y>dy+53 && mouse_y<dy+60) FilledBox(dx+2, dy+53, 76, 7, darkw);
       if (mouse_y>dy+59 && mouse_y<dy+66) FilledBox(dx+2, dy+59, 76, 7, darkw);
       if (mouse_y>dy+65 && mouse_y<dy+72) FilledBox(dx+2, dy+65, 76, 7, darkw);
       if (mouse_y>dy+71 && mouse_y<dy+78) FilledBox(dx+2, dy+71, 76, 7, darkw);
       if (mouse_y>dy+80 && mouse_y<dy+87) FilledBox(dx+2, dy+80, 76, 7, darkw);
    }

    printstring(dx+3,dy+3,"New MAP");
    printstring(dx+3,dy+9,"Load MAP");
    printstring(dx+3,dy+15,"Save MAP");
    printstring(dx+3,dy+21,"Load VSP");
    HLine(dx+2, dy+28, 76, white);
    printstring(dx+3,dy+30,"MAP Properties");
    printstring(dx+3,dy+36,"Layers"); printstring(dx+73,dy+36,">");
    printstring(dx+3,dy+42,"Options");
    printstring(dx+3,dy+48,"Columns");
    printstring(dx+3,dy+54,"Exit");
    HLine(dx+2, dy+60, 76, white);
    printstring(dx+3,dy+66,"About MapEd");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mouse_b==1 && mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9) { NewMAP(); done=1; WaitRelease(); break; }
       if (mouse_y>dy+8 && mouse_y<dy+15) { done=1; WaitRelease(); LoadMAPDialog(); break; }
       if (mouse_y>dy+14 && mouse_y<dy+21) { done=1; WaitRelease(); key[KEY_F10]=1; break; }
       if (mouse_y>dy+20 && mouse_y<dy+26) { done=1; WaitRelease(); LoadVSPDialog(); break; }
       if (mouse_y>dy+29 && mouse_y<dy+36) { done=1; WaitRelease(); MapPropertiesDialog(); break; }
       if (mouse_y>dy+35 && mouse_y<dy+42) { done=1; WaitRelease(); LayersSubMenu(dx,dy); break; }
       if (mouse_y>dy+41 && mouse_y<dy+48) { done=1; WaitRelease(); OptionsDialog(); break; }
       if (mouse_y>dy+47 && mouse_y<dy+54) { done=1; WaitRelease(); Columns(); break; }
       if (mouse_y>dy+53 && mouse_y<dy+60) { done=1; WaitRelease(); key[KEY_X]=1; key[KEY_ALT]=1; break; }
       if (mouse_y>dy+59 && mouse_y<dy+66) { done=1; About(); break; }
    }

    if (mouse_b==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[KEY_ESC]==1) done=1;
  } while (!done);
  key[KEY_ESC]=0;
}

// tilesel.c
//extern FILE *pcxf;
//extern int width, depth;
//extern int vidoffset;
//extern BITMAP **ovsp;
//extern char *t;
unsigned short ontiles; // HACK

static int TileMatch(int a, int b)
  {
	  return TileCmp(ovsp[a],ovsp[b]);
//!memcmp(ovsp+(a*256), ovsp+(b*256), 256);
  }

int ilxsize=145, ilysize=122+10;
int ilxofs=16, ilyofs=16;

// make this configurable at some point
#define MIN_TILE_X 50
#define MIN_TILE_Y 50

static int BitToNum(int num)
{
  int i;

  if (!num)
    return 0;

  for (i=0; i<32; i++)
  {
    if (num & (1<<i))
      break;
  }

  return i+1;
}

//zero
unsigned char TileCmp(BITMAP *one, BITMAP *two)
  {
	  for (int y=0; y<16; y++)
		  for (int x=0; x<16; x++)
			  if (getpixel(one,x,y) != getpixel(two,x,y))
				  return false;
	return true;
	  //return !memcmp(one,two, 256);
  }

void ImageToLayer()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, tt;
  char *str;
  char nstr1[80], nstr2[80];

  int i,j,k,m;
  FILE *f=0;
  errno_t error;
  int num_padx=0,num_pady=0; // in tiles
  int minpadx=0,minpady=0; // in tiles
  int tilesx=0,tilesy=0; // in tiles
  int totalx=0,totaly=0; // in tiles
  int excessx=0,excessy=0; // in pixels
  //int chk_1=1,chk_2=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0; // radio control
  int chk_layer=1;
  int chk_grabexcess=0,chk_centerinpad=0,chk_nodup=1; //,chk_padisblank=1;
  char txt_imagename[80];
        // ...
        int ly=0;
        int wide=0,deep=0;
        int lsz=0;
        int matched=0,plot=0; //,mapat=0;
        int tilesripped=0;
        int startx=0,starty=0;
  memset(txt_imagename,0,80);
  bmode=0; // for browser; update map/gui
  //width=0; depth=0; // for pcx loading code
  PCX* pcx=new PCX();


  sprintf(nstr1, "%d", num_padx);
  sprintf(nstr2, "%d", num_pady);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      ilxofs = (mouse_x-mxo);
      ilyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (ilxofs<0) ilxofs = 0;
      if (ilyofs<0) ilyofs = 0;
      if (ilxofs+ilxsize>sx) ilxofs = sx-ilxsize;
      if (ilyofs+ilysize>sy) ilyofs = sy-ilysize;
    }

    Window(ilxofs, ilyofs, ilxofs+ilxsize, ilyofs+ilysize, "Import Image As Layer");
    Button(ilxofs+109, ilyofs+12, "Browse");
    Button(ilxofs+109, ilyofs+30, "OK");
    GotoXY(ilxofs+5, ilyofs+25); printstring("Layer:");
    // layer select radio buttons
    for (i=0; i<6; i++)
    {
      CheckBox(ilxofs+29+(i*10), ilyofs+33, (char)(chk_layer & (1<<i)));
      GotoXY(ilxofs+31+(i*10), ilyofs+25);
      sprintf(strbuf, "%i", i+1); printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+14); printstring("Image:");
    //TextField(ilxofs+29, ilyofs+12, 75, txt_imagename, ctf == 1 ? cb : 0);
    GotoXY(ilxofs+29, ilyofs+14); printstring(txt_imagename);
    HLine(ilxofs+1, ilyofs+44, 143, white);
    GotoXY(ilxofs+5, ilyofs+49); printstring("Image Dimensions:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+5+pixels("Image Dimensions:")+3, ilyofs+49);
      sprintf(strbuf, "%dx%d", pcx->width, pcx->height);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+60); printstring("Tiles X:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+5+pixels("Tiles X:")+3, ilyofs+60);
      sprintf(strbuf, "%d", tilesx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+71); printstring("Tiles Y:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+5+pixels("Tiles Y:")+3, ilyofs+71);
      sprintf(strbuf, "%d", tilesy);
      printstring(strbuf);
    }
    GotoXY(ilxofs+51, ilyofs+60); printstring("Pad X:");
    GotoXY(ilxofs+51, ilyofs+71); printstring("Pad Y:");
    TextField(ilxofs+74, ilyofs+58, 12, nstr1, (char)(ctf == 1 ? cb : 0));
    TextField(ilxofs+74, ilyofs+69, 12, nstr2, (char)(ctf == 2 ? cb : 0));
    GotoXY(ilxofs+95, ilyofs+60); printstring("Total X:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+95+pixels("Total X:")+3, ilyofs+60);
      sprintf(strbuf, "%d", totalx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+95, ilyofs+71); printstring("Total Y:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+95+pixels("Total Y:")+3, ilyofs+71);
      sprintf(strbuf, "%d", totaly);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+81); printstring("Excess Pixels X:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+5+pixels("Excess Pixels X:")+3, ilyofs+81);
      sprintf(strbuf, "%d", excessx);
      printstring(strbuf);
    }
    GotoXY(ilxofs+5, ilyofs+91); printstring("Excess Pixels Y:");
    if (pcx->width+pcx->height)
    {
      GotoXY(ilxofs+5+pixels("Excess Pixels Y:")+3, ilyofs+91);
      sprintf(strbuf, "%d", excessy);
      printstring(strbuf);
    }
    CheckBox(ilxofs+5, ilyofs+101, (char)chk_grabexcess);
    GotoXY(ilxofs+17, ilyofs+102); printstring("Grab Excess");
    CheckBox(ilxofs+5, ilyofs+110, (char)chk_centerinpad);
    GotoXY(ilxofs+17, ilyofs+111); printstring("Center Within Padding");
    CheckBox(ilxofs+5, ilyofs+119, (char)chk_nodup);
    GotoXY(ilxofs+17, ilyofs+120); printstring("Eliminate Duplicates");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mouse_b==1 && mouse_x>ilxofs+109 && mouse_x<ilxofs+30+109 && mouse_y>ilyofs+12 && mouse_y<ilyofs+10+12 && !moving) || (key[KEY_ALT] && key[KEY_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(ilxofs+109, ilyofs+12, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1, "*.pcx", "", "", "", txt_imagename);
      key[KEY_ESC]=0; // just in case ESC exited
      error = fopen_s(&f, txt_imagename, "rb");
      if (!error) {
		  if (pcx->data)
			  delete[] pcx->data;
		  pcx->load(txt_imagename);

        if (chk_grabexcess) i=15; else i=0;
        tilesx=((pcx->width+i)&~15)/16;
        tilesy=((pcx->height+i)&~15)/16;
        excessx=pcx->width & 15;
        excessy=pcx->height & 15;
        if (tilesx<MIN_TILE_X) { minpadx=MIN_TILE_X-tilesx; num_padx=minpadx; }
        if (tilesy<MIN_TILE_Y) { minpady=MIN_TILE_Y-tilesy; num_pady=minpady; }
        sprintf(nstr1, "%d", minpadx);
        sprintf(nstr2, "%d", minpady);
        totalx=tilesx+minpadx;
        totaly=tilesy+minpady;
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>ilxofs+109 && mouse_x<ilxofs+30+109 && mouse_y>ilyofs+30 && mouse_y<ilyofs+10+30 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(ilxofs+109, ilyofs+30, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }

      if (strlen(txt_imagename))
      {
        done=1;

        ly=BitToNum(chk_layer) -1;

        startx=0;
        starty=0;
        if (chk_centerinpad)
        {
          startx=(totalx-tilesx)/2;
          starty=(totaly-tilesy)/2;
        }

		if (pcx->data)
			delete[] pcx->data;
		pcx->load(txt_imagename);

        wide=pcx->width;
        deep=pcx->height;
        if (chk_grabexcess)
        {
          // round up to the nearest multiple of 16 if grabbing excess
          wide=(pcx->width+15)&~15;
          deep=(pcx->height+15)&~15;
        }
        deep=tilesy;
        wide=tilesx;
        ontiles=(unsigned short)((wide*deep)+1); // +1 for our empty tile

        //ovsp=(char *) valloc(ontiles*256,"img2layer2",0);
		ovsp=new BITMAP*[ontiles];
		for (int index=0; index<ontiles; index++) {
			ovsp[index]=create_bitmap(16,16);
			rectfill(ovsp[index],0,0,16,16,0);
		}

        layer[ly].pmultx=1;     layer[ly].pmulty=1;
        layer[ly].pdivx=1;      layer[ly].pdivy=1;
        // totalx & totaly are the sum of tilesx/tilesy & num_padx/num_pady
        layer[ly].sizex=(unsigned short)totalx; layer[ly].sizey=(unsigned short)totaly;
        layer[ly].trans=0;      layer[ly].hline=0;

        lsz = totalx * totaly;

        if (layers[ly]) vfree(layers[ly]);
        layers[ly]=(unsigned short *) valloc((lsz*2)+4,"layer data",ly);

        tilesripped=0;
        //memset(ovsp,0,256);
        tilesripped++;

        for (i=0; i<deep; i++)
        {
          for (j=0; j<wide; j++)
          {
            GrabTileAt(pcx, (j*16), (i*16), tilesripped, 0);
            plot=tilesripped;

            // eliminate duplicates?
            if (chk_nodup)
            {
              matched=0;
              for (k=0; k<tilesripped; k++)
              {
                if (TileMatch(tilesripped, k))
                {
                  matched=1;
                  plot=k;
                  break;
                }
              }
                //begin zero
              for(m=0; m<numtiles; m++)
              {
				  /*(unsigned char*)(vsp+m*256)*/
				  /*(unsigned char*)(ovsp+tilesripped*256))*/
                if(TileCmp(vsp[m],ovsp[tilesripped]))
                {
                  matched=1;
                  plot=m-numtiles;
//                  plot=0;
                  break;
                }
              }
              //end zero!
              if (!matched) tilesripped++;
            }
            // grabbing all; keep it regardless
            else tilesripped++;

            layers[ly][((i+starty)*totalx)+j+startx]=(unsigned short)plot;
          }
        }

        // adjust indices past current tile amount
        for (i=0; i<(totalx*totaly); i++)
          layers[ly][i]+=numtiles;

        if (!ly)
        {
          if (obstruct) vfree(obstruct);
          obstruct=(char *) valloc(lsz+4,"obstruct",0);
          if (zone) vfree(zone);
          zone=(char *) valloc(lsz+4,"zone", 0);
        }

        // reallocate tilebay and patch in ovsp
        //str=(char *)valloc((numtiles+tilesripped)*256,"vsp data",0);
		BITMAP** tmp_bmp = new BITMAP*[numtiles+tilesripped];
		for (int index=0; index<numtiles; index++)
			tmp_bmp[index]=vsp[index];
        //memcpy(str, vsp, (numtiles*256));
        //vfree(vsp); vsp=(unsigned char*)str; str=0;
		delete[] vsp;
		vsp = tmp_bmp;
        //memcpy(vsp+(numtiles*256), ovsp, (tilesripped*256));
		for (int index=0; index<tilesripped; index++) {
			tmp_bmp[numtiles+index]=create_bitmap(16,16);
			blit(ovsp[index],tmp_bmp[numtiles+index],0,0,0,0,16,16);
		}
        numtiles+=(unsigned short)tilesripped;

        // did we make a new layer?
        if (ly+1>numlayers)
        {
          numlayers++;
          sprintf(rstring, "%s%i", rstring, ly+1); // update render-order
        }

        //vfree(t);
        //vfree(ovsp);
		for (int index=0; index<ontiles; index++)
			destroy_bitmap(ovsp[index]);
		delete[] ovsp;
      }
      // ...
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>ilxofs+29 && mouse_x<ilxofs+8+29 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      chk_layer ^= 1;
      chk_layer &= 1;
      //chk_1 ^= 1;
      //chk_2=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>ilxofs+39 && mouse_x<ilxofs+8+39 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 2)
      {
      chk_layer ^= 2;
      chk_layer &= 2;
      //chk_2 ^= 1;
      //chk_1=0,chk_3=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mouse_b==1 && mouse_x>ilxofs+49 && mouse_x<ilxofs+8+49 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 3)
      {
      chk_layer ^= 4;
      chk_layer &= 4;
      //chk_3 ^= 1;
      //chk_1=0,chk_2=0,chk_4=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mouse_b==1 && mouse_x>ilxofs+59 && mouse_x<ilxofs+8+59 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 4)
      {
      chk_layer ^= 8;
      chk_layer &= 8;
      //chk_4 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_5=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mouse_b==1 && mouse_x>ilxofs+69 && mouse_x<ilxofs+8+69 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 5)
      {
      chk_layer ^= 16;
      chk_layer &= 16;
      //chk_5 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_4=0,chk_6=0;
      WaitRelease(); continue;
      }
    }

    if (mouse_b==1 && mouse_x>ilxofs+79 && mouse_x<ilxofs+8+79 && mouse_y>ilyofs+33 && mouse_y<ilyofs+7+33 && !moving)
    {
      if (numlayers+1 >= 6)
      {
      chk_layer ^= 32;
      chk_layer &= 32;
      //chk_6 ^= 1;
      //chk_1=0,chk_2=0,chk_3=0,chk_4=0,chk_5=0;
      WaitRelease(); continue;
      }
    }

    // if no checkboxes are marked, mark the first.
    //if (!(chk_1+chk_2+chk_3+chk_4+chk_5+chk_6)) chk_1=1;
    if (!chk_layer) chk_layer=1;

//    if (mouse_b==1 && mouse_x>ilxofs+29 && mouse_x<ilxofs+75+29 && mouse_y>ilyofs+12 && mouse_y<ilyofs+9+12 && !moving)
//    {
//      ctf=1; str=txt_imagename; cb=1;
//      cursorblink=systemtime+40; continue;
//    }

    if (mouse_b==1)
    {
      if (mouse_x>ilxofs+74 && mouse_x<ilxofs+12+74 && mouse_y>ilyofs+58 && mouse_y<ilyofs+9+58 && !moving)
      {
        if (ctf==2)
        {
          if (atoi(str)<minpady)
            sprintf(str, "%d", minpady);
        }
        ctf=1; str=nstr1; cb=1;
        cursorblink=systemtime+40; continue;
      }

      if (mouse_x>ilxofs+74 && mouse_x<ilxofs+12+74 && mouse_y>ilyofs+69 && mouse_y<ilyofs+9+69 && !moving)
      {
        if (ctf==1)
        {
          if (atoi(str)<minpadx)
            sprintf(str, "%d", minpadx);
        }
        ctf=2; str=nstr2; cb=1;
        cursorblink=systemtime+40; continue;
      }

      switch (ctf)
      {
        case 1: if (atoi(str) < minpadx) sprintf(str, "%d", minpadx); break;
        case 2: if (atoi(str) < minpady) sprintf(str, "%d", minpady); break;
      }
    }

    if (mouse_b==1 && mouse_x>ilxofs+5 && mouse_x<ilxofs+8+5 && mouse_y>ilyofs+101 && mouse_y<ilyofs+7+101 && !moving)
    {
      chk_grabexcess ^= 1;
      // <barf>
      if (excessx+excessy)
      {
        if (chk_grabexcess)
        {
          if (excessx) tilesx++;
          if (excessy) tilesy++;
        }
        else
        {
          if (excessx) tilesx--;
          if (excessy) tilesy--;
        }
        if (tilesx<MIN_TILE_X) { minpadx=MIN_TILE_X-tilesx; num_padx=minpadx; }
        if (tilesy<MIN_TILE_Y) { minpady=MIN_TILE_Y-tilesy; num_pady=minpady; }
        sprintf(nstr1, "%d", minpadx);
        sprintf(nstr2, "%d", minpady);
        totalx=tilesx+minpadx;
        totaly=tilesy+minpady;
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>ilxofs+5 && mouse_x<ilxofs+8+5 && mouse_y>ilyofs+110 && mouse_y<ilyofs+7+110 && !moving)
    {
      chk_centerinpad ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>ilxofs+5 && mouse_x<ilxofs+8+5 && mouse_y>ilyofs+119 && mouse_y<ilyofs+7+119 && !moving)
    {
      chk_nodup ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(ilxofs+ilxsize-9) && (mouse_x<ilxofs+ilxsize-2)
    && mouse_y>(ilyofs+1) && (mouse_y<ilyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>ilxofs && mouse_x<ilxofs+ilxsize && mouse_y>ilyofs && mouse_y<(ilyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-ilxofs;
      myo=mouse_y-ilyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;

        // keep minimally required padding
        if (ctf==1)
        {
          if (atoi(str)<minpadx)
            sprintf(str, "%d", minpadx);
        }
        if (ctf==2)
        {
          if (atoi(str)<minpady)
            sprintf(str, "%d", minpady);
        }

        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=0; str=0; break;
          //case 3: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=2; str=nstr2; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          //case 3: ctf=2; str=nstr1; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      tt=strlen(str);
      str[tt]=(char)c;
      str[tt+1]=0;
      last_pressed=0;
    }

  } while (!done);

  delete pcx;

  //num_padx=atoi(nstr1);
  //num_pady=atoi(nstr2);
}

int dtxsize=102, dtysize=52;
int dtxofs=100, dtyofs=70;
// aen
void Destroyer()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;
  int zn_check=0,zd_check=0,ob_check=0,ent_check=0;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      dtxofs = (mouse_x-mxo);
      dtyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (dtxofs<0) dtxofs = 0;
      if (dtyofs<0) dtyofs = 0;
      if (dtxofs+dtxsize>sx) dtxofs = sx-dtxsize;
      if (dtyofs+dtysize>sy) dtyofs = sy-dtysize;
    }

    Window(dtxofs, dtyofs, dtxofs+dtxsize, dtyofs+dtysize, "Destroyer");
    CheckBox(dtxofs+4, dtyofs+11, zn_check);
    CheckBox(dtxofs+4, dtyofs+31, ob_check);
    CheckBox(dtxofs+4, dtyofs+21, zd_check);
    GotoXY(dtxofs+15, dtyofs+12); printstring("Zones");
    GotoXY(dtxofs+15, dtyofs+32); printstring("Obstructions");
    GotoXY(dtxofs+15, dtyofs+22); printstring("Zone Data");
    Button(dtxofs+68, dtyofs+11, "ok");
    Button(dtxofs+68, dtyofs+24, "cancel");
    CheckBox(dtxofs+4, dtyofs+41, ent_check);
    GotoXY(dtxofs+15, dtyofs+42); printstring("Entities");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>dtxofs+4 && mouse_x<dtxofs+8+4 && mouse_y>dtyofs+11 && mouse_y<dtyofs+7+11 && !moving)
    {
      zn_check ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>dtxofs+4 && mouse_x<dtxofs+8+4 && mouse_y>dtyofs+21 && mouse_y<dtyofs+7+21 && !moving)
    {
      zd_check ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>dtxofs+4 && mouse_x<dtxofs+8+4 && mouse_y>dtyofs+31 && mouse_y<dtyofs+7+31 && !moving)
    {
      ob_check ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>dtxofs+4 && mouse_x<dtxofs+8+4 && mouse_y>dtyofs+41 && mouse_y<dtyofs+7+41 && !moving)
    {
      ent_check ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>dtxofs+68 && mouse_x<dtxofs+30+68 && mouse_y>dtyofs+11 && mouse_y<dtyofs+10+11 && !moving)
    {
      // insert code for button "ok" here
      ButtonPressed(dtxofs+68, dtyofs+11, "ok");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }

      if (Confirm("Are you sure?"))
      {
        done=1;
        if (ob_check) memset(obstruct, 0, (layer[0].sizex*layer[0].sizey)+4);
        if (zn_check) memset(zone, 0, (layer[0].sizex*layer[0].sizey)+4);
        if (zd_check)
        {
          numzones=0;
          memset(zones, 0, sizeof zones);
        }
        if (ent_check)
        {
          entities=0;
          nmchr=0;
          nms=0;
          memset(ms, 0, sizeof ms);
          memset(entity, 0, sizeof entities);
          memset(chrlist, 0, sizeof chrlist);
        }
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>dtxofs+68 && mouse_x<dtxofs+30+68 && mouse_y>dtyofs+24 && mouse_y<dtyofs+10+24 && !moving)
    {
      done=1;
      // insert code for button "cancel" here
      ButtonPressed(dtxofs+68, dtyofs+24, "cancel");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(dtxofs+dtxsize-9) && (mouse_x<dtxofs+dtxsize-2)
    && mouse_y>(dtyofs+1) && (mouse_y<dtyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>dtxofs && mouse_x<dtxofs+dtxsize && mouse_y>dtyofs && mouse_y<(dtyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-dtxofs;
      myo=mouse_y-dtyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

void LayersSubMenu(int dx, int dy)
{ int done=0;
  //int mxo, myo, cursorblink;

if (mouse_b==1) {
      WaitRelease();
}
  do
  {
    if (dx<0) dx=0;
    if (dy<0) dy=0;
    if (dx+80>sx) dx=sx-80;
    if (dy+89>sy) dy=sy-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+29);

    if (mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (mouse_y>dy+8 && mouse_y<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (mouse_y>dy+14 && mouse_y<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
       if (mouse_y>dy+20 && mouse_y<dy+26) FilledBox(dx+2, dy+20, 76, 7, darkw);
    }

    GotoXY(dx+3, dy+3); printstring("Add Layer");
    GotoXY(dx+3, dy+9); printstring("Edit Layer");
    GotoXY(dx+3, dy+15); printstring("Import Image");
    GotoXY(dx+3, dy+21); printstring("Destroyer");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mouse_b==1 && mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9) { AddLayer(); NoticeDialog(); done=1; WaitRelease(); break; }
       if (mouse_y>dy+8 && mouse_y<dy+15) { EditLayerProperties(); done=1; WaitRelease(); break; }
       if (mouse_y>dy+14 && mouse_y<dy+21) { ImageToLayer(); done=1; WaitRelease(); break; }
       if (mouse_y>dy+20 && mouse_y<dy+26) { Destroyer(); done=1; WaitRelease(); break; }
    }

    if (mouse_b==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[KEY_ESC]==1) done=1;
  } while (!done);
  key[KEY_ESC]=0;
}

void ExecuteSubMenu(int dx, int dy)
{ int done=0;
  //int mxo, myo, cursorblink;

  do
  {
    if (dx<0) dx=0;
    if (dy<0) dy=0;
    if (dx+80>sx) dx=sx-80;
    if (dy+89>sy) dy=sy-89;

    RenderMap();
    RenderGUI();
    stdwindow(dx, dy, dx+80, dy+23);

    if (mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9) FilledBox(dx+2, dy+2, 76, 7, darkw);
       if (mouse_y>dy+8 && mouse_y<dy+15) FilledBox(dx+2, dy+8, 76, 7, darkw);
       if (mouse_y>dy+14 && mouse_y<dy+21) FilledBox(dx+2, dy+14, 76, 7, darkw);
    }

    GotoXY(dx+3, dy+3); printstring("MAP");
    GotoXY(dx+3, dy+9); printstring("VERGE");
    GotoXY(dx+3, dy+15); printstring("DOS");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    if (mouse_b==1 && mouse_x>dx && mouse_x<dx+80)
    {
       if (mouse_y>dy+2 && mouse_y<dy+9)   { ShellMAP();   done=1; WaitRelease(); break; }
       if (mouse_y>dy+8 && mouse_y<dy+15)  { ShellVERGE(); done=1; WaitRelease(); break; }
       //if (mouse_y>dy+14 && mouse_y<dy+21) { ShellToDOS(); done=1; WaitRelease(); break; }
    }

    if (mouse_b==1)
    {
      WaitRelease();
      done=1;
    }
    if (key[KEY_ESC]==1) done=1;
  } while (!done);
  key[KEY_ESC]=0;
}

void AddLayer()
{
  layers[numlayers]=(unsigned short *) valloc(layer[0].sizex * layer[0].sizey * 2,"layer data",numlayers);
  //memset(layers[numlayers], 0, layer[0].sizex * layer[0].sizey * 2);
  layer[numlayers].pmultx=1; layer[numlayers].pmulty=1;
  layer[numlayers].pdivx=1;  layer[numlayers].pdivy=1;
  layer[numlayers].sizex=layer[0].sizex;
  layer[numlayers].sizey=layer[0].sizey;
  layer[numlayers].trans=0;
  layer[numlayers].hline=0;
  numlayers++;
}

int lpxsize=190, lpysize=68;
int lpxofs=40, lpyofs=60;

void EditLayerProperties()
{ int done=0, moving=0;
  int mxo, myo, cursorblink, i;
  int ctf=0, cb=0, c, t;
  char *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80], nstr5[80],nstr6[80];

  i=el;
  if (i>5) i=0;
  if (i>=numlayers) i=0;

  sprintf(nstr1, "%d", layer[i].pmultx);
  sprintf(nstr2, "%d", layer[i].pdivx);
  sprintf(nstr3, "%d", layer[i].pmulty);
  sprintf(nstr4, "%d", layer[i].pdivy);
  sprintf(nstr5, "%d", layer[i].trans);
  sprintf(nstr6, "%d", layer[i].hline);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lpxofs = (mouse_x-mxo);
      lpyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (lpxofs<0) lpxofs = 0;
      if (lpyofs<0) lpyofs = 0;
      if (lpxofs+lpxsize>sx) lpxofs = sx-lpxsize;
      if (lpyofs+lpysize>sy) lpyofs = sy-lpysize;
    }

    Window(lpxofs, lpyofs, lpxofs+lpxsize, lpyofs+lpysize, "Edit Layer Properties");
    GotoXY(lpxofs+3, lpyofs+10); printstring("Parallax controls:");
    GotoXY(lpxofs+4, lpyofs+21); printstring("MultX:");
    GotoXY(lpxofs+12, lpyofs+31); printstring("DivX:");
    GotoXY(lpxofs+93, lpyofs+21); printstring("MultY:");
    GotoXY(lpxofs+101, lpyofs+31); printstring("DivY:");
    TextField(lpxofs+29, lpyofs+19, 60, nstr1, ctf == 1 ? cb : 0);
    TextField(lpxofs+29, lpyofs+29, 60, nstr2, ctf == 2 ? cb : 0);
    TextField(lpxofs+118, lpyofs+19, 60, nstr3, ctf == 3 ? cb : 0);
    TextField(lpxofs+118, lpyofs+29, 60, nstr4, ctf == 4 ? cb : 0);
    HLine(lpxofs+1, lpyofs+43, 188, white);
    GotoXY(lpxofs+9, lpyofs+48); printstring("Transparent");
    GotoXY(lpxofs+36, lpyofs+58); printstring("HLine");
    TextField(lpxofs+56, lpyofs+46, 60, nstr5, ctf == 5 ? cb : 0);
    TextField(lpxofs+56, lpyofs+56, 60, nstr6, ctf == 6 ? cb : 0);
    Button(lpxofs+156, lpyofs+55, "OK");
    GotoXY(lpxofs+144, lpyofs+47); printstring("Layer");
    sprintf(strbuf,"%d",i); GotoXY(lpxofs+168, lpyofs+47); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>lpxofs+29 && mouse_x<lpxofs+60+29 && mouse_y>lpyofs+19 && mouse_y<lpyofs+9+19 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>lpxofs+29 && mouse_x<lpxofs+60+29 && mouse_y>lpyofs+29 && mouse_y<lpyofs+9+29 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>lpxofs+118 && mouse_x<lpxofs+60+118 && mouse_y>lpyofs+19 && mouse_y<lpyofs+9+19 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>lpxofs+118 && mouse_x<lpxofs+60+118 && mouse_y>lpyofs+29 && mouse_y<lpyofs+9+29 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>lpxofs+56 && mouse_x<lpxofs+60+56 && mouse_y>lpyofs+46 && mouse_y<lpyofs+9+46 && !moving)
    {
      ctf=5; str=nstr5; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>lpxofs+56 && mouse_x<lpxofs+60+56 && mouse_y>lpyofs+56 && mouse_y<lpyofs+9+56 && !moving)
    {
      ctf=6; str=nstr6; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>lpxofs+156 && mouse_x<lpxofs+30+156 && mouse_y>lpyofs+55 && mouse_y<lpyofs+10+55 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(lpxofs+156, lpyofs+55, "OK");
      DrawMouse(); done=1;
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(lpxofs+lpxsize-9) && (mouse_x<lpxofs+lpxsize-2)
    && mouse_y>(lpyofs+1) && (mouse_y<lpyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>lpxofs && mouse_x<lpxofs+lpxsize && mouse_y>lpyofs && mouse_y<(lpyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-lpxofs;
      myo=mouse_y-lpyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_UP)
      {
        layer[i].pmultx=(char)atoi(nstr1);
        layer[i].pdivx=(char)atoi(nstr2);
        layer[i].pmulty=(char)atoi(nstr3);
        layer[i].pdivy=(char)atoi(nstr4);
        layer[i].trans=(char)atoi(nstr5);
        layer[i].hline=(char)atoi(nstr6);
        last_pressed=0; key[KEY_UP]=0;
        if (i) i--; else i=numlayers-1;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
        sprintf(nstr5, "%d", layer[i].trans);
        sprintf(nstr6, "%d", layer[i].hline);
      }
      if ((last_pressed>>8)==KEY_DOWN)
      {
        layer[i].pmultx=(char)atoi(nstr1);
        layer[i].pdivx=(char)atoi(nstr2);
        layer[i].pmulty=(char)atoi(nstr3);
        layer[i].pdivy=(char)atoi(nstr4);
        layer[i].trans=(char)atoi(nstr5);
        layer[i].hline=(char)atoi(nstr6);
        last_pressed=0; key[KEY_DOWN]=0;
        if (i<numlayers-1) i++; else i=0;
        sprintf(nstr1, "%d", layer[i].pmultx);
        sprintf(nstr2, "%d", layer[i].pdivx);
        sprintf(nstr3, "%d", layer[i].pmulty);
        sprintf(nstr4, "%d", layer[i].pdivy);
        sprintf(nstr5, "%d", layer[i].trans);
        sprintf(nstr6, "%d", layer[i].hline);
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=5; str=nstr5; break;
          case 5: ctf=6; str=nstr6; break;
          case 6: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=6; str=nstr6; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
          case 5: ctf=4; str=nstr4; break;
          case 6: ctf=5; str=nstr5; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
  layer[i].pmultx=(char)atoi(nstr1);
  layer[i].pdivx=(char)atoi(nstr2);
  layer[i].pmulty=(char)atoi(nstr3);
  layer[i].pdivy=(char)atoi(nstr4);
  layer[i].trans=(char)atoi(nstr5);
  layer[i].hline=(char)atoi(nstr6);
}
void NewMAP()
{ int cursorblink;

    if (Confirm("Lose existing MAP?"))
    {
      for (cursorblink=0; cursorblink<numlayers; cursorblink++)
        vfree(layers[cursorblink]);
      memcpy(mapname,"UNTITLED.MAP",13);
      memcpy(vspname,"UNTITLED.VSP",13);
      memcpy(musname,"\0",1);
      memcpy(rstring,"1E",2);
      xstart=1; ystart=1;
      numlayers=1;
      wrap=0;

      // aen: default newmap dimensions set to 100x100
      layer[0].pmultx=1;  layer[0].pmulty=1;
      layer[0].pdivx=1;   layer[0].pdivy=1;
      layer[0].sizex=100; layer[0].sizey=100;
      layer[0].trans=0;   layer[0].hline=0;

      layers[0] = (unsigned short *)valloc((layer[0].sizex*layer[0].sizey*2)+4,"layer data",0);
      vfree(obstruct);
      obstruct=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"obstruct",0);
      vfree(zone);
      zone=(char *) valloc((layer[0].sizex*layer[0].sizey)+4,"zone",0);
      entities=0; // Mordred

      // aen: default number of tiles set to 100
	  for (int index=0; index<numtiles; index++)
		  destroy_bitmap(vsp[index]);
	  delete[] vsp;
      numtiles=100;
      //vfree(vsp);
      //vsp=(unsigned char *) valloc(256 * numtiles,"vsp data",0);
	  vsp = new BITMAP*[numtiles];
	  for (int index=0; index<numtiles; index++) {
		  vsp[index]=create_bitmap(16,16);
		  rectfill(vsp[index],0,0,16,16,0);
	  }


      // zero: was re-allocating same memory so gfx still visible in some places
      //memset(vsp,0,256*numtiles);
      // zero: gotta clear that anim stuff man!
      memset(vspanim,0,800);
      InitTileIDX();



      layertoggle[0]=1; layertoggle[1]=0; layertoggle[2]=0; layertoggle[3]=0;
      layertoggle[4]=0; layertoggle[5]=0; layertoggle[6]=0; layertoggle[7]=0;
      xwin=0; ywin=0;
    }
}

int amxsize=150, amysize=88;
int amxofs=60, amyofs=60;

void About()
{
  int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;

  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      amxofs = (mouse_x-mxo);
      amyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (amxofs<0) amxofs = 0;
      if (amyofs<0) amyofs = 0;
      if (amxofs+amxsize>sx) amxofs = sx-amxsize;
      if (amyofs+amysize>sy) amyofs = sy-amysize;
    }

    sprintf(strbuf, "About MapEd %s", ME2_VERSION);
    Window(amxofs, amyofs, amxofs+amxsize, amyofs+amysize, strbuf);
    sprintf(strbuf, "MapEd v.%s", ME2_VERSION);
    GotoXY(amxofs+52, amyofs+16); printstring(strbuf);
    GotoXY(amxofs+31, amyofs+22); printstring("Copyright (C) 1998 vecna");
    GotoXY(amxofs+40, amyofs+28); printstring("All Rights Reserved");
    GotoXY(amxofs+(75-pixels("Additional code by Ric")/2), amyofs+41);
    printstring("Additional code by Ric");
    GotoXY(amxofs+(75-pixels("Additional code by aen")/2), amyofs+47);
    printstring("Additional code by aen");
    GotoXY(amxofs+(75-pixels("Additional code by zeromus")/2), amyofs+53);
    printstring("Additional code by zeromus");
    Button(amxofs+60, amyofs+68, "OK");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mouse_b==1 && mouse_x>amxofs+66 && mouse_x<amxofs+30+66 && mouse_y>amyofs+68 && mouse_y<amyofs+10+68 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(amxofs+60, amyofs+68, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(amxofs+amxsize-9) && (mouse_x<amxofs+amxsize-2) &&
        mouse_y>(amyofs+1) && (mouse_y<amyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>amxofs && mouse_x<amxofs+amxsize && mouse_y>amyofs && mouse_y<(amyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-amxofs;
      myo=mouse_y-amyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB)
      {
        switch (ctf)
        {
          case 0: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }

  } while (!done);
}

int cnxsize=125, cnysize=50;
int cnxofs=114, cnyofs=70;

int Confirm(char *message)
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  WaitRelease();
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      cnxofs = (mouse_x-mxo);
      cnyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (cnxofs<0) cnxofs = 0;
      if (cnyofs<0) cnyofs = 0;
      if (cnxofs+cnxsize>sx) cnxofs = sx-cnxsize;
      if (cnyofs+cnysize>sy) cnyofs = sy-cnysize;
    }

    Window(cnxofs, cnyofs, cnxofs+cnxsize, cnyofs+cnysize, "Confirm? (Y/N)");
    GotoXY(cnxofs+125/2-(pixels(message)/2), cnyofs+15);  printstring(message);

    Button(cnxofs+24, cnyofs+30, "Yes");
    Button(cnxofs+69, cnyofs+30, "No");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>cnxofs+24 && mouse_x<cnxofs+30+24 && mouse_y>cnyofs+30 && mouse_y<cnyofs+10+30 && !moving)
    {
      // insert code for button "Yes" here
      ButtonPressed(cnxofs+24, cnyofs+30, "Yes");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      return 1;
    }

    if (mouse_b==1 && mouse_x>cnxofs+69 && mouse_x<cnxofs+30+69 && mouse_y>cnyofs+30 && mouse_y<cnyofs+10+30 && !moving)
    {
      ButtonPressed(cnxofs+69, cnyofs+30, "No");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      return 0;
    }

    if (mouse_b==1 && mouse_x>(cnxofs+cnxsize-9) && (mouse_x<cnxofs+cnxsize-2)
        && mouse_y>(cnyofs+1) && (mouse_y<cnyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>cnxofs && mouse_x<cnxofs+cnxsize && mouse_y>cnyofs && mouse_y<(cnyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-cnxofs;
      myo=mouse_y-cnyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }
      if ((last_pressed>>8)==KEY_N)
      {
        key[KEY_N]=0;
        done=1;
        break;
      }
      if ((last_pressed>>8)==KEY_Y)
      {
        key[KEY_Y]=0;
        return 1;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB)
      {
        switch (ctf)
        {
          case 0: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }

  } while (!done);
  return 0;
}

int lmxsize=145, lmysize=36;
int lmxofs=80, lmyofs=50;

void LoadMAPDialog()
{ char lmapname[80];
  int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  memset(lmapname, 0, 80);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lmxofs = (mouse_x-mxo);
      lmyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (lmxofs<0) lmxofs = 0;
      if (lmyofs<0) lmyofs = 0;
      if (lmxofs+lmxsize>sx) lmxofs = sx-lmxsize;
      if (lmyofs+lmysize>sy) lmyofs = sy-lmysize;
    }

    Window(lmxofs, lmyofs, lmxofs+lmxsize, lmyofs+lmysize, "Load MAP");
    GotoXY(lmxofs+2, lmyofs+13); printstring("Load MAP:");
    TextField(lmxofs+38, lmyofs+11, 70, lmapname, ctf == 1 ? cb : 0);
    Button(lmxofs+79, lmyofs+22, "OK");
    Button(lmxofs+111, lmyofs+22, "Cancel");
    Button(lmxofs+111, lmyofs+11, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>lmxofs+38 && mouse_x<lmxofs+70+38 && mouse_y>lmyofs+11 && mouse_y<lmyofs+9+11 && !moving)
    {
      ctf=1; str=lmapname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>lmxofs+79 && mouse_x<lmxofs+30+79 && mouse_y>lmyofs+22 && mouse_y<lmyofs+10+22 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      if (modified)
         if (!Confirm("Lose unsaved changes?")) { done=1; continue; }
      for (int index=0; index<numlayers; index++) {
           if (layers[index]) vfree(layers[index]);
	  }
      if (obstruct) vfree(obstruct);
      if (zone) vfree(zone);
      //if (vsp) vfree(vsp);
	  for (int index=0; index<numtiles; index++) {
		  destroy_bitmap(vsp[index]);
	  }
	  delete[] vsp;
	  vsp = 0;

      memcpy(mapname, lmapname, strlen(lmapname)+1);
      LoadMAP(mapname);
      InitTileIDX();
      xwin=0; ywin=0; done=1;

      ButtonPressed(lmxofs+79, lmyofs+22, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
	    RenderMap();
	    RenderGUI();
		DrawMouse();
        ShowPage();
        //ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>lmxofs+111 && mouse_x<lmxofs+30+111 && mouse_y>lmyofs+22 && mouse_y<lmyofs+10+22 && !moving)
    {
      // insert code for button "Cancel" here
      ButtonPressed(lmxofs+111, lmyofs+22, "Cancel");
      DrawMouse(); done=1;
      while (mouse_b)
      {
        ReadMouse();
	    RenderMap();
	    RenderGUI();
		DrawMouse();
        ShowPage();
        //ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>lmxofs+111 && mouse_x<lmxofs+30+111 && mouse_y>lmyofs+11 && mouse_y<lmyofs+10+11 && !moving) || (key[KEY_ALT] && key[KEY_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(lmxofs+111, lmyofs+11, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
	    RenderMap();
	    RenderGUI();
		DrawMouse();
        ShowPage();
        //ReadMouse();
      }
      WaitRelease();
      Browse(1,"*.map","","","",lmapname);
      continue;
    }

    if (mouse_b==1 && mouse_x>(lmxofs+lmxsize-9) && (mouse_x<lmxofs+lmxsize-2)
    && mouse_y>(lmyofs+1) && (mouse_y<lmyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>lmxofs && mouse_x<lmxofs+lmxsize && mouse_y>lmyofs && mouse_y<(lmyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-lmxofs;
      myo=mouse_y-lmyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB)
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lmapname; break;
          case 1: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }

  } while (!done);
}

int lvxsize=145, lvysize=35;
int lvxofs=80, lvyofs=50;

void LoadVSPDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;
  char lvspname[80];

  last_pressed=0;
  memset(lvspname,0,80);
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      lvxofs = (mouse_x-mxo);
      lvyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (lvxofs<0) lvxofs = 0;
      if (lvyofs<0) lvyofs = 0;
      if (lvxofs+lvxsize>sx) lvxofs = sx-lvxsize;
      if (lvyofs+lvysize>sy) lvyofs = sy-lvysize;
    }

    Window(lvxofs, lvyofs, lvxofs+lvxsize, lvyofs+lvysize, "Load VSP");
    GotoXY(lvxofs+2, lvyofs+13); printstring("Load VSP:");
    TextField(lvxofs+38, lvyofs+11, 70, lvspname, ctf == 1 ? cb : 0);
    Button(lvxofs+79, lvyofs+21, "OK");
    Button(lvxofs+111, lvyofs+21, "Cancel");
    Button(lvxofs+111, lvyofs+10, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>lvxofs+38 && mouse_x<lvxofs+70+38 && mouse_y>lvyofs+11 && mouse_y<lvyofs+9+11 && !moving)
    {
      ctf=1; str=lvspname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>lvxofs+79 && mouse_x<lvxofs+30+79 && mouse_y>lvyofs+21 && mouse_y<lvyofs+10+21 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      vfree(vsp);
      memcpy(vspname, lvspname, strlen(lvspname)+1);
      LoadVSP(vspname);
      InitTileIDX();
      ButtonPressed(lvxofs+79, lvyofs+21, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>lvxofs+111 && mouse_x<lvxofs+30+111 && mouse_y>lvyofs+21 && mouse_y<lvyofs+10+21 && !moving)
    {
      // insert code for button "Cancel" here
      ButtonPressed(lvxofs+111, lvyofs+21, "Cancel");
      DrawMouse(); done=1;
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>lvxofs+111 && mouse_x<lvxofs+30+111 && mouse_y>lvyofs+10 && mouse_y<lvyofs+10+10 && !moving) || (key[KEY_ALT] && key[KEY_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(lvxofs+111, lvyofs+10, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.vsp","","","",lvspname);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(lvxofs+lvxsize-9) && (mouse_x<lvxofs+lvxsize-2)
    && mouse_y>(lvyofs+1) && (mouse_y<lvyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>lvxofs && mouse_x<lvxofs+lvxsize && mouse_y>lvyofs && mouse_y<(lvyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-lvxofs;
      myo=mouse_y-lvyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lvspname; break;
          case 1: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=lvspname; break;
          case 1: ctf=0; str=lvspname; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}
int mpxsize=175, mpysize=37;
int mpxofs=60, mpyofs=60;

void MPDialog()
{ int done=0, moving=0;
  char playmod[80];
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  memset(playmod, 0, 80);
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      mpxofs = (mouse_x-mxo);
      mpyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (mpxofs<0) mpxofs = 0;
      if (mpyofs<0) mpyofs = 0;
      if (mpxofs+mpxsize>sx) mpxofs = sx-mpxsize;
      if (mpyofs+mpysize>sy) mpyofs = sy-mpysize;
    }

    Window(mpxofs, mpyofs, mpxofs+mpxsize, mpyofs+mpysize, "MOD Player Controls");
    GotoXY(mpxofs+2, mpyofs+15); printstring("MOD Playing:");
    TextField(mpxofs+49, mpyofs+13, 90, playmod, ctf == 1 ? cb : 0);
    Button(mpxofs+110, mpyofs+24, "Play");
    Button(mpxofs+142, mpyofs+24, "Stop");
    Button(mpxofs+78, mpyofs+24, "OK");
    Button(mpxofs+142, mpyofs+12, "Browse");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>mpxofs+49 && mouse_x<mpxofs+90+49 && mouse_y>mpyofs+13 && mouse_y<mpyofs+9+13 && !moving)
    {
      ctf=1; str=playmod; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>mpxofs+110 && mouse_x<mpxofs+30+110 && mouse_y>mpyofs+24 && mouse_y<mpyofs+10+24 && !moving)
    {
      // insert code for button "Play" here
      ButtonPressed(mpxofs+110, mpyofs+24, "Play");
      PlayMusic(playmod);
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>mpxofs+142 && mouse_x<mpxofs+30+142 && mouse_y>mpyofs+24 && mouse_y<mpyofs+10+24 && !moving)
    {
      // insert code for button "Stop" here
      memset(playingmod, 0, 80);
      memset(playmod, 0, 80);
      StopMusic();
      ButtonPressed(mpxofs+142, mpyofs+24, "Stop");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>mpxofs+78 && mouse_x<mpxofs+30+78 && mouse_y>mpyofs+24 && mouse_y<mpyofs+10+24 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(mpxofs+78, mpyofs+24, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>mpxofs+142 && mouse_x<mpxofs+30+142 && mouse_y>mpyofs+12 && mouse_y<mpyofs+10+12 && !moving) || (key[KEY_ALT] && key[KEY_B]))
    {
      // insert code for button "Browse" here
      ButtonPressed(mpxofs+142, mpyofs+12, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(4,"*.s3m","*.mod","*.xm",".it",playmod);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(mpxofs+mpxsize-9) && (mouse_x<mpxofs+mpxsize-2)
    && mouse_y>(mpyofs+1) && (mouse_y<mpyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>mpxofs && mouse_x<mpxofs+mpxsize && mouse_y>mpyofs && mouse_y<(mpyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-mpxofs;
      myo=mouse_y-mpyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=playmod; break;
          case 1: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=playmod; break;
          case 1: ctf=0; str=playmod; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

int zexsize=135, zeysize=86;
int zexofs=80, zeyofs=40;

void ZoneEdDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink, i=0;
  int ctf=0, cb=0, c, t;
  char *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  i=curzone;
  sprintf(nstr1, "%d", zones[i].script);
  sprintf(nstr2, "%d", zones[i].percent);
  sprintf(nstr3, "%d", zones[i].delay);
  sprintf(nstr4, "%d", zones[i].entityscript);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      zexofs = (mouse_x-mxo);
      zeyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (zexofs<0) zexofs = 0;
      if (zeyofs<0) zeyofs = 0;
      if (zexofs+zexsize>sx) zexofs = sx-zexsize;
      if (zeyofs+zeysize>sy) zeyofs = sy-zeysize;
    }

    Window(zexofs, zeyofs, zexofs+zexsize, zeyofs+zeysize, "Zone Editor");
    GotoXY(zexofs+8, zeyofs+12); printstring("Name:");
    TextField(zexofs+30, zeyofs+10, 100, zones[i].name, ctf == 1 ? cb : 0);
    GotoXY(zexofs+5, zeyofs+22); printstring("Script:");
    TextField(zexofs+30, zeyofs+20, 100, nstr1, ctf == 2 ? cb : 0);
    GotoXY(zexofs+2, zeyofs+32); printstring("Chance:");
    TextField(zexofs+30, zeyofs+30, 100, nstr2, ctf == 3 ? cb : 0);
    GotoXY(zexofs+7, zeyofs+42); printstring("Delay:");
    TextField(zexofs+30, zeyofs+40, 100, nstr3, ctf == 4 ? cb : 0);
    GotoXY(zexofs+4, zeyofs+52); printstring("Entity:");
    TextField(zexofs+30, zeyofs+50, 100, nstr4, ctf == 5 ? cb : 0);
    CheckBox(zexofs+20, zeyofs+61, zones[i].aaa);
    GotoXY(zexofs+31, zeyofs+62); printstring("Allow Adjacent Activation");
    Button(zexofs+99, zeyofs+71, "OK");
    GotoXY(zexofs+12, zeyofs+73); printstring("Zone");
    GotoXY(zexofs+30, zeyofs+73);
    sprintf(strbuf,"%d",i); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>zexofs+30 && mouse_x<zexofs+100+30 && mouse_y>zeyofs+10 && mouse_y<zeyofs+9+10 && !moving)
    {
      ctf=1; str=zones[i].name; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>zexofs+30 && mouse_x<zexofs+100+30 && mouse_y>zeyofs+20 && mouse_y<zeyofs+9+20 && !moving)
    {
      ctf=2; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>zexofs+30 && mouse_x<zexofs+100+30 && mouse_y>zeyofs+30 && mouse_y<zeyofs+9+30 && !moving)
    {
      ctf=3; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>zexofs+30 && mouse_x<zexofs+100+30 && mouse_y>zeyofs+40 && mouse_y<zeyofs+9+40 && !moving)
    {
      ctf=4; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>zexofs+30 && mouse_x<zexofs+100+30 && mouse_y>zeyofs+50 && mouse_y<zeyofs+9+50 && !moving)
    {
      ctf=5; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>zexofs+20 && mouse_x<zexofs+8+20 && mouse_y>zeyofs+61 && mouse_y<zeyofs+7+61 && !moving)
    {
      zones[i].aaa ^= 1; WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>zexofs+99 && mouse_x<zexofs+30+99 && mouse_y>zeyofs+71 && mouse_y<zeyofs+10+71 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(zexofs+99, zeyofs+71, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(zexofs+zexsize-9) && (mouse_x<zexofs+zexsize-2)
    && mouse_y>(zeyofs+1) && (mouse_y<zeyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>zexofs && mouse_x<zexofs+zexsize && mouse_y>zeyofs && mouse_y<(zeyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-zexofs;
      myo=mouse_y-zeyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }
      if ((last_pressed>>8)==KEY_UP)
      {
        zones[i].script=(char)atoi(nstr1);
        zones[i].percent=(char)atoi(nstr2);
        zones[i].delay=(unsigned short)atoi(nstr3);
        zones[i].entityscript=(char)atoi(nstr4);
        if (i<255) i++; else i=0;
        last_pressed=0; key[KEY_UP]=0;
        sprintf(nstr1, "%d", zones[i].script);
        sprintf(nstr2, "%d", zones[i].percent);
        sprintf(nstr3, "%d", zones[i].delay);
        sprintf(nstr4, "%d", zones[i].entityscript);
        continue;
      }
      if ((last_pressed>>8)==KEY_DOWN)
      {
        zones[i].script=(char)atoi(nstr1);
        zones[i].percent=(char)atoi(nstr2);
        zones[i].delay=(char)atoi(nstr3);
        zones[i].entityscript=(char)atoi(nstr4);
        if (i) i--; else i=255;
        last_pressed=0; key[KEY_DOWN]=0;
        sprintf(nstr1, "%d", zones[i].script);
        sprintf(nstr2, "%d", zones[i].percent);
        sprintf(nstr3, "%d", zones[i].delay);
        sprintf(nstr4, "%d", zones[i].entityscript);
        continue;
      }
      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=zones[i].name; break;
          case 1: ctf=2; str=nstr1; break;
          case 2: ctf=3; str=nstr2; break;
          case 3: ctf=4; str=nstr3; break;
          case 4: ctf=5; str=nstr4; break;
          case 5: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=nstr4; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=zones[i].name; break;
          case 3: ctf=2; str=nstr1; break;
          case 4: ctf=3; str=nstr2; break;
          case 5: ctf=4; str=nstr3; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }

  } while (!done);
  curzone=(char)i;
  zones[i].script=(char)atoi(nstr1);
  zones[i].percent=(char)atoi(nstr2);
  zones[i].delay=(unsigned short)atoi(nstr3);
  zones[i].entityscript=(char)atoi(nstr4);
}

int exsize=180, eysize=175;
int exofs=70, eyofs=30;

void EntityEditor(int i)
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80], nstr5[80], nstr6[80], nstr7[80], nstr8[80], nstr9[80], nstr10[80], nstr11[80], nstr12[80], nstr13[80];

  sprintf(nstr1, "%d", entity[i].x);
  sprintf(nstr2, "%d", entity[i].y);
  sprintf(nstr3, "%d", entity[i].chrindex);
  sprintf(nstr4, "%d", entity[i].speed);
  sprintf(nstr5, "%d", entity[i].actscript);
  sprintf(nstr6, "%d", entity[i].movecode);
  sprintf(nstr7, "%d", entity[i].step);
  sprintf(nstr8, "%d", entity[i].data2);
  sprintf(nstr9, "%d", entity[i].data3);
  sprintf(nstr10, "%d", entity[i].delay);
  sprintf(nstr11, "%d", entity[i].data5);
  sprintf(nstr12, "%d", entity[i].data6);
  sprintf(nstr13, "%d", entity[i].movescript);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      exofs = (mouse_x-mxo);
      eyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (exofs<0) exofs = 0;
      if (eyofs<0) eyofs = 0;
      if (exofs+exsize>sx) exofs = sx-exsize;
      if (eyofs+eysize>sy) eyofs = sy-eysize;
    }

    Window(exofs, eyofs, exofs+exsize, eyofs+eysize, "Entity Editor");
    GotoXY(exofs+7, eyofs+13); printstring("Desc:");
    TextField(exofs+26, eyofs+11, 80, entity[i].desc, ctf == 1 ? cb : 0);
    GotoXY(exofs+19, eyofs+23); printstring("X:");
    TextField(exofs+26, eyofs+21, 80, nstr1, ctf == 2 ? cb : 0);
    GotoXY(exofs+19, eyofs+33); printstring("Y:");
    TextField(exofs+26, eyofs+31, 80, nstr2, ctf == 3 ? cb : 0);
    GotoXY(exofs+11, eyofs+43); printstring("CHR:");
    TextField(exofs+26, eyofs+41, 80, nstr3, ctf == 4 ? cb : 0);

    // ***
    // entity image goes here (just above chr list button)
    // ***

    BigButton(exofs+116, eyofs+61, "CHR List");
    BigButton(exofs+115, eyofs+153, "Scripts");
    GotoXY(exofs+2, eyofs+155); printstring("Movescript:");
    HLine(exofs+1, eyofs+73, 178, white);
    GotoXY(exofs+3, eyofs+53); printstring("Speed:");
    TextField(exofs+26, eyofs+51, 80, nstr4, ctf == 5 ? cb : 0);
    TextField(exofs+26, eyofs+61, 80, nstr5, ctf == 6 ? cb : 0);
    GotoXY(exofs+107, eyofs+79); printstring("Is Obstruction");
    CheckBox(exofs+161, eyofs+87, entity[i].obsmode1);
    GotoXY(exofs+102, eyofs+88); printstring("Is Obstructable");
    CheckBox(exofs+161, eyofs+78, entity[i].obsmode2);
    GotoXY(exofs+24, eyofs+79); printstring("Auto Face");
    CheckBox(exofs+61, eyofs+78, entity[i].face);
    GotoXY(exofs+10, eyofs+88); printstring("Activ. Method");
    CheckBox(exofs+61, eyofs+87, entity[i].actm);
    HLine(exofs+1, eyofs+98, 178, white);
    GotoXY(exofs+126, eyofs+166); printstring("Entity");
    GotoXY(exofs+6, eyofs+104); printstring("Move Code:");
    TextField(exofs+47, eyofs+103, 80, nstr6, ctf == 7 ? cb : 0);
    TextField(exofs+30, eyofs+115, 40, nstr7, ctf == 8 ? cb : 0);
    TextField(exofs+30, eyofs+125, 40, nstr8, ctf == 9 ? cb : 0);
    TextField(exofs+30, eyofs+135, 40, nstr9, ctf == 10 ? cb : 0);
    TextField(exofs+113, eyofs+115, 40, nstr10, ctf == 11 ? cb : 0);
    TextField(exofs+113, eyofs+125, 40, nstr11, ctf == 12 ? cb : 0);
    TextField(exofs+113, eyofs+135, 40, nstr12, ctf == 13 ? cb : 0);
    HLine(exofs+1, eyofs+148, 178, white);
    TextField(exofs+46, eyofs+153, 60, nstr13, ctf == 14 ? cb : 0);
    GotoXY(exofs+5, eyofs+63); printstring("Activ:");
    GotoXY(exofs+152, eyofs+166);
    sprintf(strbuf, "%d/%d", i, entities-1); printstring(strbuf);
    entity[i].movecode=(char)atoi(nstr6);
    if (ctf!=4) entity[i].chrindex=(char)atoi(nstr3);

    switch (entity[i].movecode)
    {
      case 0: GotoXY(exofs+131, eyofs+104); printstring("Stopped"); break;
      case 1: GotoXY(exofs+131, eyofs+104); printstring("Wander");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:"); break;
      case 2: GotoXY(exofs+131, eyofs+104); printstring("Wander Zone");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:");
              GotoXY(exofs+10, eyofs+126); printstring("Zone:"); break;
      case 3: GotoXY(exofs+131, eyofs+104); printstring("Wander Box");
              GotoXY(exofs+11, eyofs+116); printstring("Step:");
              GotoXY(exofs+90, eyofs+116); printstring("Delay:");
              GotoXY(exofs+21, eyofs+126); printstring("X1:");
              GotoXY(exofs+21, eyofs+136); printstring("Y1:");
              GotoXY(exofs+102, eyofs+126); printstring("X2:");
              GotoXY(exofs+102, eyofs+136); printstring("Y2:"); break;
      case 4: GotoXY(exofs+131, eyofs+104); printstring("Scripted"); break;
    }
    switch (entity[i].actm)
    {
      case 0: GotoXY(exofs+72, eyofs+88); printstring("Actv"); break;
      case 1: GotoXY(exofs+72, eyofs+88); printstring("Adjc"); break;
    }

    if (chrs[entity[i].chrindex].frame)
      TCopySprite(exofs+143-(chrs[entity[i].chrindex].fx/2),
                  eyofs+31-(chrs[entity[i].chrindex].fy/2),
                  chrs[entity[i].chrindex].fx, chrs[entity[i].chrindex].fy,
                  chrs[entity[i].chrindex].frame);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+11 && mouse_y<eyofs+9+11 && !moving)
    {
      ctf=1; str=entity[i].desc; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+21 && mouse_y<eyofs+9+21 && !moving)
    {
      ctf=2; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+31 && mouse_y<eyofs+9+31 && !moving)
    {
      ctf=3; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+41 && mouse_y<eyofs+9+41 && !moving)
    {
      ctf=4; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+116 && mouse_x<exofs+50+116 && mouse_y>eyofs+61 && mouse_y<eyofs+10+61 && !moving)
    {
      // insert code for button "CHR List" here
      BigButtonPressed(exofs+116, eyofs+61, "CHR List");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      CHRList();
//      DoCHRdealy();
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+51 && mouse_y<eyofs+9+51 && !moving)
    {
      ctf=5; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+26 && mouse_x<exofs+80+26 && mouse_y>eyofs+61 && mouse_y<eyofs+9+61 && !moving)
    {
      ctf=6; str=nstr5; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+115 && mouse_x<exofs+50+115 && mouse_y>eyofs+153 && mouse_y<eyofs+10+153 && !moving)
    {
      // insert code for button "Scripts..." here
      BigButtonPressed(exofs+115, eyofs+153, "Scripts");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      MoveScriptDialog();
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+161 && mouse_x<exofs+8+161 && mouse_y>eyofs+87 && mouse_y<eyofs+7+87 && !moving)
    {
      entity[i].obsmode1 ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+161 && mouse_x<exofs+8+161 && mouse_y>eyofs+78 && mouse_y<eyofs+7+78 && !moving)
    {
      entity[i].obsmode2 ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+61 && mouse_x<exofs+8+61 && mouse_y>eyofs+78 && mouse_y<eyofs+7+78 && !moving)
    {
      entity[i].face ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+61 && mouse_x<exofs+8+61 && mouse_y>eyofs+87 && mouse_y<eyofs+7+87 && !moving)
    {
      entity[i].actm ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>exofs+47 && mouse_x<exofs+80+47 && mouse_y>eyofs+103 && mouse_y<eyofs+9+103 && !moving)
    {
      ctf=7; str=nstr6; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+30 && mouse_x<exofs+40+30 && mouse_y>eyofs+115 && mouse_y<eyofs+9+115 && !moving)
    {
      ctf=8; str=nstr7; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+30 && mouse_x<exofs+40+30 && mouse_y>eyofs+125 && mouse_y<eyofs+9+125 && !moving)
    {
      ctf=9; str=nstr8; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+30 && mouse_x<exofs+40+30 && mouse_y>eyofs+135 && mouse_y<eyofs+9+135 && !moving)
    {
      ctf=10; str=nstr9; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+113 && mouse_x<exofs+40+113 && mouse_y>eyofs+115 && mouse_y<eyofs+9+115 && !moving)
    {
      ctf=11; str=nstr10; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+113 && mouse_x<exofs+40+113 && mouse_y>eyofs+125 && mouse_y<eyofs+9+125 && !moving)
    {
      ctf=12; str=nstr11; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+113 && mouse_x<exofs+40+113 && mouse_y>eyofs+135 && mouse_y<eyofs+9+135 && !moving)
    {
      ctf=13; str=nstr12; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>exofs+46 && mouse_x<exofs+60+46 && mouse_y>eyofs+153 && mouse_y<eyofs+9+153 && !moving)
    {
      ctf=14; str=nstr13; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>(exofs+exsize-9) && (mouse_x<exofs+exsize-2)
    && mouse_y>(eyofs+1) && (mouse_y<eyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>exofs && mouse_x<exofs+exsize && mouse_y>eyofs && mouse_y<(eyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-exofs;
      myo=mouse_y-eyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_UP)
      {
        entity[i].x=atoi(nstr1);
        entity[i].y=atoi(nstr2);
        entity[i].chrindex=(char)atoi(nstr3);
        entity[i].speed=(char)atoi(nstr4);
        entity[i].actscript=atoi(nstr5);
        entity[i].movecode=(char)atoi(nstr6);
        entity[i].step=(unsigned short)atoi(nstr7);
        entity[i].data2=(unsigned short)atoi(nstr8);
        entity[i].data3=(unsigned short)atoi(nstr9);
        entity[i].delay=(unsigned short)atoi(nstr10);
        entity[i].data5=(unsigned short)atoi(nstr11);
        entity[i].data6=(unsigned short)atoi(nstr12);
        entity[i].movescript=(char)atoi(nstr13);

        if (i<entities-1) i++; else i=0;
        last_pressed=0; key[KEY_UP]=0;

        sprintf(nstr1, "%d", entity[i].x);
        sprintf(nstr2, "%d", entity[i].y);
        sprintf(nstr3, "%d", entity[i].chrindex);
        sprintf(nstr4, "%d", entity[i].speed);
        sprintf(nstr5, "%d", entity[i].actscript);
        sprintf(nstr6, "%d", entity[i].movecode);
        sprintf(nstr7, "%d", entity[i].step);
        sprintf(nstr8, "%d", entity[i].data2);
        sprintf(nstr9, "%d", entity[i].data3);
        sprintf(nstr10, "%d", entity[i].delay);
        sprintf(nstr11, "%d", entity[i].data5);
        sprintf(nstr12, "%d", entity[i].data6);
        sprintf(nstr13, "%d", entity[i].movescript);
        continue;
      }
      if ((last_pressed>>8)==KEY_DOWN)
      {
        entity[i].x=atoi(nstr1);
        entity[i].y=atoi(nstr2);
        entity[i].chrindex=(char)atoi(nstr3);
        entity[i].speed=(char)atoi(nstr4);
        entity[i].actscript=(char)atoi(nstr5);
        entity[i].movecode=(char)atoi(nstr6);
        entity[i].step=(unsigned short)atoi(nstr7);
        entity[i].data2=(unsigned short)atoi(nstr8);
        entity[i].data3=(unsigned short)atoi(nstr9);
        entity[i].delay=(unsigned short)atoi(nstr10);
        entity[i].data5=(unsigned short)atoi(nstr11);
        entity[i].data6=(unsigned short)atoi(nstr12);
        entity[i].movescript=(char)atoi(nstr13);

        if (i) i--; else i=entities-1;
        last_pressed=0; key[KEY_DOWN]=0;

        sprintf(nstr1, "%d", entity[i].x);
        sprintf(nstr2, "%d", entity[i].y);
        sprintf(nstr3, "%d", entity[i].chrindex);
        sprintf(nstr4, "%d", entity[i].speed);
        sprintf(nstr5, "%d", entity[i].actscript);
        sprintf(nstr6, "%d", entity[i].movecode);
        sprintf(nstr7, "%d", entity[i].step);
        sprintf(nstr8, "%d", entity[i].data2);
        sprintf(nstr9, "%d", entity[i].data3);
        sprintf(nstr10, "%d", entity[i].delay);
        sprintf(nstr11, "%d", entity[i].data5);
        sprintf(nstr12, "%d", entity[i].data6);
        sprintf(nstr13, "%d", entity[i].movescript);
        continue;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=entity[i].desc; break;
          case 1: ctf=2; str=nstr1; break;
          case 2: ctf=3; str=nstr2; break;
          case 3: ctf=4; str=nstr3; break;
          case 4: ctf=5; str=nstr4; break;
          case 5: ctf=6; str=nstr5; break;
          case 6: ctf=7; str=nstr6; break;
          case 7: ctf=8; str=nstr7; break;
          case 8: ctf=9; str=nstr8; break;
          case 9: ctf=10; str=nstr9; break;
          case 10: ctf=11; str=nstr10; break;
          case 11: ctf=12; str=nstr11; break;
          case 12: ctf=13; str=nstr12; break;
          case 13: ctf=14; str=nstr13; break;
          case 14: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=13; str=nstr12; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=entity[i].desc; break;
          case 3: ctf=2; str=nstr1; break;
          case 4: ctf=3; str=nstr2; break;
          case 5: ctf=4; str=nstr3; break;
          case 6: ctf=5; str=nstr4; break;
          case 7: ctf=6; str=nstr5; break;
          case 8: ctf=7; str=nstr6; break;
          case 9: ctf=8; str=nstr7; break;
          case 10: ctf=9; str=nstr8; break;
          case 11: ctf=10; str=nstr9; break;
          case 12: ctf=11; str=nstr10; break;
          case 13: ctf=12; str=nstr11; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
  entity[i].x=atoi(nstr1);
  entity[i].y=atoi(nstr2);
  entity[i].chrindex=(char)atoi(nstr3);
  entity[i].speed=(char)atoi(nstr4);
  entity[i].actscript=(char)atoi(nstr5);
  entity[i].movecode=(char)atoi(nstr6);
  entity[i].step=(unsigned short)atoi(nstr7);
  entity[i].data2=(unsigned short)atoi(nstr8);
  entity[i].data3=(unsigned short)atoi(nstr9);
  entity[i].delay=(unsigned short)atoi(nstr10);
  entity[i].data5=(unsigned short)atoi(nstr11);
  entity[i].data6=(unsigned short)atoi(nstr12);
  entity[i].movescript=(char)atoi(nstr13);
}

int clxsize=170, clysize=78;
int clxofs=105, clyofs=80;
char base=0;

void CHRList()
{ char done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      clxofs = (mouse_x-mxo);
      clyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (clxofs<0) clxofs = 0;
      if (clyofs<0) clyofs = 0;
      if (clxofs+clxsize>sx) clxofs = sx-clxsize;
      if (clyofs+clysize>sy) clyofs = sy-clysize;
    }

    Window(clxofs, clyofs, clxofs+clxsize, clyofs+clysize, "Entity CHR Reference Index");
    TextField(clxofs+14, clyofs+11, 120, chrlist[base].t, ctf == 1 ? cb : 0);
    TextField(clxofs+14, clyofs+22, 120, chrlist[base+1].t, ctf == 2 ? cb : 0);
    TextField(clxofs+14, clyofs+33, 120, chrlist[base+2].t, ctf == 3 ? cb : 0);
    TextField(clxofs+14, clyofs+44, 120, chrlist[base+3].t, ctf == 4 ? cb : 0);
    TextField(clxofs+14, clyofs+55, 120, chrlist[base+4].t, ctf == 5 ? cb : 0);
    Button(clxofs+137, clyofs+65, "OK");
    Button(clxofs+137, clyofs+10, "Browse");
    Button(clxofs+137, clyofs+21, "Browse");
    Button(clxofs+137, clyofs+32, "Browse");
    Button(clxofs+137, clyofs+43, "Browse");
    Button(clxofs+137, clyofs+54, "Browse");
    GotoXY(clxofs+3, clyofs+11); sprintf(strbuf,"%d:", base); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+22); sprintf(strbuf,"%d:", base+1); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+33); sprintf(strbuf,"%d:", base+2); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+44); sprintf(strbuf,"%d:", base+3); printstring(strbuf);
    GotoXY(clxofs+3, clyofs+55); sprintf(strbuf,"%d:", base+4); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>clxofs+14 && mouse_x<clxofs+120+14 && mouse_y>clyofs+11 && mouse_y<clyofs+9+11 && !moving)
    {
      ctf=1; str=chrlist[base].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+14 && mouse_x<clxofs+120+14 && mouse_y>clyofs+22 && mouse_y<clyofs+9+22 && !moving)
    {
      ctf=2; str=chrlist[base+1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+14 && mouse_x<clxofs+120+14 && mouse_y>clyofs+33 && mouse_y<clyofs+9+33 && !moving)
    {
      ctf=3; str=chrlist[base+2].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+14 && mouse_x<clxofs+120+14 && mouse_y>clyofs+44 && mouse_y<clyofs+9+44 && !moving)
    {
      ctf=4; str=chrlist[base+3].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+14 && mouse_x<clxofs+120+14 && mouse_y>clyofs+55 && mouse_y<clyofs+9+55 && !moving)
    {
      ctf=5; str=chrlist[base+4].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+65 && mouse_y<clyofs+10+65 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(clxofs+137, clyofs+65, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+10 && mouse_y<clyofs+10+10 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+10, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base].t);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+21 && mouse_y<clyofs+10+21 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+21, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+1].t);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+32 && mouse_y<clyofs+10+32 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+32, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+2].t);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+43 && mouse_y<clyofs+10+43 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+43, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+3].t);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>clxofs+137 && mouse_x<clxofs+30+137 && mouse_y>clyofs+54 && mouse_y<clyofs+10+54 && !moving)
    {
      // insert code for button "Browse" here
      ButtonPressed(clxofs+137, clyofs+54, "Browse");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      Browse(1,"*.chr","","","",chrlist[base+4].t);
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(clxofs+clxsize-9) && (mouse_x<clxofs+clxsize-2)
    && mouse_y>(clyofs+1) && (mouse_y<clyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>clxofs && mouse_x<clxofs+clxsize && mouse_y>clyofs && mouse_y<(clyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-clxofs;
      myo=mouse_y-clyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }
      if (key[KEY_UP])
      {
        key[KEY_UP]=0;
        if (base) base--;
        last_pressed=0;
        continue;
      }
      if (key[KEY_DOWN])
      {
        key[KEY_DOWN]=0;
        if (base<95) base++;
        last_pressed=0;
        continue;
      }
      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=chrlist[base].t; break;
          case 1: ctf=2; str=chrlist[base+1].t; break;
          case 2: ctf=3; str=chrlist[base+2].t; break;
          case 3: ctf=4; str=chrlist[base+3].t; break;
          case 4: ctf=5; str=chrlist[base+4].t; break;
          case 5: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=chrlist[base].t; break;
          case 1: ctf=0; str=chrlist[base].t; break;
          case 2: ctf=1; str=chrlist[base+1].t; break;
          case 3: ctf=2; str=chrlist[base+2].t; break;
          case 4: ctf=3; str=chrlist[base+3].t; break;
          case 5: ctf=4; str=chrlist[base+4].t; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

int msxsize=181, msysize=77;
int msxofs=70, msyofs=50;
int base1=0;

void MoveScriptDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      msxofs = (mouse_x-mxo);
      msyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (msxofs<0) msxofs = 0;
      if (msyofs<0) msyofs = 0;
      if (msxofs+msxsize>sx) msxofs = sx-msxsize;
      if (msyofs+msysize>sy) msyofs = sy-msysize;
    }

    Window(msxofs, msyofs, msxofs+msxsize, msyofs+msysize, "Movement Script Editor");
    TextField(msxofs+15, msyofs+12, 160, ms[base1].t, ctf == 1 ? cb : 0);
    TextField(msxofs+15, msyofs+22, 160, ms[base1+1].t, ctf == 2 ? cb : 0);
    TextField(msxofs+15, msyofs+32, 160, ms[base1+2].t, ctf == 3 ? cb : 0);
    TextField(msxofs+15, msyofs+42, 160, ms[base1+3].t, ctf == 4 ? cb : 0);
    TextField(msxofs+15, msyofs+52, 160, ms[base1+4].t, ctf == 5 ? cb : 0);
    Button(msxofs+146, msyofs+63, "OK");
    GotoXY(msxofs+3, msyofs+11); sprintf(strbuf,"%d:", base1); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+22); sprintf(strbuf,"%d:", base1+1); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+33); sprintf(strbuf,"%d:", base1+2); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+44); sprintf(strbuf,"%d:", base1+3); printstring(strbuf);
    GotoXY(msxofs+3, msyofs+55); sprintf(strbuf,"%d:", base1+4); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>msxofs+15 && mouse_x<msxofs+160+15 && mouse_y>msyofs+12 && mouse_y<msyofs+10+12 && !moving)
    {
      ctf=1; str=ms[base1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>msxofs+15 && mouse_x<msxofs+160+15 && mouse_y>msyofs+22 && mouse_y<msyofs+9+22 && !moving)
    {
      ctf=2; str=ms[base1+1].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>msxofs+15 && mouse_x<msxofs+160+15 && mouse_y>msyofs+32 && mouse_y<msyofs+9+32 && !moving)
    {
      ctf=3; str=ms[base1+2].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>msxofs+15 && mouse_x<msxofs+160+15 && mouse_y>msyofs+42 && mouse_y<msyofs+9+42 && !moving)
    {
      ctf=4; str=ms[base1+3].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>msxofs+15 && mouse_x<msxofs+160+15 && mouse_y>msyofs+52 && mouse_y<msyofs+9+52 && !moving)
    {
      ctf=5; str=ms[base1+4].t; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>msxofs+146 && mouse_x<msxofs+30+146 && mouse_y>msyofs+63 && mouse_y<msyofs+10+63 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      done=1;
      ButtonPressed(msxofs+146, msyofs+63, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(msxofs+msxsize-9) && (mouse_x<msxofs+msxsize-2)
    && mouse_y>(msyofs+1) && (mouse_y<msyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>msxofs && mouse_x<msxofs+msxsize && mouse_y>msyofs && mouse_y<(msyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-msxofs;
      myo=mouse_y-msyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }
      if (key[KEY_UP])
      {
        key[KEY_UP]=0;
        if (base1) base1--;
        last_pressed=0;
        continue;
      }
      if (key[KEY_DOWN])
      {
        key[KEY_DOWN]=0;
        if (base1<95) base1++;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=ms[base1].t; break;
          case 1: ctf=2; str=ms[base1+1].t; break;
          case 2: ctf=3; str=ms[base1+2].t; break;
          case 3: ctf=4; str=ms[base1+3].t; break;
          case 4: ctf=5; str=ms[base1+4].t; break;
          case 5: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=ms[base1+4].t; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=ms[base1].t; break;
          case 3: ctf=2; str=ms[base1+1].t; break;
          case 4: ctf=3; str=ms[base1+2].t; break;
          case 5: ctf=4; str=ms[base1+3].t; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

int prxsize=150, prysize=80;
int prxofs=50, pryofs=60;

void MapPropertiesDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      prxofs = (mouse_x-mxo);
      pryofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (prxofs<0) prxofs = 0;
      if (pryofs<0) pryofs = 0;
      if (prxofs+prxsize>sx) prxofs = sx-prxsize;
      if (pryofs+prysize>sy) pryofs = sy-prysize;
    }

    Window(prxofs, pryofs, prxofs+prxsize, pryofs+prysize, "MAP Properties");
    GotoXY(prxofs+10, pryofs+23); printstring("Music:");
    TextField(prxofs+33, pryofs+11, 112, mapname, ctf == 1 ? cb : 0);
    TextField(prxofs+33, pryofs+21, 112, musname, ctf == 2 ? cb : 0);
    GotoXY(prxofs+18, pryofs+33); printstring("VSP:");
    TextField(prxofs+33, pryofs+31, 112, vspname, ctf == 3 ? cb : 0);
    GotoXY(prxofs+3, pryofs+43); printstring("rstring:");
    TextField(prxofs+33, pryofs+41, 112, rstring, ctf == 4 ? cb : 0);
    BigButton(prxofs+97, pryofs+56, "Resize MAP");
    HLine(prxofs+1, pryofs+53, 148, white);
    GotoXY(prxofs+3, pryofs+58); printstring("xsize:");
    sprintf(strbuf,"%d",layer[0].sizex); printstring(strbuf);
    GotoXY(prxofs+49, pryofs+58); printstring("ysize:");
    sprintf(strbuf,"%d",layer[0].sizey); printstring(strbuf);
    Button(prxofs+117, pryofs+67, "OK");
    GotoXY(prxofs+16, pryofs+13); printstring("MAP:");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>prxofs+33 && mouse_x<prxofs+112+33 && mouse_y>pryofs+11 && mouse_y<pryofs+9+11 && !moving)
    {
      ctf=1; str=mapname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>prxofs+33 && mouse_x<prxofs+112+33 && mouse_y>pryofs+21 && mouse_y<pryofs+9+21 && !moving)
    {
      ctf=2; str=musname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>prxofs+33 && mouse_x<prxofs+112+33 && mouse_y>pryofs+31 && mouse_y<pryofs+9+31 && !moving)
    {
      ctf=3; str=vspname; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>prxofs+33 && mouse_x<prxofs+112+33 && mouse_y>pryofs+41 && mouse_y<pryofs+9+41 && !moving)
    {
      ctf=4; str=rstring; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>prxofs+97 && mouse_x<prxofs+30+97 && mouse_y>pryofs+56 && mouse_y<pryofs+10+56 && !moving)
    {
      // insert code for button "Resize MAP" here
      BigButtonPressed(prxofs+97, pryofs+56, "Resize MAP");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease();
      ResizeMapDialog();
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>prxofs+117 && mouse_x<prxofs+30+117 && mouse_y>pryofs+67 && mouse_y<pryofs+10+67 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(prxofs+117, pryofs+67, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(prxofs+prxsize-9) && (mouse_x<prxofs+prxsize-2)
    && mouse_y>(pryofs+1) && (mouse_y<pryofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>prxofs && mouse_x<prxofs+prxsize && mouse_y>pryofs && mouse_y<(pryofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-prxofs;
      myo=mouse_y-pryofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=mapname; break;
          case 1: ctf=2; str=musname; break;
          case 2: ctf=3; str=vspname; break;
          case 3: ctf=4; str=rstring; break;
          case 4: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=4; str=rstring; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=mapname; break;
          case 3: ctf=2; str=musname; break;
          case 4: ctf=3; str=vspname; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }

  } while (!done);

}

int rsxsize=134, rsysize=44;
int rsxofs=20, rsyofs=20;

static void PutByte(unsigned char *to, unsigned char v, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return;

  to[(y*w)+x]=v;
}

static unsigned char GetByte(unsigned char *from, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return 0;

  return from[(y*w)+x];
}

static void PutWord(unsigned short *to, unsigned short v, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return;

  to[(y*w)+x]=v;
}

static unsigned short GetWord(unsigned short *from, int x, int y, int w, int h)
{
  if (x<0 || x>=w || y<0 || y>=h)
    return 0;

  return from[(y*w)+x];
}

void ResizeMapDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink, basex, basey, i; //, xl, yl;
  int ctf=0, cb=0, c, t;
  char *str, *tb; //, *s1, *d1;
  char nstr1[80], nstr2[80];
  unsigned short *tbw; //, *s=NULL,*d=NULL;
  //int temp_width;
  int a,b;

  basex=layer[0].sizex;
  basey=layer[0].sizey;
  sprintf(nstr1, "%d", basex);
  sprintf(nstr2, "%d", basey);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      rsxofs = (mouse_x-mxo);
      rsyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (rsxofs<0) rsxofs = 0;
      if (rsyofs<0) rsyofs = 0;
      if (rsxofs+rsxsize>sx) rsxofs = sx-rsxsize;
      if (rsyofs+rsysize>sy) rsyofs = sy-rsysize;
    }

    Window(rsxofs, rsyofs, rsxofs+rsxsize, rsyofs+rsysize, "Resize MAP");
    GotoXY(rsxofs+4, rsyofs+12); printstring("Base X:");
    GotoXY(rsxofs+4, rsyofs+22); printstring("Base Y:");
    TextField(rsxofs+30, rsyofs+10, 100, nstr1, ctf == 1 ? cb : 0);
    TextField(rsxofs+30, rsyofs+20, 100, nstr2, ctf == 2 ? cb : 0);
    Button(rsxofs+69, rsyofs+31, "Resize");
    Button(rsxofs+101, rsyofs+31, "Cancel");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>rsxofs+30 && mouse_x<rsxofs+100+30 && mouse_y>rsyofs+10 && mouse_y<rsyofs+9+10 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>rsxofs+30 && mouse_x<rsxofs+100+30 && mouse_y>rsyofs+20 && mouse_y<rsyofs+9+20 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>rsxofs+69 && mouse_x<rsxofs+30+69 && mouse_y>rsyofs+31 && mouse_y<rsyofs+10+31 && !moving)
    {
      printf("&");
      fflush(stdout);
      // insert code for button "Resize" here
      ButtonPressed(rsxofs+69, rsyofs+31, "Resize");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      basex=atoi(nstr1);
      basey=atoi(nstr2);

      tb = (char *)valloc(basex*basey,"tilebuf",0);
      for (b=0; b<basey; ++b)
        for (a=0; a<basex; ++a)
          PutByte((unsigned char*)tb,
            GetByte((unsigned char*)obstruct,a,b,layer[0].sizex,layer[0].sizey),
            a,b,basex,basey);
      vfree(obstruct);
      obstruct=tb;

      tb = (char *)valloc(basex*basey,"tilebuf",0);
      for (b=0; b<basey; ++b)
        for (a=0; a<basex; ++a)
          PutByte((unsigned char*)tb,
            GetByte((unsigned char*)zone,a,b,layer[0].sizex,layer[0].sizey),
            a,b,basex,basey);
      vfree(zone);
      zone=tb;

      for (i=0; i<numlayers; i++)
      {
        tbw=(unsigned short *)valloc(basex*basey*2,"tilebuf",0);
        //memset(tbw, 0, basex*basey*2);
        for (b=0; b<basey; ++b)
          for (a=0; a<basex; ++a)
            PutWord(tbw,
              GetWord(layers[i],a,b,layer[i].sizex,layer[i].sizey),
              a,b,basex,basey);

        vfree(layers[i]);
        layers[i]=tbw;
      }
      for (i=0; i<numlayers; ++i)
      {
        layer[i].sizex=(unsigned short)basex;
        layer[i].sizey=(unsigned short)basey;
      }
      xwin=0; ywin=0;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>rsxofs+101 && mouse_x<rsxofs+30+101 && mouse_y>rsyofs+31 && mouse_y<rsyofs+10+31 && !moving)
    {
      // insert code for button "Cancel" here
      done=1;
      ButtonPressed(rsxofs+101, rsyofs+31, "Cancel");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(rsxofs+rsxsize-9) && (mouse_x<rsxofs+rsxsize-2)
    && mouse_y>(rsyofs+1) && (mouse_y<rsyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>rsxofs && mouse_x<rsxofs+rsxsize && mouse_y>rsyofs && mouse_y<(rsyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-rsxofs;
      myo=mouse_y-rsyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=2; str=nstr2; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
  basex=atoi(nstr1);
  basey=atoi(nstr2);
}

int cdxsize=150, cdysize=45;
int cdxofs=110, cdyofs=70;
char curtrack=1;

void CDDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink=0;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      cdxofs = (mouse_x-mxo);
      cdyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (cdxofs<0) cdxofs = 0;
      if (cdyofs<0) cdyofs = 0;
      if (cdxofs+cdxsize>sx) cdxofs = sx-cdxsize;
      if (cdyofs+cdysize>sy) cdyofs = sy-cdysize;
    }

    Window(cdxofs, cdyofs, cdxofs+cdxsize, cdyofs+cdysize, "CD Player");
    Button(cdxofs+53, cdyofs+10, "Play");
    Button(cdxofs+53, cdyofs+21, "Stop");
    Button(cdxofs+85, cdyofs+10, "Eject");
    Button(cdxofs+85, cdyofs+21, "Close");
    Button(cdxofs+117, cdyofs+10, "Next");
    Button(cdxofs+117, cdyofs+21, "Prev");
    Button(cdxofs+117, cdyofs+32, "OK");
    GotoXY(cdxofs+4, cdyofs+18); printstring("Track ");
    sprintf(strbuf,"%d",curtrack); printstring(strbuf);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>cdxofs+53 && mouse_x<cdxofs+30+53 && mouse_y>cdyofs+10 && mouse_y<cdyofs+10+10 && !moving)
    {
      // insert code for button "Play" here
      ButtonPressed(cdxofs+53, cdyofs+10, "Play");
      DrawMouse();
      ShowPage();
      //CD_Play(curtrack);
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>cdxofs+53 && mouse_x<cdxofs+30+53 && mouse_y>cdyofs+21 && mouse_y<cdyofs+10+21 && !moving)
    {
      // insert code for button "Stop" here
      ButtonPressed(cdxofs+53, cdyofs+21, "Stop");
      DrawMouse();
      ShowPage();
      //CD_Stop();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>cdxofs+85 && mouse_x<cdxofs+30+85 && mouse_y>cdyofs+10 && mouse_y<cdyofs+10+10 && !moving)
    {
      // insert code for button "Eject" here
      ButtonPressed(cdxofs+85, cdyofs+10, "Eject");
      DrawMouse();
      ShowPage();
      //CD_Open_Door();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>cdxofs+85 && mouse_x<cdxofs+30+85 && mouse_y>cdyofs+21 && mouse_y<cdyofs+10+21 && !moving)
    {
      // insert code for button "Close" here
      ButtonPressed(cdxofs+85, cdyofs+21, "Close");
      DrawMouse();
      ShowPage();
      //CD_Close_Door();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>cdxofs+117 && mouse_x<cdxofs+30+117 && mouse_y>cdyofs+10 && mouse_y<cdyofs+10+10 && !moving)
    {
      // insert code for button "Next" here
      ButtonPressed(cdxofs+117, cdyofs+10, "Next");
      curtrack++;
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>cdxofs+117 && mouse_x<cdxofs+30+117 && mouse_y>cdyofs+21 && mouse_y<cdyofs+10+21 && !moving)
    {
      // insert code for button "Prev" here
      ButtonPressed(cdxofs+117, cdyofs+21, "Prev");
      DrawMouse();
      if (curtrack>1) curtrack--;
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if ((mouse_b==1 && mouse_x>cdxofs+117 && mouse_x<cdxofs+30+117 && mouse_y>cdyofs+32 && mouse_y<cdyofs+10+32 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(cdxofs+117, cdyofs+32, "OK");
      DrawMouse();
      done=1;
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(cdxofs+cdxsize-9) && (mouse_x<cdxofs+cdxsize-2)
    && mouse_y>(cdyofs+1) && (mouse_y<cdyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>cdxofs && mouse_x<cdxofs+cdxsize && mouse_y>cdyofs && mouse_y<(cdyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-cdxofs;
      myo=mouse_y-cdyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

int ntxsize=130, nytsize=51;
int ntxofs=110, ntyofs=70;

void NoticeDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink=0;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      ntxofs = (mouse_x-mxo);
      ntyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (ntxofs<0) ntxofs = 0;
      if (ntyofs<0) ntyofs = 0;
      if (ntxofs+ntxsize>sx) ntxofs = sx-ntxsize;
      if (ntyofs+nytsize>sy) ntyofs = sy-nytsize;
    }

    Window(ntxofs, ntyofs, ntxofs+ntxsize, ntyofs+nytsize, "Notice!");
    GotoXY(ntxofs+20, ntyofs+20); printstring("Don't forget to add this");
    GotoXY(ntxofs+14, ntyofs+26); printstring("layer to the Render String!");
    Button(ntxofs+50, ntyofs+36, "OK");
    GotoXY(ntxofs+25, ntyofs+14); printstring("Layer has been added.");

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if ((mouse_b==1 && mouse_x>ntxofs+50 && mouse_x<ntxofs+30+50 && mouse_y>ntyofs+36 && mouse_y<ntyofs+10+36 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(ntxofs+50, ntyofs+36, "OK");
      done=1;
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>(ntxofs+ntxsize-9) && (mouse_x<ntxofs+ntxsize-2)
    && mouse_y>(ntyofs+1) && (mouse_y<ntyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>ntxofs && mouse_x<ntxofs+ntxsize && mouse_y>ntyofs && mouse_y<(ntyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-ntxofs;
      myo=mouse_y-ntyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
}

int oxsize=150, oysize=98;
int oxofs=120, oyofs=70;
extern char vcedprog[80];

void OptionsDialog()
{ int done=0, moving=0;
  int mxo, myo, cursorblink;
  int ctf=0, cb=0, c, t;
  char *str;
  char nstr1[80], nstr2[80], nstr3[80], nstr4[80];

  sprintf(nstr1, "%d", winbg);
  sprintf(nstr2, "%d", titlebg);
  sprintf(nstr3, "%d", brightw);
  sprintf(nstr4, "%d", darkw);
  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      oxofs = (mouse_x-mxo);
      oyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (oxofs<0) oxofs = 0;
      if (oyofs<0) oyofs = 0;
      if (oxofs+oxsize>sx) oxofs = sx-oxsize;
      if (oyofs+oysize>sy) oyofs = sy-oysize;
    }

    Window(oxofs, oyofs, oxofs+oxsize, oyofs+oysize, "MapEd Options");
    GotoXY(oxofs+16, oyofs+12); printstring("Smooth Scroll");
    CheckBox(oxofs+5, oyofs+11, scrollmode);
    GotoXY(oxofs+16, oyofs+20); printstring("Tile Highlight");
    CheckBox(oxofs+5, oyofs+19, mh);
    GotoXY(oxofs+38, oyofs+31); printstring("Window Background");
    TextField(oxofs+5, oyofs+29, 30, nstr1, ctf == 1 ? cb : 0);
    TextField(oxofs+5, oyofs+39, 30, nstr2, ctf == 2 ? cb : 0);
    GotoXY(oxofs+38, oyofs+41); printstring("Titlebar Background");
    CheckBox(oxofs+74, oyofs+11, vm);
    GotoXY(oxofs+84, oyofs+12); printstring("640x480");
    CheckBox(oxofs+74, oyofs+19, mouse_scroll);
    GotoXY(oxofs+84, oyofs+20); printstring("Mouse Scrolling");
    TextField(oxofs+5, oyofs+49, 30, nstr3, ctf == 3 ? cb : 0);
    TextField(oxofs+5, oyofs+59, 30, nstr4, ctf == 4 ? cb : 0);
    GotoXY(oxofs+38, oyofs+51); printstring("White");
    GotoXY(oxofs+38, oyofs+61); printstring("Shadow");
    Button(oxofs+60, oyofs+85, "OK");
    HLine(oxofs+1, oyofs+71, 148, white);
    GotoXY(oxofs+3, oyofs+76); printstring("Editor:");
    TextField(oxofs+28, oyofs+74, 118, vcedprog, ctf == 5 ? cb : 0);

    DrawMouse();
    ShowPage();
    CheckTimerStuff();

    // input phase
    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+8+5 && mouse_y>oyofs+11 && mouse_y<oyofs+7+11 && !moving)
    {
      xwin=xwin>>4; xwin=xwin<<4;
      ywin=ywin>>4; ywin=ywin<<4;
      scrollmode ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+8+5 && mouse_y>oyofs+19 && mouse_y<oyofs+7+19 && !moving)
    {
      mh ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+30+5 && mouse_y>oyofs+29 && mouse_y<oyofs+9+29 && !moving)
    {
      ctf=1; str=nstr1; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+30+5 && mouse_y>oyofs+39 && mouse_y<oyofs+9+39 && !moving)
    {
      ctf=2; str=nstr2; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+74 && mouse_x<oxofs+8+74 && mouse_y>oyofs+11 && mouse_y<oyofs+7+11 && !moving)
    {
      vm ^= 1; WaitRelease();
      ShutdownVideo();
	  if (0==vm) {
		  InitVideo(320,200,120,(key_shifts & KB_ALT_FLAG)!=0);
	  } else {
		  InitVideo(640,480,120,(key_shifts & KB_ALT_FLAG)!=0);
	  }
      set_intensity(63);
      InitMouse();
      continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+74 && mouse_x<oxofs+8+74 && mouse_y>oyofs+19 && mouse_y<oyofs+7+19 && !moving)
    {
      mouse_scroll ^= 1; WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+30+5 && mouse_y>oyofs+49 && mouse_y<oyofs+9+49 && !moving)
    {
      ctf=3; str=nstr3; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+5 && mouse_x<oxofs+30+5 && mouse_y>oyofs+59 && mouse_y<oyofs+9+59 && !moving)
    {
      ctf=4; str=nstr4; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if ((mouse_b==1 && mouse_x>oxofs+60 && mouse_x<oxofs+30+60 && mouse_y>oyofs+85 && mouse_y<oyofs+10+85 && !moving) || (key[KEY_ENTER] && !ctf))
    {
      // insert code for button "OK" here
      ButtonPressed(oxofs+60, oyofs+85, "OK");
      DrawMouse();
      while (mouse_b)
      {
        ReadMouse();
        ShowPage();
        ReadMouse();
      }
      done=1;
      WaitRelease(); continue;
    }

    if (mouse_b==1 && mouse_x>oxofs+28 && mouse_x<oxofs+118+28 && mouse_y>oyofs+74 && mouse_y<oyofs+9+74 && !moving)
    {
      ctf=5; str=vcedprog; cb=1;
      cursorblink=systemtime+40; continue;
    }

    if (mouse_b==1 && mouse_x>(oxofs+oxsize-9) && (mouse_x<oxofs+oxsize-2)
    && mouse_y>(oyofs+1) && (mouse_y<oyofs+8) && !moving)
    {
      done=1;
      WaitRelease();
      break;
    }

    if (mouse_b==1 && mouse_x>oxofs && mouse_x<oxofs+oxsize && mouse_y>oyofs && mouse_y<(oyofs+9) && !moving)
    {
      moving=1;
      mxo=mouse_x-oxofs;
      myo=mouse_y-oyofs;
      continue;
    }

    if (mouse_b==1 && !moving)
    {
      ctf=0;
      cb=0;
      str=0;
      WaitRelease();
    }

    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && !key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=1; str=nstr1; break;
          case 1: ctf=2; str=nstr2; break;
          case 2: ctf=3; str=nstr3; break;
          case 3: ctf=4; str=nstr4; break;
          case 4: ctf=5; str=vcedprog; break;
          case 5: ctf=0; str=0; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if ((last_pressed>>8)==KEY_TAB && key[KEY_LSHIFT])
      {
        switch (ctf)
        {
          case 0: ctf=5; str=vcedprog; break;
          case 1: ctf=0; str=0; break;
          case 2: ctf=1; str=nstr1; break;
          case 3: ctf=2; str=nstr2; break;
          case 4: ctf=3; str=nstr3; break;
          case 5: ctf=4; str=nstr4; break;
        }
        key[KEY_TAB]=0;
        last_pressed=0;
        cb=1; cursorblink=systemtime+40;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
  } while (!done);
  winbg=atoi(nstr1);
  titlebg=atoi(nstr2);
  brightw=atoi(nstr3);
  darkw=atoi(nstr4);
}

int vcexsize=200, vceysize=60;
int vcexofs=76, vceyofs=70;

void VCNotify(char *msg)
{ int done=0, moving=0;
  int mxo=0, myo=0, cursorblink=0;
  int ctf=0, cb=0, c, t;
  char *str;

  last_pressed=0;
  do
  {
    RenderMap();
    RenderGUI();

    if (ctf && (int)systemtime >= cursorblink)
    {
      cb ^= 1;
      cursorblink = systemtime+40;
    }
    if (moving)
    {
      vcexofs = (mouse_x-mxo);
      vceyofs = (mouse_y-myo);
      if (!mouse_b) moving=0;
      if (vcexofs<0) vcexofs = 0;
      if (vceyofs<0) vceyofs = 0;
      if (vcexofs+vcexsize>sx) vcexofs = sx-vcexsize;
      if (vceyofs+vceysize>sy) vceyofs = sy-vceysize;
    }

    Window(vcexofs, vceyofs, vcexofs+vcexsize, vceyofs+vceysize, "VergeC Error Report");	Button(vcexofs+85, vceyofs+41, "OK");	GotoXY(vcexofs+29, vceyofs+17); printstring("VCC has reported the following error:");	GotoXY(vcexofs+10-(pixels(msg)/2), vceyofs+23); printstring(msg);	DrawMouse();	ShowPage();	CheckTimerStuff();	// input phase	if ((mb==1 && mx>vcexofs+85 && mx<vcexofs+30+85 && my >vceyofs+41 && my<vceyofs+10+41 && !moving) || key[SCAN_ENTER] && !ctf))	{		// insert code for button "OK" here		ButtonPressed(vcexofs+85, vceyofs+41, "OK");		DrawMouse();		while (mb)		{			ReadMouse();			ShowPage();			ReadMouse();		}		done=1;		WaitRelease(); continue;	}	if (mb==1 && mx>(vcexofs+vcexsize-9) && (mx<vcexofs+vcexsize-2)	&& my>(vceyofs+1) && (my<vceyofs+8) && !moving)	{		done=1;		WaitRelease();		break;	}	if (mb==1 && mx>vcexofs && mx<vcexofs+vcexsize && my>vceyofs && my<(vceyofs+9) && !moving)	{		moving=1;		mxo=mx-vcexofs;		myo=my-vceyofs;		continue;	}	if (mb==1 && !moving)	{		ctf=0;		cb=0;		str=0;		WaitRelease();	}    if (last_pressed)
    {
      //if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) c=key_shift_tbl[last_pressed];
      //else c=key_ascii_tbl[last_pressed];
	  c=(char)(last_pressed&0xff);

      if ((last_pressed>>8)==KEY_ESC)
      {
        key[KEY_ESC]=0;
        done=1;
        break;
      }

      if ((last_pressed>>8)==KEY_ENTER)
      {
        key[KEY_ENTER]=0;
        ctf=0;
        str=0;
        last_pressed=0;
        continue;
      }

      if (!ctf)
        continue;

      if ((last_pressed>>8)==KEY_BACKSPACE && strlen(str))
      {
        str[strlen(str)-1]=0;
        key[KEY_BACKSPACE]=0;
        last_pressed=0;
        continue;
      }
      else if ((last_pressed>>8)==KEY_BACKSPACE)
      {
        last_pressed=0;
        continue;
      }

      t=strlen(str);
      str[t]=(char)c;
      str[t+1]=0;
      last_pressed=0;
    }
	} while (!done);
}