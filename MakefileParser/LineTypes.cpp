#include "LineTypes.h"

namespace LINETYPES
{
	wxString toString(const LineType& type)
	{
		wxString result;
		
		switch(type)
		{
			default:
				result = wxT("Not Set");
				break;
			case LINE_EMPTY: // zero line, or whitespace / tabs only
				result = wxT("Empty Line");
				break;
			case LINE_TARGET: // target: object.o '\'
				result = wxT("Target");
				break;
			case LINE_DEPEND: // bla \<newline> bla
				result = wxT("Dependency");
				break;
			case LINE_ACTION: // gcc -c ...
				result = wxT("Action");
				break;
			case LINE_COMMENT: // # ...
				result = wxT("Comment");
				break;
			case LINE_COMMAND: // makefile command
				result = wxT("Makefile command");
				break;
			case LINE_INVALID: // No clue
				result = wxT("Unknown");
				break;
		}
		
		return result;
	}
}
