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

/*
	tokenize.h
	coded by aen
*/

#ifndef TOKENIZE_INC
#define TOKENIZE_INC

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

//#include "str.h"

class zToken
{
public:
	zToken(std::string nid="", int nline=0)
		: id(nid), line(nline)
		{ }
		std::string id;
	int line;
};


class zTokenizer
{
private:
	std::string filename;

// valid symbol groupings
	std::vector<std::string> symbol_key;

// source token accumulation
	std::vector<zToken> tokens;
	int curtoken;

// input stream traveller
	const char* source;

// > RECOGNIZERS <
	bool isWhite(char c);
	bool isAlpha(char c);
	bool isBinaryDigit(char c);
	bool IsOctalDigit(char c);
	bool isDigit(char c);
	bool isHexDigit(char c);
	bool isAlphaNumeric(char c);
	bool isQuote(char c);
	bool isValidIdentifier(std::string s);
	bool isValidInteger(std::string s);
	bool isValidNumber(std::string s);

// > EATERS <
	bool atC_Comment();
	bool atC_CommentEnd();
	bool atCPP_Comment();
	void skipC_Comment();
	void skipCPP_Comment();
	void skipWhite();

// > COLLECTORS <
	zToken collectIdentifier();
	zToken collectNumber();
	std::string accumulateStringLiteral();
	zToken collectStringLiteral();
	zToken collectSymbol();
	zToken collectToken();

	void error(std::string message)
	{
		std::cout << "zTokenizer: error: " << message.c_str() << std::endl;
		abort();
	}
public:
	zTokenizer()
		{ }
~zTokenizer()
	{ }

// generate symbol table from a NULL-terminated character string array
	void DefineSymbols(const char* nsymbol_key[])
	{
	int n;
	const char* e;

		e = nsymbol_key[n = 0];
		while (e)
		{
			symbol_key.push_back(e);
			e = nsymbol_key[++n];
		}
	}
// generate symbol table from a vector_t<string_t> object
	void DefineSymbols(std::vector<std::string>& nsymbol_key)
	{
		symbol_key = nsymbol_key;
	}

	void Tokenize(std::string nfilename)
	{
	// every time we tokenize a file, save the name
		filename = nfilename;

		std::ifstream in;
		in.open(filename.c_str(),std::ios::binary);
		std::filebuf* pbuf = in.rdbuf();
		int size=pbuf->pubseekoff(0,std::ios::end,std::ios::in);
		pbuf->pubseekpos(0,std::ios::in);
		char* src=new char[size+1];
		pbuf->sgetn(src,size);
		src[size]='\0';
		source=(const char*)src;
		in.close();
		//memorystream_t in;
	// load input stream
		//in.loadfromfile(filename.c_str());
		//in.write("", 1);
	// start source off at beginnning
		//source = (const char*)in.getdata();

	// clear token list
		tokens.clear();

	// collect the tokens!
		skipWhite();
		while (*source)
		{
			tokens.push_back(collectToken());
			skipWhite();
		}
	// we shouldn't access source anywhere outside this routine
		source = NULL;

		ReSet();
	}

// validators
	bool OK()
		{ return (curtoken >= 0 && curtoken < (int)tokens.size()); }

// navigators
	void ReSet()
		{ curtoken = 0; }
	void Next()
		{ ++curtoken; }
	void Prev()
		{ --curtoken; }
	void Move(int count = 0)
		{ curtoken += count; }

// retrieval
	std::string Token()
		{ return OK() ? tokens[0+curtoken].id : ""; }
	std::string TokenInc()
		{ return OK() ? tokens[curtoken++].id : ""; }
	std::string TokenDec()
		{ return OK() ? tokens[curtoken--].id : ""; }

	void DumpTokens()
	{
	int n;
		for (n = 0; n < (int)tokens.size(); n++)
			std::cout << tokens[n].id.c_str() << '\n';
	}

	int TokenCount()
		{ return (int)tokens.size(); }
};

#endif // TOKENIZE_INC