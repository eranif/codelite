#include "ColoursAndFontsManager.h"
#include "PHPDebugPane.h"
#include "XDebugManager.h"
#include "lexer_configuration.h"
#include "php_event.h"
#include "php_utils.h"
#include "php_workspace.h"
#include "xdebugevent.h"
#include <cl_aui_notebook_art.h>
#include <editor_config.h>
#include <event_notifier.h>
#include <globals.h>
#include <lexer_configuration.h>
#include <wx/tokenzr.h>

PHPDebugPane::PHPDebugPane(wxWindow* parent)
    : PHPDebugPaneBase(parent)
{
    Hide();
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_STACK_TRACE, &PHPDebugPane::OnUpdateStackTrace, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &PHPDebugPane::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTING, &PHPDebugPane::OnXDebugSessionStarting, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &PHPDebugPane::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &PHPDebugPane::OnSettingsChanged, this);
    m_console = new TerminalEmulatorUI(m_auiBook);

    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_auiBook->SetStyle((kNotebook_Default & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_auiBook->SetStyle(kNotebook_Default);
    }

    m_auiBook->AddPage(m_console, _("Console"), true);
    LexerConf::Ptr_t phpLexer = ColoursAndFontsManager::Get().GetLexer("php");
    if(phpLexer) { phpLexer->Apply(m_console->GetTerminalOutputWindow()); }
    m_tbBreakpoints->AddTool(wxID_DELETE, _("Delete"), clGetManager()->GetStdIcons()->LoadBitmap("minus"),
                             _("Delete the selected breakpoints"));
    m_tbBreakpoints->AddTool(wxID_CLEAR, _("Delete all breakpoints"),
                             clGetManager()->GetStdIcons()->LoadBitmap("clean"), _("Delete all breakpoints"));
    m_tbBreakpoints->Bind(wxEVT_TOOL, &PHPDebugPane::OnDeleteBreakpoint, this, wxID_DELETE);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &PHPDebugPane::OnDeleteBreakpointUI, this, wxID_DELETE);
    m_tbBreakpoints->Bind(wxEVT_TOOL, &PHPDebugPane::OnClearAll, this, wxID_CLEAR);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &PHPDebugPane::OnClearAllUI, this, wxID_CLEAR);
    m_tbBreakpoints->Realize();

    // Assign bitmaps to the view
    m_bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("forward"));
    m_bitmaps.push_back(clGetManager()->GetStdIcons()->LoadBitmap("placeholder"));
    m_dvListCtrlStackTrace->SetBitmaps(&m_bitmaps);
}

PHPDebugPane::~PHPDebugPane()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_STACK_TRACE, &PHPDebugPane::OnUpdateStackTrace, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &PHPDebugPane::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTING, &PHPDebugPane::OnXDebugSessionStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &PHPDebugPane::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &PHPDebugPane::OnSettingsChanged, this);
}

void PHPDebugPane::OnUpdateStackTrace(XDebugEvent& e)
{
    e.Skip();
    m_dvListCtrlStackTrace->DeleteAllItems();

    const wxArrayString& calls = e.GetStrings();
    for(size_t i = 0; i < calls.GetCount(); ++i) {
        wxArrayString elements = ::wxStringTokenize(calls.Item(i), "|", wxTOKEN_RET_EMPTY);
        if(elements.GetCount() == 4) {
            wxVector<wxVariant> cols;
            cols.push_back(::MakeBitmapIndexText(elements.Item(0), ((int)i == e.GetInt()) ? 0 : 1)); // Level
            cols.push_back(elements.Item(1));                                                        // Where
            cols.push_back(::URIToFileName(elements.Item(2)));                                       // File
            cols.push_back(elements.Item(3));                                                        // Line
            m_dvListCtrlStackTrace->AppendItem(cols);
        }
    }
}

void PHPDebugPane::OnCallStackItemActivated(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);

    // Open the file - we use an event to do so
    wxString depth = m_dvListCtrlStackTrace->GetItemText(item, 0);
    wxString filename = m_dvListCtrlStackTrace->GetItemText(item, 2);
    wxString lineNumber = m_dvListCtrlStackTrace->GetItemText(item, 3);

    long nLine(-1);
    long nDepth(-1);
    lineNumber.ToLong(&nLine);
    depth.ToLong(&nDepth);

    PHPEvent eventOpenFile(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED);
    eventOpenFile.SetLineNumber(nLine);
    eventOpenFile.SetInt(nDepth);
    eventOpenFile.SetFileName(filename);
    EventNotifier::Get()->AddPendingEvent(eventOpenFile);
}

void PHPDebugPane::OnClearAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    PHPEvent eventDelAllBP(wxEVT_PHP_DELETE_ALL_BREAKPOINTS);
    EventNotifier::Get()->AddPendingEvent(eventDelAllBP);
}

void PHPDebugPane::OnClearAllUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlBreakpoints->GetItemCount() && XDebugManager::Get().IsDebugSessionRunning());
}

void PHPDebugPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // Send event for every breakpoint id
    wxDataViewItemArray items;
    m_dvListCtrlBreakpoints->GetSelections(items);
    for(size_t i = 0; i < items.GetCount(); ++i) {

        XDebugBreakpoint bp = GetBreakpoint(items.Item(i));
        PHPEvent eventDelBP(wxEVT_PHP_DELETE_BREAKPOINT);
        eventDelBP.SetInt(bp.GetBreakpointId());
        eventDelBP.SetFileName(bp.GetFileName());
        eventDelBP.SetLineNumber(bp.GetLine());
        EventNotifier::Get()->AddPendingEvent(eventDelBP);
    }
}

void PHPDebugPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlBreakpoints->GetSelectedItemsCount() && XDebugManager::Get().IsDebugSessionRunning());
}

XDebugBreakpoint PHPDebugPane::GetBreakpoint(const wxDataViewItem& item) const
{
    if(!item.IsOk()) { return XDebugBreakpoint(); }

    wxString id = m_dvListCtrlBreakpoints->GetItemText(item, 0);
    wxString filename = m_dvListCtrlBreakpoints->GetItemText(item, 1);
    wxString lineNumber = m_dvListCtrlBreakpoints->GetItemText(item, 2);

    long nId(-1);
    long nLine(-1);

    lineNumber.ToLong(&nLine);
    id.ToCLong(&nId);
    XDebugBreakpoint bp(filename, nLine);
    bp.SetBreakpointId(nId);
    return bp;
}

void PHPDebugPane::OnRefreshBreakpointsView(XDebugEvent& e)
{
    e.Skip();
    // LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("php");
    // if(lex) {
    //     m_dvListCtrlBreakpoints->SetFont(lex->GetFontForSyle(wxSTC_HPHP_DEFAULT));
    //     m_dvListCtrlStackTrace->SetFont(lex->GetFontForSyle(wxSTC_HPHP_DEFAULT));
    // }

    // Load the breakpoints table
    m_dvListCtrlBreakpoints->DeleteAllItems();
    const XDebugBreakpoint::List_t& bps = XDebugManager::Get().GetBreakpointsMgr().GetBreakpoints();
    XDebugBreakpoint::List_t::const_iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        wxVector<wxVariant> cols;
        cols.push_back(wxString() << iter->GetBreakpointId());
        cols.push_back(iter->GetFileName());
        cols.push_back(wxString() << iter->GetLine());
        m_dvListCtrlBreakpoints->AppendItem(cols);
    }
}

void PHPDebugPane::OnBreakpointItemActivated(wxDataViewEvent& event)
{
    XDebugBreakpoint bp = GetBreakpoint(event.GetItem());
    PHPEvent eventOpenFile(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED);
    eventOpenFile.SetLineNumber(bp.GetLine());
    eventOpenFile.SetFileName(bp.GetFileName());
    EventNotifier::Get()->AddPendingEvent(eventOpenFile);
}

void PHPDebugPane::SelectTab(const wxString& title)
{
    size_t count = m_auiBook->GetPageCount();
    for(size_t i = 0; i < count; ++i) {
        if(m_auiBook->GetPageText(i) == title) {
            m_auiBook->SetSelection(i);
            break;
        }
    }
}

void PHPDebugPane::OnXDebugSessionEnded(XDebugEvent& e)
{
    e.Skip();
    // Clear the stacktrace view
    m_dvListCtrlStackTrace->DeleteAllItems();
    m_console->SetTerminal(NULL);
}

void PHPDebugPane::OnXDebugSessionStarted(XDebugEvent& e)
{
    e.Skip();
    OnRefreshBreakpointsView(e);
    m_console->Clear();
}

void PHPDebugPane::OnXDebugSessionStarting(XDebugEvent& event)
{
    event.Skip();
    m_console->SetTerminal(PHPWorkspace::Get()->GetTerminalEmulator());
    LexerConf::Ptr_t phpLexer = ColoursAndFontsManager::Get().GetLexer("php");
    if(phpLexer) { phpLexer->Apply(m_console->GetTerminalOutputWindow()); }
}
void PHPDebugPane::OnCallStackMenu(wxDataViewEvent& event) {}

void PHPDebugPane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_auiBook->SetStyle((m_auiBook->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_auiBook->SetStyle((m_auiBook->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs);
    }
}
