#include "static.h"

wxString stvariables::dotpngname;
wxString stvariables::dottxtname;
wxString stvariables::dotfilesdir;
wxString stvariables::gmonfile;
wxString stvariables::filetype;
wxString stvariables::gprofname;
wxString stvariables::dotname;

wxString stvariables::sd;
wxString stvariables::sw;
wxString stvariables::sq;


void stvariables::InicializeStatic()
{
	sq = wxT("\"");
	sw = wxT(" ");
	dotpngname = wxT("dot.png");
	dottxtname = wxT("dot.txt");
	gmonfile = wxT("gmon.out");
	dotfilesdir = wxT("CallGraph");
	sd = wxFileName::GetPathSeparator(); 
	
#ifdef __WXMSW__
	gprofname = wxT("gprof.exe");
	dotname = wxT("dot.exe");
	filetype = wxT(".exe");
#else
	gprofname = wxT("gprof");
	dotname = wxT("dot");
	filetype = wxT("");	
#endif 
}

