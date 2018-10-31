#ifndef NODEJSCLIDEBUGGEROUTPUTPARSER_H
#define NODEJSCLIDEBUGGEROUTPUTPARSER_H

#include <vector>
#include <wx/string.h>

class NodeJSCLIDebuggerOutputParser
{
    enum eDebuggerState {
        kStateWaitingPrefix,
        kStateWaitingSuffix,
    };
    eDebuggerState m_state = kStateWaitingPrefix;
    wxString m_output;
    long m_commandID = wxNOT_FOUND;
    std::vector<std::pair<long, wxString> > m_results;

protected:
    bool CheckForPrefix(wxString& outputString);
    long CheckForSuffix(wxString& outputString);
    void PrepareOutput(wxString& outputString);

public:
    NodeJSCLIDebuggerOutputParser();
    virtual ~NodeJSCLIDebuggerOutputParser();
    /**
     * @brief process output from the debugger CLI. If a completed entry output is found, return the command ID
     * that this output belongs to, otherwise return wxNOT_FOUND
     */
    void ParseOutput(const wxString& outputString);
    bool HasResults() const { return !m_results.empty(); }
    std::pair<long, wxString> TakeResult();
    void Clear();
};

#endif // NODEJSCLIDEBUGGEROUTPUTPARSER_H
