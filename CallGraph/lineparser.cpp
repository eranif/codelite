#include "lineparser.h"

#include <wx/listimpl.cpp>

WX_DEFINE_LIST(LineParserList);

LineParser* LineParser::Clone()
{
	LineParser *newline = new LineParser();
	
	newline->index = this->index;
	newline->time = this->time;
	newline->self = this->self;
	newline->children = this->children;
	newline->called0 = this->called0;
	newline->called1 = this->called1;
	newline->name = this->name;
	newline->nameid = this->nameid;
	newline->parents = this->parents; 
	newline->pline = this->pline;
	newline->child = this->child;
	newline->cycle = this->cycle; 
	newline->recursive = this->recursive;
	newline->cycleid = this->cycleid;
	
	return newline;
}
