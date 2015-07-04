#include "NodeJSDebuggerPane.h"
#include "event_notifier.h"
#include "json_node.h"
#include "macros.h"
#include <map>
#include "ColoursAndFontsManager.h"
#include <wx/wupdlock.h>
#include <wx/msgdlg.h>

NodeJSDebuggerPane::NodeJSDebuggerPane(wxWindow* parent)
    : NodeJSDebuggerPaneBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_LOST_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);

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
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_LOST_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);
    ClearCallstack();
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
            event.SetInt(line);
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
    FrameData* cd = (FrameData*)m_dvListCtrlCallstack->GetItemData(event.GetItem());
    clDebugEvent eventSelected(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME);
    eventSelected.SetInt(cd->index);
    EventNotifier::Get()->AddPendingEvent(eventSelected);
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

void NodeJSDebuggerPane::AddLocal(wxDataViewItem& parent, const wxString& name, int refId)
{
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
                AddLocal(child, p.second, p.first);
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
        AddLocal(locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt());
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
        AddLocal(locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt());
    }

    if(m_dataviewLocalsModel->HasChildren(locals)) {
        m_dataviewLocals->Expand(locals);
    }
}

void NodeJSDebuggerPane::OnExceptionThrown(clDebugEvent& event)
{
    event.Skip();
    ::wxMessageBox(_("An exception thrown!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
    int where = m_notebook->FindPage(m_panelConsoleLog);
    if(where != wxNOT_FOUND) {
        m_notebook->SetSelection(where);
    }
}
