#include <stdio.h>
#include <errno.h>
#include <wx/string.h>
#include "tokendb.h"

#include "cppwordscanner.h"

char *loadFile(const char *fileName);

int main(int argc, char **argv)
{
	CppTokenList l;
	printf("scan started...\n");
	CppWordScanner scanner(wxT("../../../LiteEditor/manager.cpp")) ;
	
	TokenDb db;
	db.Open(wxT("token.db"));
	db.BeginTransaction();
	
	// set a database to keep the records
	scanner.SetDatabase( &db );
	scanner.Match(wxT("name"), l);
	
	db.Commit();
	
	printf("Word count: %d\n", l.size());
	return 0;
}
