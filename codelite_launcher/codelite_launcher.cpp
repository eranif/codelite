#include <wx/cmdline.h>
#include <wx/crt.h>
#include <wx/process.h>
#include <wx/string.h>

const wxCmdLineEntryDesc gCmdLineDesc[] = {
    { wxCMD_LINE_OPTION, "n", "name", "Executable to launch", wxCMD_LINE_VAL_STRING }, { wxCMD_LINE_NONE }
};

bool Process(wxCmdLineParser& parser);
void PrintUsage();

int main(int argc, char** argv)
{
    wxCmdLineParser parser(argc, argv);
    parser.SetDesc(gCmdLineDesc);
    parser.SetSwitchChars(wxT("-"));

    Process(parser);
    return 0;
}

bool Process(wxCmdLineParser& parser)
{
    parser.Parse();

    //	long     process_id   (wxNOT_FOUND);
    wxString exe_name(wxEmptyString);

    // We got a command to close eTrigger
    //	parser.Found(wxT("pid"),  &process_id);
    parser.Found(wxT("name"), &exe_name);

    if(/*process_id == wxNOT_FOUND || */ exe_name.IsEmpty()) {
        PrintUsage();
        return false;
    }

    // Kill the process
    //	if (wxProcess::Exists(process_id)) {
    //
    //		//Okay, then let's close parent.exe
    //		int killError = wxProcess::Kill(process_id, wxSIGTERM);
    //		if (killError) {
    //			if (killError == wxKILL_BAD_SIGNAL) {
    //				wxPrintf(wxT("wxProcess::Kill Error: no such signal\n"));
    //				return true;
    //			}
    //			if (killError == wxKILL_ACCESS_DENIED) {
    //				wxPrintf(wxT("wxProcess::Kill Error: permission denied\n"));
    //				return true;
    //			}
    //			if (killError == wxKILL_NO_PROCESS) {
    //				wxPrintf(wxT("wxProcess::Kill Error: no such process\n"));
    //				return true;
    //			}
    //			if (killError == wxKILL_ERROR) {
    //				wxPrintf(wxT("An Error occurred attempting to close parent.exe, the helper will now attempt to forcefully
    //close %s\n"), exe_name.c_str()); 				int killError2 = wxProcess::Kill(process_id, wxSIGKILL);
    //				if (killError2) {
    //					wxPrintf(wxT("The Helper could not close %s\n"), exe_name.c_str());
    //					return true;
    //				}
    //			}
    //		}
    //	} else {
    //		wxPrintf(wxT("Error: process %d does not exist\n"), process_id);
    //	}

    // Now let's open the parent
    wxProcess proc;
    wxSleep(3);

    proc.Open(wxString::Format(wxT("%s"), exe_name.c_str()), wxEXEC_ASYNC | wxEXEC_NOHIDE);
    proc.Detach();
    return true;
}

void PrintUsage()
{
    wxPrintf(wxT("codelite_restarter --pid=<number> --name=<string>\n"));
    wxPrintf(wxT("--pid,-p           Process ID\n"));
    wxPrintf(wxT("--name,-n          Executable to launch\n"));
}
