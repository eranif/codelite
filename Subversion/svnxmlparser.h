#ifndef SVNXMLPARSER_H
#define SVNXMLPARSER_H

#include "wx/string.h"
#include "wx/arrstr.h"

class SvnXmlParser 
{
public:
	enum FileState{
		//wc-status: conflict
		StateConflict		= 0x00000001,		

		//any state that is considered as modified, such:
		//wc-status: deleted, modified, added, merged
		StateModified		= 0x00000002,

		//wc-status: unversioned
		StateUnversioned	= 0x00000004
	};

public:
	SvnXmlParser();
	~SvnXmlParser();

	static void GetFiles(const wxString &input, wxArrayString &files, FileState state = StateModified);
};

#endif //SVNXMLPARSER_H

