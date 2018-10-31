#ifndef NODEJSCLIDEBUGGERFRAMEENTRY_H
#define NODEJSCLIDEBUGGERFRAMEENTRY_H

#include <wx/string.h>
class NodeJSCLIDebuggerFrameEntry
{
    wxString m_file;
    long m_lineNumber = 0;

public:
    NodeJSCLIDebuggerFrameEntry(const wxString& rawline, const wxString& workingDirectory);
    virtual ~NodeJSCLIDebuggerFrameEntry();
    bool IsOk() const { return !m_file.IsEmpty(); }
    void SetFile(const wxString& file) { this->m_file = file; }
    void SetLineNumber(long lineNumber) { this->m_lineNumber = lineNumber; }
    const wxString& GetFile() const { return m_file; }
    long GetLineNumber() const { return m_lineNumber; }
};

#endif // NODEJSCLIDEBUGGERFRAMEENTRY_H
