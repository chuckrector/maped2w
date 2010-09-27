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

// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                        The VergeC Compiler                          ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                            Main module                              ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

#define VERSION "2.5.27"

#ifdef __DJGPP__
#define BUILD_TAG "DJGPP V2\0"
#endif

#ifdef __WATCOMC__
#define BUILD_TAG "Watcom 11.0\0"
#endif

#define BUILD_TAG "Visual C++ (.NET)\0"

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <direct.h>
#include <io.h>
#include <iostream>
#include <time.h>

#include "vcc.h"
#include "compile.h"
#include "lexical.h"
#include "allegro.h"

extern int pp_dump;
extern int pp_nomark;

// DATA ////////////////////////////////////////////////////////////////////////////////////////////

char outmode, cmode;
char fname[80];
char quiet, verbose;
int locate=0;

// CODE ////////////////////////////////////////////////////////////////////////////////////////////

char* va(char* format, ...)
{
	va_list argptr;
	static char temp[1024];

	va_start(argptr, format);
	vsprintf(temp, format, argptr);
	va_end(argptr);

	return temp;
}

void dprint(char *message, ...)
{
	va_list  lst;
	char     string[1024];

	if (quiet) {
		return;
	}

	va_start (lst, message);
	vsprintf (string, message, lst);
    va_end   (lst);
    printf   ("%s \n", string);
}

void vprint(char *message, ...)
{
	va_list  lst;
	char     string[1024];

	if (!verbose) {
		return;
	}

	va_start (lst, message);
	vsprintf (string, message, lst);
	va_end   (lst);

    printf   ("%s \n", string);
}

void err(char *message, ...)
{
	va_list  lst;
	char     string[1024];

	va_start (lst, message);
	vsprintf (string, message, lst);
	va_end   (lst);

	if (quiet) {
		FILE* efile;
		fopen_s(&efile, "ERROR.TXT", "w");
		fprintf(efile, "%s \n", string);
		fclose(efile);
	} else {
		allegro_message("%s \n", string);
	}

	remove("vcctemp.$$$");
	throw SomeError();
	//exit(-1);
}

void vcerr(char* message, ...)
{
	va_list	lst;
	char	temp[1024]="";

	// compose message
	va_start(lst, message);
	vsprintf(temp, message, lst);
	va_end(lst);

	err("%s(%d) %s", source_file, lines, temp);
}

// debugging
void log(char* message, ...)
{
	va_list lst;
	static char temp[1024];
	FILE* fp;
	errno_t error;

	temp[0]='\0';
	va_start(lst, message);
	vsprintf(temp, message, lst);
	va_end(lst);

	error = fopen_s(&fp, "VCC.LOG", "a+");
	if (error) {
		err("log: unable to open VCC.LOG for writing");
	}
	fprintf(fp, temp);
	fclose(fp);
}

void DestroyEventMarkers()
{
	for (int index=0; index<(int)functbl.size(); index++)
		if (functbl[index]) delete functbl[index];
	functbl.clear();
	//while (functbl.head())
	//{
	//	funcmark* pmark=(funcmark *)functbl.head();
	//	functbl.unlink((linked_node *)pmark);
	//	delete pmark;
	//}
}

void vcc_compile_mode_map(_finddata_t found, int maxsize, bool readSysIdx)
{
	FILE	*outfile, *infile;
	errno_t error;
	char *x	=0;
	int z	=0;

// disregard extension
	x = found.name;
	while ('.' != *x) ++x;
	*x = 0;

	DestroyEventMarkers();

	current_func=0;	// DO *NOT* FORGET THIS; lest CheckLibFunc giveth you a man-beating.

	CompileMAP(found,maxsize,readSysIdx);

// append map extension and open
	error = fopen_s(&infile, va("%s.map", found.name), "rb+");
	if (error) {
		err("unable to open %s.", va("%s.map", found.name));
	}

// TODO: make this check for the sig and simply notify user if invalid
	fseek(infile, 6, SEEK_CUR);

// read size of map data (which is stored minus the appended code size)
	fread(&z, 1, 4, infile);
	fseek(infile, 0, SEEK_SET);

	error = fopen_s(&outfile, "outtemp.$$$", "wb");
	if (error) {
		err("unable to open outtemp.$$$");
	}

// copy map data to output file
	x = new char[z];
	if (!x)
	{
		err("vcc_compile_mode_map: memory exhausted on x");
	}
	fread(x, 1, z, infile);
	fwrite(x, 1, z, outfile);
	delete[] x; // whoops

// done with input file, now we're just writing to output
	fclose(infile);

// write code offsets for events
	int totmarkers=(int)functbl.size();//number_nodes();
	fwrite(&totmarkers, 1, 4, outfile);
	for (int index=0; index<(int)functbl.size(); index++) {
		int n=functbl[index]->mark;
		fwrite(&n, 1, 4, outfile);
	}
	//functbl.go_head();
	//do
	//{
	//	int n=((funcmark *)functbl.current())->mark;
	//	fwrite(&n, 1, 4, outfile);

	//	functbl.go_next();
	//} while (functbl.current() != functbl.head());

// write code size then dump code
	int codesize=(int)(code-outbuf);
	fwrite(&codesize, 1, 4, outfile);
	fwrite(outbuf, 1, code-outbuf, outfile);

	fclose(outfile);

// remove existing map file, and rename temp name to map's name
	remove(va("%s.map", found.name));
	rename("outtemp.$$$", va("%s.map", found.name));
}

void DumpSystemVCS()
{
	FILE*	f;
	errno_t error;
	int		n;
	char	buf[8];
	char*	index;

	f = 0;
	n = 0;
	buf[0] = '\0';

	error = fopen_s(&f, "system.xvc", "rb");
// ensure system.xvc exists; if not at this point, just create empty one
	if (error) {
		error = fopen_s(&f, "system.xvc", "wb");
		if (error) {
			err("DumpSystemVCS: error opening system.xvc for writing.");
		}
		fwrite(xvc_sig, 1, 8, f);
		n = 8+4;
		fwrite(&n, 1, 4, f);
	}
	fclose(f);

	error = fopen_s(&f, "system.xvc", "rb");
	if (error) {
		err("DumpSystemVCS: unable to open system.xvc");
	}

// check signature
	fread(buf, 1, 8, f);
	if (strncmp(buf, xvc_sig, 7))
	{
		err("DumpSystemVCS: system.xvc contains invalid signature: %s", buf);
	}

// seek to start position for code dump
	fread(&n, 1, 4, f);

// save start of file
	index = new char [n];
	if (!index)
	{
		err("DumpSystemVCS: memory exahusted on index");
	}
	fseek(f, 0, SEEK_SET);
	fread(index, 1, n, f);

	fclose(f);

	error = fopen_s(&f, "system.xvc", "wb");
	if (error) {
		err("DumpSystemVCS: unable to open system.xvc for code append");
	}

// write back start of file
	fwrite(index, 1, n, f);

// function count is simply number of nodes in function list
	int numfuncs=(int)functionlist.size();//number_nodes();
	fwrite(&numfuncs, 1, 4, f);

// these two still use separate counters because arrayed vars increase the total overall count
	fwrite(&curstartofs, 1, 4, f);
	fwrite(&sstartofs, 1, 4, f);

// write all system script code
	fwrite(outbuf, 1, code-outbuf, f);

// VC end-of-code marker
	fputc(255, f);

	fclose(f);
}

void vcc_compile_mode_system()
{
	current_func=0; // DO *NOT* FORGET THIS; lest CheckLibFunc giveth you a man-beating.

	intptr_t hFile;
	struct _finddata_t found;

// compile map scripts
	if ((hFile = _findfirst("SYSTEM.VC", &found)) == -1L)
	{
		err("SYSTEM.VC not found.");
	}

	_findclose(hFile);

	CompileSystem(found);
	DumpSystemVCS();
}

void vcc_compile_mode_all()
{
	intptr_t hFile;
	struct _finddata_t f;

// compile system script
	vcc_compile_mode_system();

// compile map scripts
	if ((hFile = _findfirst("*.MAP", &f)) == -1L)
	{
		err("No mapfiles found.");
	}

	std::vector<_finddata_t> files;
	int maxsize=0;
	do {
		files.push_back(f);
		if (maxsize<(int)f.size) {
			maxsize=f.size;
		}
	} while (!_findnext(hFile, &f));

	for (int index=0; index<(int)files.size(); index++) {
		_finddata_t& found=files[index];
		//printf("Compiling map: %s\n",found.name);
		vcc_compile_mode_map(found,maxsize,false);
	}

	_findclose(hFile);

	dprint("%i total VC lines compiled.", tlines);
}

/*
void DestroyVarList()
{
	while (varlist.head())
	{
		variable_t* p=(variable_t *)varlist.head();
		varlist.unlink((linked_node *)p);
		delete p;
	}
}

void DestroyFunctionList()
{
	while (functionlist.head())
	{
		function_t* p=(function_t *)functionlist.head();
		functionlist.unlink((linked_node *)p);
		delete p;
	}
}

void DestroyStrList()
{
	while (strlist.head())
	{
		string_t* p=(string_t *)strlist.head();
		strlist.unlink((linked_node *)p);
		delete p;
	}
}
*/

//#pragma off (unreferenced);

void CompileMap_forMaped(char* filename) {
	try {
		InitCompileSystem();

		intptr_t hFile;
		struct _finddata_t f;
	// compile map scripts
		if ((hFile = _findfirst(filename, &f)) == -1L) {
			err(va("%s not found.",filename));
		}
		_findclose(hFile);

		vcc_compile_mode_map(f,f.size,true);

		remove("vcctemp.$$$");
		remove("ERROR.TXT");
	} catch (SomeError) {
		//...
	}
}

void CompileEverything_forMaped() {
	try {
		InitCompileSystem();
		
		vcc_compile_mode_all();

		remove("vcctemp.$$$");
		remove("ERROR.TXT");
	} catch (SomeError) {
		//...
	}
}

//int main(int argc, char *argv[])
//{
//	clock_t start=clock();
//
//	int loop		=0;
//	char ch			=0;
//	char *argstr	=0;
//
//	cmode		=0;
//	pp_dump		=0;
//	pp_nomark	=0;
//
//	for (loop = 1; loop < argc; loop++)
//	{
//	// point to argument string
//		argstr = &loop[argv][0];
//
//		ch = *argstr;
//	// skip leading punctuators, if any
//		if ('-' == ch || '+' == ch || '/' == ch)
//		{
//			++argstr;
//		}
//
//		if (!stricmp(argstr, "v"))
//		{
//			verbose = 1; continue;
//		}
//		if (!stricmp(argstr, "q"))
//		{
//			quiet = 1; continue;
//		}
//	// compile SYSTEM.VC only
//		if (!stricmp(argstr, "system"))
//		{
//			cmode = 2; continue;
//		}
//	// compile all available .VC files
//		if (!stricmp(argstr, "all"))
//		{
//			cmode = 3; continue;
//		}
//	// disable line/#include markers
//		if (!stricmp(argstr, "ppnomark"))
//		{
//			pp_nomark = 1; continue;
//		}
//	// dump preprocessor output to temp files
//		if (!stricmp(argstr, "ppdump"))
//		{
//			pp_dump = 1; continue;
//		}
//	// debug locator option
//		if ('.' == *argstr) {
//			locate = atoi(argstr+1); continue;
//		}
//
//	// at this point, the argument is assumed to be a file
//		strncpy(fname, argstr, 79);
//		fname[79]='\0';
//
//		cmode = 1;
//		continue;
//	}
//
//	dprint("vcc v.%s\nCopyright (C)1998 Benjamin Eirich. All rights reserved.\nAdapted for STL in 2004 by Charles Rector.", VERSION);
//	vprint("%s build %s on %s %s", BUILD_TAG, __FILE__, __DATE__, __TIME__);
//
//	if (!cmode)
//	{
//		err("No input files.");
//	}
//
//
//	switch (cmode)
//	{
//		case 1: {
//			intptr_t hFile;
//			struct _finddata_t f;
//		// compile map scripts
//			if ((hFile = _findfirst(fname, &f)) == -1L) {
//				err(va("%s not found.",fname));
//			}
//			_findclose(hFile);
//
//			vcc_compile_mode_map(f,f.size,true);
//			break;
//		}
//		case 2: vcc_compile_mode_system(); break;
//		case 3: vcc_compile_mode_all(); break;
//
//		default:
//			err("You have now entered the twilight zone.");
//	}
//
//	/*
//// free everything
//	DestroyVarList();
//	DestroyFunctionList();
//	DestroyStrList();
//	delete[] source; source=0;
//	delete[] outbuf; outbuf=0;
//	*/
//
//	remove("vcctemp.$$$");
//	remove("ERROR.TXT");
//
//	clock_t end=clock();
//	double total=(double)(end-start)/CLOCKS_PER_SEC;
//	printf("Took %g seconds\n",total);
//
//	return 0;
//}