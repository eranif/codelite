#include "NodeJSCLIDebuggerOutputParser.h"
#include "file_logger.h"
#include <wx/regex.h>

NodeJSCLIDebuggerOutputParser::NodeJSCLIDebuggerOutputParser() {}

NodeJSCLIDebuggerOutputParser::~NodeJSCLIDebuggerOutputParser() {}

void NodeJSCLIDebuggerOutputParser::ParseOutput(const wxString& outputString)
{
    wxString s = outputString;
    PrepareOutput(s);
    while(!s.IsEmpty()) {
        switch(m_state) {
        case kStateWaitingPrefix:
            CheckForPrefix(s);
            break;
        case kStateWaitingSuffix:
            CheckForSuffix(s);
            break;
        }
    }
}

bool NodeJSCLIDebuggerOutputParser::CheckForPrefix(wxString& outputString)
{
    static wxRegEx rePrefix("#start_command_([0-9]+)");
    //clDEBUG() << "CheckForPrefix is called with:\n" << outputString;
    m_output.Clear();
    if(rePrefix.Matches(outputString)) {
        long commandId;
        wxString command_number = rePrefix.GetMatch(outputString, 1);
        wxString full_match = rePrefix.GetMatch(outputString, 0);
        command_number.ToCLong(&commandId);
        m_commandID = commandId;
        m_state = kStateWaitingSuffix;
        clDEBUG() << "Waiting for command output...";
        // Remove the matched string
        int where = outputString.Find(full_match);
        outputString = outputString.Mid(where + full_match.length());
        return true;
    } else {
        // No match at all, clear the output
        outputString.Clear();
    }
    return false;
}

long NodeJSCLIDebuggerOutputParser::CheckForSuffix(wxString& outputString)
{
    static wxRegEx reSuffix("#end_command_([0-9]+)");
    clDEBUG() << "CheckForSuffix is called with:\n" << outputString;
    if(reSuffix.Matches(outputString)) {
        long matchedCommandID = m_commandID;
        wxString full_match = reSuffix.GetMatch(outputString, 0);
        int where = outputString.Find(full_match);
        if(where != 0) {
            // append anything up until where to the output
            m_output << outputString.Mid(0, where);
        }
        
        // Remove CLI debugger prompt
        m_output.Replace("debug>", "");
        
        m_results.push_back({ m_commandID, m_output });
        m_commandID = wxNOT_FOUND;
        m_state = kStateWaitingPrefix;
        m_output.Clear();
        outputString = outputString.Mid(where + full_match.length());
        return matchedCommandID;
    } else {
        // Concatenate the output
        m_output << outputString;
        outputString.Clear();
        return wxNOT_FOUND;
    }
}

void NodeJSCLIDebuggerOutputParser::Clear()
{
    m_results.clear();
    m_commandID = wxNOT_FOUND;
    m_output.Clear();
    m_state = kStateWaitingPrefix;
}

std::pair<long, wxString> NodeJSCLIDebuggerOutputParser::TakeResult()
{
    if(m_results.empty()) { return { wxNOT_FOUND, "" }; }
    std::pair<long, wxString> res = (*m_results.begin());
    m_results.erase(m_results.begin());
    return res;
}

void NodeJSCLIDebuggerOutputParser::PrepareOutput(wxString& outputString)
{
    wxString fixedString;
    for(size_t i = 0; i < outputString.size(); ++i) {
        wxChar ch = outputString[i];
        switch(ch) {
        case '\r': {
            // Unwind back and delete everything until the first '\n' we find or start of string
            size_t where = fixedString.rfind('\n');
            if(where != wxString::npos) {
                fixedString.Truncate(where + 1);
            } else {
                fixedString.Clear();
            }
        } break;
        case '\b': {
            if(!fixedString.IsEmpty() && (fixedString.Last() != '\n')) {
                // remove last char
                fixedString.Truncate(fixedString.size() - 1);
            }

        } break;
        case '\n': {
            fixedString << ch;

        } break;
        default: {
            fixedString << ch;

        } break;
        }
    }
    outputString.swap(fixedString);
}
