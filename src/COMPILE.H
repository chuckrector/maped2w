/*
VERGE 2.5+j (AKA V2k+j) -  A video game creation engine
Copyright (C) 1998-2000  Benjamin Eirich (AKA vecna), et al
Please see authors.txt for a complete list of contributing authors.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef COMPILE_INC
#define OMPILE_INC

//#include "linked.h"
#include <vector>
#include <direct.h>
#include <io.h>
#include <hash_map>

#include "lexical.h"

#define VOID 1
#define INT 1
#define CHARPTR 2
#define STRING 3

extern char	xvc_sig[9];

// event offset marker
struct funcmark
//	: public linked_node
{
	int mark;
	funcmark(int x)
	: mark(x) {}
	//void mark_the_spot(int x) { mark=x; }
	int compare(void* c);
};

struct function_t
//	: public linked_node
{
	int created_index;

	char fname[40];
	char argtype[20];
	int numargs, numlocals;
	int returntype;
	int syscodeofs;

	function_t();

	int compare(void* o);
	void write(FILE* fp);
	void read(FILE* fp);
};

struct variable_t
//	: public linked_node
{
	int created_index;

	char vname[40];
	int varstartofs;
	int arraylen;

	variable_t();

	int compare(void* o);
	void write(FILE* fp);
	void read(FILE* fp);
};

struct string_sym
//	: public linked_node
{
	int created_index;

	char vname[40];
	int vsofs;
	int arraylen;

	string_sym();

	int compare(void* o);
	void write(FILE* fp);
	void read(FILE* fp);
};

extern function_t*	find_fun;
extern variable_t*	find_var;
extern string_sym*	find_str;

extern unsigned char *source, *src;
extern unsigned char *outbuf, *code;
extern unsigned char inevent;

extern function_t* current_func;

extern int curstartofs;
extern int sstartofs;

//extern linked_list functionlist;
//extern linked_list varlist;
//extern linked_list strlist;
extern std::vector<function_t*> functionlist_array;
extern std::vector<variable_t*> varlist_array;
extern std::vector<string_sym*> strlist_array;

typedef std::hash_map<char*, variable_t*>/*, raw_compare>*/ variable_lookup;
extern variable_lookup varlist;

typedef std::hash_map<char*, function_t*>/*, raw_compare>*/ function_lookup;
extern function_lookup functionlist;

typedef std::hash_map<char*, string_sym*>/*, raw_compare>*/ string_lookup;
extern string_lookup strlist;


//extern int varidx;
//extern int funcidx;

extern int startsyscript;

#define MAX_LOCALS 20
extern char larg[MAX_LOCALS][40]; // was [12][40]. blah
//extern int c;

//extern char *functbl[512];
//extern linked_list functbl;
extern std::vector<funcmark*> functbl;
//extern int mfuncs;

extern void CompileMAP(_finddata_t found, int maxsize, bool readSysIdx);
extern void CompileSystem(_finddata_t found);
extern void Expect(char *str);

extern void EmitC(char c);
extern void EmitW(short int w);
extern void EmitD(int w);
extern void EmitOperand();
extern void EmitString(char *str);
extern void HandleString();

#endif // COMPILE_INC
