#include "NodeJSDebuggerPane.h"
#include "event_notifier.h"
#include "json_node.h"
#include "macros.h"
#include <map>
#include "ColoursAndFontsManager.h"
#include <wx/wupdlock.h>
#include <wx/msgdlg.h>
#include "globals.h"
#include "NodeJSDebugger.h"
#include "NoteJSWorkspace.h"
#include "imanager.h"

NodeJSDebuggerPane::NodeJSDebuggerPane(wxWindow* parent)
    : NodeJSDebuggerPaneBase(parent)
{
    EventNotifier::Get()->Bind(
        wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED, &NodeJSDebuggerPane::OnExpressionEvaluated, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_LOST_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebuggerPane::OnFrameSelected, this);
    EventNotifier::Get()->Bind(
        wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, &NodeJSDebuggerPane::OnUpdateDebuggerView, this);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_consoleLog);
    }
    m_dataviewLocals->SetIndent(16);
    m_dataviewLocals->GetColumn(0)->SetWidth(150);
    m_dataviewLocals->GetColumn(1)->SetWidth(100);
    m_dataviewLocals->GetColumn(2)->SetWidth(500);

    m_dvListCtrlCallstack->GetColumn(0)->SetWidth(30);
    m_dvListCtrlCallstack->GetColumn(1)->SetWidth(200);
    m_dvListCtrlCallstack->GetColumn(2)->SetWidth(300);
    m_dvListCtrlCallstack->GetColumn(3)->SetWidth(100);
}

NodeJSDebuggerPane::~NodeJSDebuggerPane()
{
    EventNotifier::Get()->Unbind(
        wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED, &NodeJSDebuggerPane::OnExpressionEvaluated, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_LOST_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebuggerPane::OnFrameSelected, this);
    EventNotifier::Get()->Unbind(
        wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, &NodeJSDebuggerPane::OnUpdateDebuggerView, this);

    ClearCallstack();
}
void NodeJSDebuggerPane::ParseRefsArray(const JSONElement& refs)
{
    int refsCount = refs.arraySize();
    for(int i = 0; i < refsCount; ++i) {
        JSONElement ref = refs.arrayItem(i);
        int handleId = ref.namedObject("handle").toInt();
        Handle h;
        h.type = ref.namedObject("type").toString();
        if(h.type == "undefined") {
            h.value = "undefined";
        } else if(h.type == "number" || h.type == "boolean") {
            h.value = ref.namedObject("text").toString();
        } else if(h.type == "string") {
            h.value << "\"" << ref.namedObject("text").toString() << "\"";
        } else if(h.type == "script" || h.type == "function") {
            h.value = ref.namedObject("name").toString();
        } else if(h.type == "null") {
            h.value = "null";
        } else if(h.type == "object") {
            h.value = "{...}";
            JSONElement props = ref.namedObject("properties");
            int propsCount = props.arraySize();
            for(int n = 0; n < propsCount; ++n) {
                JSONElement prop = props.arrayItem(n);
                wxString propName = prop.namedObject("name").toString();
                int propId = prop.namedObject("ref").toInt();
                h.properties.insert(std::make_pair(propId, propName));
            }
        }
        m_handles.insert(std::make_pair(handleId, h));
    }
}

void NodeJSDebuggerPane::OnUpdateCallstack(clDebugEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_dataviewLocals);
    ClearCallstack();

    JSONRoot root(event.GetString());
    JSONElement frames = root.toElement().namedObject("body").namedObject("frames");
    JSONElement refs = root.toElement().namedObject("refs");

    // Load the handlers into a map
    m_handles.clear();
    ParseRefsArray(refs);

    int count = frames.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement frame = frames.arrayItem(i);
        int index = frame.namedObject("index").toInt();
        int funcRef = frame.namedObject("func").namedObject("ref").toInt();
        int fileRef = frame.namedObject("script").namedObject("ref").toInt();
        int line = frame.namedObject("line").toInt() + 1;

        wxVector<wxVariant> cols;
        cols.push_back(wxString() << index);
        wxString file, func;
        if(m_handles.count(funcRef)) {
            func = m_handles.find(funcRef)->second.value;
        }
        if(m_handles.count(funcRef)) {
            file = m_handles.find(fileRef)->second.value;
        }
        cols.push_back(func);
        cols.push_back(file);
        cols.push_back(wxString() << line);

        FrameData* cd = new FrameData();
        cd->file = file;
        cd->line = line;
        cd->function = func;
        cd->index = i;
        m_dvListCtrlCallstack->AppendItem(cols, (wxUIntPtr)cd);

        if(i == 0) {
            // Notify the debugger to use frame #0 for the indicator
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            event.SetLineNumber(line);
            event.SetFileName(file);
            EventNotifier::Get()->AddPendingEvent(event);
            BuildLocals(frame);
            BuildArguments(frame);
        }
    }
}

void NodeJSDebuggerPane::OnLostControl(clDebugEvent& event)
{
    event.Skip();
    m_dataviewLocals->Enable(false);
    m_dvListCtrlCallstack->Enable(false);
}

void NodeJSDebuggerPane::OnConsoleLog(clDebugEvent& event)
{
    event.Skip();
    m_consoleLog->AppendText(event.GetString());
    ::clRecalculateSTCHScrollBar(m_consoleLog);
    m_consoleLog->ScrollToEnd();
}

void NodeJSDebuggerPane::OnSessionStarted(clDebugEvent& event)
{
    event.Skip();
    m_consoleLog->ClearAll();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_consoleLog);
    }
}

void NodeJSDebuggerPane::OnItemActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    FrameData* cd = (FrameData*)m_dvListCtrlCallstack->GetItemData(event.GetItem());
    CHECK_PTR_RET(cd);
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();
    if(!debugger) return;
    debugger->SelectFrame(cd->index);
}

void NodeJSDebuggerPane::ClearCallstack()
{
    for(int i = 0; i < m_dvListCtrlCallstack->GetItemCount(); ++i) {
        FrameData* cd = (FrameData*)m_dvListCtrlCallstack->GetItemData(m_dvListCtrlCallstack->RowToItem(i));
        wxDELETE(cd);
    }
    m_dvListCtrlCallstack->DeleteAllItems();
    m_dataviewLocalsModel->Clear();
    m_dataviewLocals->Enable(true);
    m_dvListCtrlCallstack->Enable(true);
}

void NodeJSDebuggerPane::AddLocal(wxDataViewItem& parent, const wxString& name, int refId, int depth)
{
    if(depth >= 20) {
        // don't go into infinite recurse
        return;
    }

    wxVector<wxVariant> cols;
    cols.push_back(name);

    // extract the value
    if(m_handles.count(refId)) {
        Handle h = m_handles.find(refId)->second;
        cols.push_back(h.type);
        cols.push_back(h.value);
        wxDataViewItem child = m_dataviewLocalsModel->AppendItem(parent, cols);

        if(!h.properties.empty()) {
            std::for_each(h.properties.begin(), h.properties.end(), [&](const std::pair<int, wxString>& p) {
                AddLocal(child, p.second, p.first, depth + 1);
            });
        }
    } else {
        cols.push_back("");
        cols.push_back("");
        m_dataviewLocalsModel->AppendItem(parent, cols);
    }
}

void NodeJSDebuggerPane::BuildArguments(const JSONElement& json)
{
    wxVector<wxVariant> cols;
    cols.push_back("Arguments");
    cols.push_back(wxEmptyString);
    cols.push_back(wxEmptyString);

    wxDataViewItem locals = m_dataviewLocalsModel->AppendItem(wxDataViewItem(NULL), cols);
    JSONElement arr = json.namedObject("arguments");
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement local = arr.arrayItem(i);
        AddLocal(
            locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt(), 0);
    }

    if(m_dataviewLocalsModel->HasChildren(locals)) {
        m_dataviewLocals->Expand(locals);
    }
}

void NodeJSDebuggerPane::BuildLocals(const JSONElement& json)
{
    wxVector<wxVariant> cols;
    cols.push_back("Locals");
    cols.push_back(wxEmptyString);
    cols.push_back(wxEmptyString);

    wxDataViewItem locals = m_dataviewLocalsModel->AppendItem(wxDataViewItem(NULL), cols);
    JSONElement arr = json.namedObject("locals");
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement local = arr.arrayItem(i);
        AddLocal(
            locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt(), 0);
    }

    if(m_dataviewLocalsModel->HasChildren(locals)) {
        m_dataviewLocals->Expand(locals);
    }
}

void NodeJSDebuggerPane::OnExceptionThrown(clDebugEvent& event)
{
    event.Skip();
    ::wxMessageBox(_("An uncaught exception thrown!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();
    if(!debugger) return;
    debugger->Callstack();
}

void NodeJSDebuggerPane::OnUpdateDebuggerView(clDebugEvent& event)
{
    event.Skip();
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();
    if(!debugger) return;
    m_dvListCtrlBreakpoints->DeleteAllItems();
    const NodeJSBreakpoint::List_t& breakpoints = debugger->GetBreakpointsMgr()->GetBreakpoints();
    std::for_each(breakpoints.begin(), breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
        wxVector<wxVariant> cols;
        cols.push_back(wxString() << bp.GetNodeBpID());
        cols.push_back(wxString() << bp.GetLine());
        cols.push_back(bp.GetFilename());
        m_dvListCtrlBreakpoints->AppendItem(cols);
    });
}

void NodeJSDebuggerPane::OnFrameSelected(clDebugEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_dataviewLocals);
    m_dataviewLocalsModel->Clear();
    m_dataviewLocals->Enable(true);

    JSONRoot root(event.GetString());
    JSONElement json = root.toElement();
    JSONElement frame = json.namedObject("body");
    JSONElement refs = json.namedObject("refs");

    // Load the handlers into a map
    m_handles.clear();
    ParseRefsArray(refs);

    int index = frame.namedObject("index").toInt();
    int funcRef = frame.namedObject("func").namedObject("ref").toInt();
    int fileRef = frame.namedObject("script").namedObject("ref").toInt();
    int line = frame.namedObject("line").toInt() + 1;

    wxVector<wxVariant> cols;
    cols.push_back(wxString() << index);
    wxString file, func;
    if(m_handles.count(funcRef)) {
        func = m_handles.find(funcRef)->second.value;
    }
    if(m_handles.count(funcRef)) {
        file = m_handles.find(fileRef)->second.value;
    }
    cols.push_back(func);
    cols.push_back(file);
    cols.push_back(wxString() << line);

    // Notify the debugger to use frame #0 for the indicator
    clDebugEvent eventHighlight(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
    eventHighlight.SetLineNumber(line);
    eventHighlight.SetFileName(file);
    EventNotifier::Get()->AddPendingEvent(eventHighlight);
    BuildLocals(frame);
    BuildArguments(frame);
}

void NodeJSDebuggerPane::OnBreakpointSelected(wxDataViewEvent& event)
{
    wxVariant v;
    int line;
    wxString file;
    // sanity
    CHECK_ITEM_RET(event.GetItem());
    wxDataViewItem item = event.GetItem();
    int row = m_dvListCtrlBreakpoints->ItemToRow(item);
    // sanity
    if(row >= m_dvListCtrlBreakpoints->GetItemCount()) return;

    m_dvListCtrlBreakpoints->GetValue(v, row, 1);
    line = v.GetInteger();

    m_dvListCtrlBreakpoints->GetValue(v, row, 2);
    file = v.GetString();

    CallAfter(&NodeJSDebuggerPane::DoOpenFile, file, line);
}

void NodeJSDebuggerPane::DoOpenFile(const wxString& filename, int line)
{
    clGetManager()->OpenFile(filename, "", line - 1);
}
void NodeJSDebuggerPane::OnEvaluateExpression(wxCommandEvent& event)
{
    if(m_textCtrlExpression->IsEmpty()) return;

    clDebugEvent evalEvent(wxEVT_NODEJS_DEBUGGER_EVAL_EXPRESSION);
    evalEvent.SetString(m_textCtrlExpression->GetValue());
    EventNotifier::Get()->AddPendingEvent(evalEvent);
}

void NodeJSDebuggerPane::OnExpressionEvaluated(clDebugEvent& event)
{
    event.Skip();
    wxString message;
    message << "eval(" << m_textCtrlExpression->GetValue() << "):\n" << event.GetString();
    
    wxString currentText = m_consoleLog->GetValue();
    if(!currentText.EndsWith("\n")) {
        message.Prepend("\n");
    }
    if(!message.EndsWith("\n")) {
        message << "\n";
    }
    m_consoleLog->AppendText(message);
    m_consoleLog->ScrollToEnd();
    
    // Restore the focus to the text control
    m_textCtrlExpression->CallAfter(&wxTextCtrl::SetFocus);
}
