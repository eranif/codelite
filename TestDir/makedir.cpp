#include "wx/cmdline.h"
#include "wx/dir.h"
#include "wx/filefn.h"

#include <wx/arrstr.h>
#include <wx/init.h> //wxInitializer
#include <wx/log.h>
#include <wx/string.h> //wxString
#include <wx/tokenzr.h>

#if wxVERSION_NUMBER < 2900
static const wxCmdLineEntryDesc cmdLineDesc[] = { { wxCMD_LINE_PARAM, NULL, NULL, wxT("Directory name"),
                                                    wxCMD_LINE_VAL_STRING,
                                                    wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
                                                  { wxCMD_LINE_NONE } };
#else
static const wxCmdLineEntryDesc cmdLineDesc[] = { { wxCMD_LINE_PARAM, NULL, NULL, "Directory name",
                                                    wxCMD_LINE_VAL_STRING,
                                                    wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
                                                  { wxCMD_LINE_NONE } };
#endif

int main(int argc, char** argv)
{
    // Initialize the wxWidgets library
    wxInitializer initializer;
    wxLog::EnableLogging(false);

    // parse the input
    wxCmdLineParser parser;
    parser.SetCmdLine(argc, argv);
    parser.SetDesc(cmdLineDesc);

    if(parser.Parse() != 0) {
        return -1;
    }

    for(size_t i = 0; i < parser.GetParamCount(); i++) {
        wxString argument = parser.GetParam(i);
        if(!wxDir::Exists(argument)) {
            argument.Replace(wxT("\\"), wxT("/"));
            wxArrayString arr = wxStringTokenize(argument, wxT("/"), wxTOKEN_STRTOK);
            wxString path;
            for(size_t i = 0; i < arr.GetCount(); i++) {
                path << arr.Item(i) << wxT("/");
                wxMkdir(path, 0777);
            }
        }
    }

    return 0;
}
