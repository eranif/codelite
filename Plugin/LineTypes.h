#ifndef LINETYPES_H_INCLUDED
#define LINETYPES_H_INCLUDED

#include "wx/string.h"
#include <vector>

namespace LINETYPES
{
	/**
	 * Different line types known.
	 */ 
	enum LineType
	{
		LINE_EMPTY, // zero line, or whitespace / tabs only
		LINE_TARGET, // target: object.o '\'
		LINE_DEPEND, // bla \<newline> bla
		LINE_ACTION, // gcc -c ...
		LINE_COMMENT, // # ...
		LINE_COMMAND, // makefile command
		LINE_INVALID, // No clue
	};
	
	/**
	* The 'type' names of what type 'line' is.
	*/ 
	struct TypedString
	{
		LineType type;
		wxString line;
	};	

};

typedef struct LINETYPES::TypedString TypedString;
typedef std::vector<TypedString> TypedStrings;

#endif // LINETYPES_H_INCLUDED
