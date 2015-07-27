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
    virtual void OnEvaluateExpression(wxCommandEvent& event);
    virtual void OnBreakpointSelected(wxDataViewEvent& event);
    void ClearCallstack();
    void BuildLocals(const JSONElement& json);
    void BuildArguments(const JSONElement& json);
    void AddLocal(wxDataViewItem& parent, const wxString& name, int refId, int depth);
    void ParseRefsArray(const JSONElement& refs);
    void DoOpenFile(const wxString& filename, int line);

protected:
    void OnItemActivated(wxDataViewEvent& event);
    void OnUpdateCallstack(clDebugEvent& event);
    void OnExpressionEvaluated(clDebugEvent& event);
    void OnLostControl(clDebugEvent& event);
    void OnConsoleLog(clDebugEvent& event);
    void OnSessionStarted(clDebugEvent& event);
    void OnExceptionThrown(clDebugEvent& event);
    void OnUpdateDebuggerView(clDebugEvent& event);
    void OnFrameSelected(clDebugEvent& event);

public:
    NodeJSDebuggerPane(wxWindow* parent);
    virtual ~NodeJSDebuggerPane();
};
#endif // NODEJSDEBUGGERPANE_H
