#include "NodeJSCLIDebuggerFrameEntry.h"
#include <wx/filename.h>

NodeJSCLIDebuggerFrameEntry::NodeJSCLIDebuggerFrameEntry(const wxString& rawline, const wxString& workingDirectory)
{
    wxString remainder = rawline;
    remainder.Replace("(anonymous)", "");
    // remainder => <file>:line:col
    wxString col = remainder.AfterLast(':');
    remainder = remainder.BeforeLast(':');
    // remainder => <file>:line
    wxString line = remainder.AfterLast(':');
    m_file = remainder.BeforeLast(':');
    m_file.Trim().Trim(false);
    line.ToCLong(&m_lineNumber);
    wxFileName fn(m_file);
    if(fn.IsAbsolute()) {
        m_file = fn.GetFullPath();
    } else {
        fn.MakeAbsolute(workingDirectory);
        m_file = fn.GetFullPath();
    }
}

NodeJSCLIDebuggerFrameEntry::~NodeJSCLIDebuggerFrameEntry() {}
