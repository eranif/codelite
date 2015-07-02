#ifndef NODEJSDEBUGGERPANE_H
#define NODEJSDEBUGGERPANE_H

#include "WebToolsBase.h"
#include "NodeJSEvents.h"
#include <map>
#include "json_node.h"

class NodeJSDebuggerPane : public NodeJSDebuggerPaneBase
{
    struct FrameData {
        int index;
        int line;
        wxString file;
        wxString function;
        FrameData()
            : index(wxNOT_FOUND)
            , line(wxNOT_FOUND)
        {
        }
    };
    
    struct Handle {
        wxString name;
        wxString value;
        wxString type;
        std::map<int, wxString> properties;
    };
    
    std::map<int, Handle> m_handles;

protected:
    void ClearCallstack();
    void BuildLocals(const JSONElement& json);
    void BuildArguments(const JSONElement& json);
    void AddLocal(wxDataViewItem& parent, const wxString& name, int refId);

protected:
    void OnItemActivated(wxDataViewEvent& event);
    void OnUpdateCallstack(clDebugEvent& event);
    void OnLostControl(clDebugEvent& event);
    void OnConsoleLog(clDebugEvent& event);
    void OnSessionStarted(clDebugEvent& event);

public:
    NodeJSDebuggerPane(wxWindow* parent);
    virtual ~NodeJSDebuggerPane();
};
#endif // NODEJSDEBUGGERPANE_H
