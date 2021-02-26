#include "CallFrame.h"
#include "CallFrameScope.h"
#include "NodeDebugger.h"
#include "NodeDebuggerPane.h"
#include "NodeDebuggerTooltip.h"
#include "NodeFileManager.h"
#include "NodeJSDebuggerBreakpoint.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "PropertyDescriptor.h"
#include "bitmap_loader.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"
#include "wxterminal.h"
#include <imanager.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

class LocalTreeItemData : public wxTreeItemData
{
    wxString m_data;

public:
    LocalTreeItemData(const wxString& data)
        : m_data(data)
    {
    }
    virtual ~LocalTreeItemData() {}
    void SetData(const wxString& data) { this->m_data = data; }
    const wxString& GetData() const { return m_data; }
};

NodeDebuggerPane::NodeDebuggerPane(wxWindow* parent)
    : NodeJSCliDebuggerPaneBase(parent)
{
    // Terminal for stdout/stdin with the running JS application
    m_terminal = new wxTerminal(m_panelOutput);
    m_terminal->SetInteractive(true);
    m_panelOutput->GetSizer()->Add(m_terminal, 1, wxEXPAND);

    // Console for real-time evaluating with Node.js
    m_node_console = new wxTerminal(m_panelConsole);
    m_node_console->SetInteractive(true);
    m_panelConsole->GetSizer()->Add(m_node_console, 1, wxEXPAND);

    m_treeCtrlLocals->AddHeader("Name");
    m_treeCtrlLocals->AddHeader("Value");
    m_terminal->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeDebuggerPane::OnRunTerminalCommand, this);
    m_node_console->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeDebuggerPane::OnEval, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_CONSOLE, &NodeDebuggerPane::OnConsoleOutput, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeDebuggerPane::OnUpdateBacktrace, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &NodeDebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeDebuggerPane::OnMarkLine, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeDebuggerPane::OnInteract, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, &NodeDebuggerPane::OnUpdateBreakpoints,
                               this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EVAL_RESULT, &NodeDebuggerPane::OnEvalResult, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_CREATE_OBJECT, &NodeDebuggerPane::OnCreateObject, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeDebuggerPane::OnDebugSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_TOOLTIP_DESTROY, &NodeDebuggerPane::OnDestroyTip, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_LOCAL_OBJECT_PROPERTIES, &NodeDebuggerPane::OnLocalProperties,
                               this);
    m_dvListCtrlCallstack->SetSortFunction(nullptr);
    m_dvListCtrlBreakpoints->SetSortFunction(nullptr);

    DoPrintStartupMessages();
    auto images = m_tbBreakpoints->GetBitmapsCreateIfNeeded();
    m_tbBreakpoints->AddTool(wxID_CLEAR, _("Clear all breakpoints"), images->Add("clean"));
    m_tbBreakpoints->AddTool(wxID_REMOVE, _("Delete breakpoint"), images->Add("minus"));
    m_tbBreakpoints->Realize();

    m_tbBreakpoints->Bind(wxEVT_TOOL, &NodeDebuggerPane::OnClearAllBreakpoints, this, wxID_CLEAR);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &NodeDebuggerPane::OnClearAllBreakpointsUI, this, wxID_CLEAR);
    m_tbBreakpoints->Bind(wxEVT_TOOL, &NodeDebuggerPane::OnDeleteBreakpoint, this, wxID_REMOVE);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &NodeDebuggerPane::OnDeleteBreakpointUI, this, wxID_REMOVE);
}

NodeDebuggerPane::~NodeDebuggerPane()
{
    DoDestroyTip();

    m_terminal->Unbind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeDebuggerPane::OnRunTerminalCommand, this);
    m_node_console->Unbind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeDebuggerPane::OnEval, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_CONSOLE, &NodeDebuggerPane::OnConsoleOutput, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeDebuggerPane::OnUpdateBacktrace, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STOPPED, &NodeDebuggerPane::OnDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_MARK_LINE, &NodeDebuggerPane::OnMarkLine, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeDebuggerPane::OnInteract, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, &NodeDebuggerPane::OnUpdateBreakpoints,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EVAL_RESULT, &NodeDebuggerPane::OnEvalResult, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_CREATE_OBJECT, &NodeDebuggerPane::OnCreateObject, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeDebuggerPane::OnDebugSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_TOOLTIP_DESTROY, &NodeDebuggerPane::OnDestroyTip, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_LOCAL_OBJECT_PROPERTIES, &NodeDebuggerPane::OnLocalProperties,
                                 this);
}

void NodeDebuggerPane::OnUpdateBacktrace(clDebugCallFramesEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_dvListCtrlCallstack);

    // Keep the frames
    m_frames = event.GetCallFrames();
    m_dvListCtrlCallstack->DeleteAllItems();

    for(size_t i = 0; i < m_frames.size(); ++i) {
        CallFrame* frame = m_frames[i]->To<CallFrame>();

        wxVector<wxVariant> cols;
        wxString filename = NodeFileManager::Get().GetFilePath(frame->GetLocation().GetScriptId());
        cols.push_back(wxString() << "#" << i);
        cols.push_back(frame->GetFunctionName());
        cols.push_back(filename);
        cols.push_back(wxString() << (frame->GetLocation().GetLineNumber() + 1));
        m_dvListCtrlCallstack->AppendItem(cols, (wxUIntPtr) new wxStringClientData(frame->GetCallFrameId()));
        if(i == 0) {
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            event.SetLineNumber(frame->GetLocation().GetLineNumber() + 1);
            event.SetFileName(filename);
            EventNotifier::Get()->AddPendingEvent(event);

            // Update the locals view
            DoUpdateLocalsView(frame);
            NodeJSWorkspace::Get()->GetDebugger()->SetActiveFrame(frame->GetCallFrameId());
        }
    }
}

void NodeDebuggerPane::OnDebuggerStopped(clDebugEvent& event)
{
    event.Skip();
    m_dvListCtrlCallstack->DeleteAllItems([](wxUIntPtr data) {
        wxStringClientData* scd = reinterpret_cast<wxStringClientData*>(data);
        wxDELETE(scd);
    });
    m_dvListCtrlBreakpoints->DeleteAllItems([](wxUIntPtr data) {
        wxStringClientData* scd = reinterpret_cast<wxStringClientData*>(data);
        wxDELETE(scd);
    });
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    DoDestroyTip();

    // Clear the locals view
    m_treeCtrlLocals->DeleteAllItems();
    m_localsPendingItems.clear();

    // Clear the frames
    m_frames.clear();
}

void NodeDebuggerPane::OnMarkLine(clDebugEvent& event)
{
    event.Skip();
    NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    NodeJSWorkspace::Get()->GetDebugger()->SetDebuggerMarker(event.GetFileName(), event.GetLineNumber());
}

void NodeDebuggerPane::OnUpdateBreakpoints(clDebugEvent& event)
{
    event.Skip();
    m_dvListCtrlBreakpoints->DeleteAllItems([](wxUIntPtr data) {
        wxStringClientData* scd = reinterpret_cast<wxStringClientData*>(data);
        wxDELETE(scd);
    });
    const NodeJSBreakpoint::Vec_t& bps = NodeJSWorkspace::Get()->GetDebugger()->GetBreakpointsMgr()->GetBreakpoints();
    for(size_t i = 0; i < bps.size(); ++i) {
        const NodeJSBreakpoint& bp = bps[i];
        wxVector<wxVariant> cols;
        cols.push_back(bp.GetFilename());
        cols.push_back(wxString() << bp.GetLine());
        m_dvListCtrlBreakpoints->AppendItem(cols, (wxUIntPtr) new wxStringClientData(bp.GetNodeBpID()));
    }
}

void NodeDebuggerPane::OnInteract(clDebugEvent& event)
{
    event.Skip();
    if(!event.IsAnswer()) {
        m_dvListCtrlCallstack->DeleteAllItems();
        NodeJSWorkspace::Get()->GetDebugger()->ClearDebuggerMarker();
    }
    if(event.GetString() == "exception") {
        // The debugger paused of an uncaught exception
        ::wxMessageBox(_("Node.js: uncaught exception!"), "Node.js", wxICON_ERROR | wxCENTRE,
                       EventNotifier::Get()->TopFrame());
        if(!event.GetArguments().IsEmpty()) {
            m_terminal->AddTextWithEOL(event.GetArguments());
            SelectTab(_("Stdin / Stdout"));
        }
    }
}

void NodeDebuggerPane::OnRunTerminalCommand(clCommandEvent& event)
{
    // Dont call event.Skip() here
    wxString command = event.GetString();
    NodeJSWorkspace::Get()->GetDebugger()->SendToDebuggee(command);
}

void NodeDebuggerPane::OnConsoleOutput(clDebugEvent& event)
{
    // Dont call event.Skip() here
    m_terminal->AddTextRaw(event.GetString());
}

void NodeDebuggerPane::OnEval(clCommandEvent& event)
{
    // We always eval on the active frame
    if(m_dvListCtrlCallstack->IsEmpty()) {
        return;
    }
    const wxString& frameId = NodeJSWorkspace::Get()->GetDebugger()->GetActiveFrame();
    if(frameId.IsEmpty()) {
        return;
    }
    NodeJSWorkspace::Get()->GetDebugger()->Eval(event.GetString(), frameId);
}

void NodeDebuggerPane::OnEvalResult(clDebugRemoteObjectEvent& event)
{
    RemoteObject* pro = event.GetRemoteObject()->To<RemoteObject>();
    m_node_console->AddTextRaw(pro->ToString() + "\n");
}

void NodeDebuggerPane::OnCreateObject(clDebugRemoteObjectEvent& event)
{
    nSerializableObject::Ptr_t o = event.GetRemoteObject();
    if(!m_debuggerTooltip) {
        m_debuggerTooltip = new NodeDebuggerTooltip(this);
    }
    m_debuggerTooltip->Show(o);
}

void NodeDebuggerPane::OnDestroyTip(clCommandEvent& event)
{
    event.Skip();
    DoDestroyTip();
}

void NodeDebuggerPane::DoDestroyTip()
{
    if(m_debuggerTooltip) {
        m_debuggerTooltip->Hide();
        m_debuggerTooltip->Destroy();
    }
    m_debuggerTooltip = nullptr;
}

void NodeDebuggerPane::OnDebugSessionStarted(clDebugEvent& event)
{
    event.Skip();
    DoPrintStartupMessages();
}

void NodeDebuggerPane::DoPrintStartupMessages()
{
    m_node_console->Clear();
    m_terminal->Clear();
    m_node_console->AddTextWithEOL("##==========================================================");
    m_node_console->AddTextWithEOL("## Node.js console");
    m_node_console->AddTextWithEOL("## use this console to send commands directly to node.js");
    m_node_console->AddTextWithEOL("##==========================================================");

    m_terminal->AddTextWithEOL("##==========================================================");
    m_terminal->AddTextWithEOL("## Node.js stdin/stdout console");
    m_terminal->AddTextWithEOL("## stdout messages (e.g. console.log(..) will appear here");
    m_terminal->AddTextWithEOL("##==========================================================");
}

void NodeDebuggerPane::OnClearAllBreakpoints(wxCommandEvent& event)
{
    wxUnusedVar(event);
    NodeJSWorkspace::Get()->GetDebugger()->DeleteAllBreakpoints();
}

void NodeDebuggerPane::OnClearAllBreakpointsUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_dvListCtrlBreakpoints->IsEmpty());
}

void NodeDebuggerPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrlBreakpoints->GetSelection();
    CHECK_ITEM_RET(item);

    wxStringClientData* cd = reinterpret_cast<wxStringClientData*>(m_dvListCtrlBreakpoints->GetItemData(item));
    if(cd) {
        NodeJSWorkspace::Get()->GetDebugger()->DeleteBreakpointByID(cd->GetData());
    }
}

void NodeDebuggerPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlBreakpoints->GetSelection().IsOk());
}

void NodeDebuggerPane::DoUpdateLocalsView(CallFrame* callFrame)
{
    m_treeCtrlLocals->DeleteAllItems();
    if(!callFrame) {
        return;
    }
    wxTreeItemId root = m_treeCtrlLocals->AddRoot("Locals");
    const nSerializableObject::Vec_t& chain = callFrame->GetScopeChain();
    for(size_t i = 0; i < chain.size(); ++i) {
        CallFrameScope* scope = chain[i]->To<CallFrameScope>();
        wxString name = scope->GetDisplayName();
        const wxString& objectId = scope->GetRemoteObject().GetObjectId();
        wxTreeItemId scopeItemId =
            m_treeCtrlLocals->AppendItem(root, name, wxNOT_FOUND, wxNOT_FOUND, new LocalTreeItemData(objectId));
        if(scope->GetRemoteObject().IsObject() && !objectId.IsEmpty()) {
            m_treeCtrlLocals->AppendItem(scopeItemId, "Loading...");

            // Dont expand the "global" scope
            if(name != "global") {
                m_treeCtrlLocals->Expand(scopeItemId);
            }
            m_localsPendingItems.insert({ objectId, scopeItemId });

            // Request the properties for this object from the debugger
            NodeJSWorkspace::Get()->GetDebugger()->GetObjectProperties(objectId,
                                                                       wxEVT_NODEJS_DEBUGGER_LOCAL_OBJECT_PROPERTIES);
        }
    }
}

void NodeDebuggerPane::OnLocalProperties(clDebugEvent& event)
{
    wxString objectId = event.GetStartupCommands();
    if(m_localsPendingItems.count(objectId) == 0) {
        return;
    }
    wxTreeItemId item = m_localsPendingItems[objectId];
    m_localsPendingItems.erase(objectId);
    m_treeCtrlLocals->DeleteChildren(item);
    // Get the output result (an array of PropertyDescriptor)
    wxString s = event.GetString();
    JSON root(s);
    JSONItem prop_arr = root.toElement();
    int size = prop_arr.arraySize();
    std::vector<PropertyDescriptor> propVec;
    for(int i = 0; i < size; ++i) {
        JSONItem prop = prop_arr.arrayItem(i);
        PropertyDescriptor propDesc;
        propDesc.FromJSON(prop);
        if(!propDesc.IsEmpty()) {
            propVec.push_back(propDesc);
        }
    }

    for(size_t i = 0; i < propVec.size(); ++i) {
        const PropertyDescriptor& prop = propVec[i];
        wxTreeItemId child = m_treeCtrlLocals->AppendItem(item, prop.GetName());
        m_treeCtrlLocals->SetItemText(child, prop.GetTextPreview(), 1);
        m_treeCtrlLocals->SetItemData(child, new LocalTreeItemData(prop.GetValue().GetObjectId()));
        if(prop.HasChildren()) {
            m_treeCtrlLocals->AppendItem(child, "<dummy>");
        }
    }
}
void NodeDebuggerPane::OnLocalExpanding(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    wxString object_id = GetLocalObjectItem(item);
    if(object_id.IsEmpty()) {
        m_treeCtrlLocals->DeleteChildren(item);
    } else {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeCtrlLocals->GetFirstChild(item, cookie);
        if(m_treeCtrlLocals->GetItemText(child) == "<dummy>") {
            m_treeCtrlLocals->SetItemText(child, "Loading...");
            m_localsPendingItems.insert({ object_id, item });

            // Request the properties for this object from the debugger
            NodeJSWorkspace::Get()->GetDebugger()->GetObjectProperties(object_id,
                                                                       wxEVT_NODEJS_DEBUGGER_LOCAL_OBJECT_PROPERTIES);
        }
    }
}

wxString NodeDebuggerPane::GetLocalObjectItem(const wxTreeItemId& item) const
{
    if(item.IsOk() == false) {
        return "";
    }
    LocalTreeItemData* d = dynamic_cast<LocalTreeItemData*>(m_treeCtrlLocals->GetItemData(item));
    if(!d) {
        return "";
    }
    return d->GetData();
}

void NodeDebuggerPane::OnStackEntryActivated(wxDataViewEvent& event)
{
    // Set a new active frame
    wxStringClientData* d = reinterpret_cast<wxStringClientData*>(m_dvListCtrlCallstack->GetItemData(event.GetItem()));
    CHECK_PTR_RET(d);

    CallFrame* frame = GetFrameById(d->GetData());
    CHECK_PTR_RET(frame);

    NodeJSWorkspace::Get()->GetDebugger()->SetActiveFrame(d->GetData());

    // Open the current file and line and
    // Set the debugger marker on this line
    wxString file_path = m_dvListCtrlCallstack->GetItemText(event.GetItem(), 2);
    wxString line_number = m_dvListCtrlCallstack->GetItemText(event.GetItem(), 3);

    long nLineNumber = 0;
    line_number.ToCLong(&nLineNumber);
    NodeJSWorkspace::Get()->GetDebugger()->SetDebuggerMarker(file_path, nLineNumber);

    // Update the locals view to reflect the current frame
    DoUpdateLocalsView(frame);
}

CallFrame* NodeDebuggerPane::GetFrameById(const wxString& frameId) const
{
    for(size_t i = 0; i < m_frames.size(); ++i) {
        CallFrame* frame = m_frames[i]->To<CallFrame>();
        if(frame->GetCallFrameId() == frameId) {
            return frame;
        }
    }
    return nullptr;
}
void NodeDebuggerPane::OnStackContextMenu(wxDataViewEvent& event)
{
    wxMenu m;
    m.Append(XRCID("node-copy-backtrace"), _("Copy Backtrace"));
    m.Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event) {
            wxString backtrace;
            for(size_t i = 0; i < m_dvListCtrlCallstack->GetItemCount(); ++i) {
                wxString line;
                wxDataViewItem item = m_dvListCtrlCallstack->RowToItem(i);
                for(size_t col = 0; col < 4; ++col) {
                    line << m_dvListCtrlCallstack->GetItemText(item, col) << " ";
                }
                backtrace << line << "\n";
            }
            ::CopyToClipboard(backtrace);
        },
        XRCID("node-copy-backtrace"));
    m_dvListCtrlCallstack->PopupMenu(&m);
}

void NodeDebuggerPane::SelectTab(const wxString& label)
{
    // Select the terminal tab
    int tabIndex = m_notebook->GetPageIndex(label);
    if(tabIndex != wxNOT_FOUND) {
        m_notebook->SetSelection(tabIndex);
    }
}
