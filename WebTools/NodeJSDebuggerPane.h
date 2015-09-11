#ifndef NODEJSDEBUGGERPANE_H
#define NODEJSDEBUGGERPANE_H

#include "WebToolsBase.h"
#include "NodeJSEvents.h"
#include <map>
#include "json_node.h"
#include "NodeJSOuptutParser.h"

class NodeJSDebuggerPane : public NodeJSDebuggerPaneBase
{
    struct FrameData
    {
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

public:
    std::map<int, NodeJSHandle> m_handles;
    std::vector<PendingLookupDV> m_pendingLookupRefs;

protected:
    virtual void OnLocalExpanding(wxDataViewEvent& event);
    virtual void OnEvaluateExpression(wxCommandEvent& event);
    virtual void OnBreakpointSelected(wxDataViewEvent& event);
    void ClearCallstack();
    void BuildLocals(const JSONElement& json);
    void BuildArguments(const JSONElement& json);
    wxDataViewItem AddLocal(const wxDataViewItem& parent, const wxString& name, int refId);
    void ParseRefsArray(const JSONElement& refs);
    NodeJSHandle ParseRef(const JSONElement& ref);
    void DoOpenFile(const wxString& filename, int line);
    void DoDeleteLocalItemAfter(const wxDataViewItem& item);
    void DoAddKnownRefs(const std::map<int, wxString>& refs, const wxDataViewItem& parent);
    void DoAddUnKnownRefs(const std::map<int, wxString>& refs, const wxDataViewItem& parent);
    void Clear();

protected:
    void OnItemActivated(wxDataViewEvent& event);
    void OnUpdateCallstack(clDebugEvent& event);
    void OnExpressionEvaluated(clDebugEvent& event);
    void OnLostControl(clDebugEvent& event);
    void OnLookup(clDebugEvent& event);
    void OnConsoleLog(clDebugEvent& event);
    void OnSessionStarted(clDebugEvent& event);
    void OnSessionStopped(clDebugEvent& event);
    void OnExceptionThrown(clDebugEvent& event);
    void OnUpdateDebuggerView(clDebugEvent& event);
    void OnFrameSelected(clDebugEvent& event);

public:
    NodeJSDebuggerPane(wxWindow* parent);
    virtual ~NodeJSDebuggerPane();
};
#endif // NODEJSDEBUGGERPANE_H
