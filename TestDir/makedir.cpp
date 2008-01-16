#include "wx/filefn.h"
#include "wx/dir.h"
#include <wx/init.h>	//wxInitializer
#include <wx/string.h>	//wxString
#include "wx/cmdline.h"

static const wxCmdLineEntryDesc cmdLineDesc[] = 
{
	{wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
	{wxCMD_LINE_NONE }
};


int main(int argc, char **argv) 
{
	//Initialize the wxWidgets library
	wxInitializer initializer;
	
	//parse the input
	wxCmdLineParser parser;
	parser.SetDesc(cmdLineDesc);
	parser.SetCmdLine(argc, argv);
	if (parser.Parse() != 0) {
		return -1;
	}

	for (size_t i=0; i< parser.GetParamCount(); i++) {
		wxString argument = parser.GetParam(i);
		if( !wxDir::Exists(argument) ){
			wxMkdir(argument, 0777);
		}
	}

	return 0;
}
