/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef LEXICAL_INC
#define LEXICAL_INC

#include <hash_map>

extern int hardfuncs_hash_ready;
extern int hardvar0_hash_ready;
extern int hardvar1_hash_ready;

extern void init_hardfuncs_hash();
extern void init_hardvar0_hash();
extern void init_hardvar1_hash();

extern void ParseWhitespace();
extern void GetNumber();
extern void GetToken();
extern void InitCompileSystem();
extern int NextIs(char *str);

extern void GetString(void);

struct token_t {
	int index;
	char ident[1024];
	int value;
	int type;
	int subtype;
	char* ptr;
};

struct hardfunc_t {
	int index;
	int ret;
};

extern token_t tok;

typedef stdext::hash_map<char*, hardfunc_t*>/*, raw_compare>*/ hardfunc_lookup;
typedef stdext::hash_map<char*, int> hardvar0_lookup;
typedef stdext::hash_map<char*, int> hardvar1_lookup;

extern hardfunc_lookup hardfuncs_hash;
extern hardvar0_lookup hardvar0_hash;
extern hardvar1_lookup hardvar1_hash;

extern char token[2000];                 // Token buffer
extern unsigned int token_nvalue;        // int value of token if it's type DIGIT
extern char token_type;                  // type of current token.
extern char token_subtype;               // This is just crap.
extern unsigned char chr_table[256];              // Character type table.
extern char returntypes[];
extern int lines, tlines;
extern char *source_file;

extern char TokenIs(char *str);
extern void Expect(char *str);

extern int numhardfuncs, funcidx;
extern int varcategory, numhardvar0;
extern int numhardvar1;
extern char *hardfuncs[];
extern char *hardvar0[];
extern char *hardvar1[];

extern void	GetEmitStringLiteral();

#endif // LEXICAL_INC
