#include "PHPDebugPane.h"
#include <event_notifier.h>
#include "XDebugManager.h"
#include <wx/tokenzr.h>
#include <globals.h>
#include <cl_aui_notebook_art.h>
#include "php_utils.h"
#include "php_event.h"
#include "xdebugevent.h"
#include <editor_config.h>
#include <lexer_configuration.h>

PHPDebugPane::PHPDebugPane(wxWindow* parent)
    : PHPDebugPaneBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_STACK_TRACE, &PHPDebugPane::OnUpdateStackTrace, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &PHPDebugPane::OnXDebugSessionEnded, this);

#ifndef __WXGTK__
    m_auiBook->SetArtProvider( new clAuiGlossyTabArt );
#endif
    // Set the proper font
}

PHPDebugPane::~PHPDebugPane()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_STACK_TRACE, &PHPDebugPane::OnUpdateStackTrace, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &PHPDebugPane::OnRefreshBreakpointsView, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &PHPDebugPane::OnXDebugSessionEnded, this);
}

void PHPDebugPane::OnUpdateStackTrace(XDebugEvent& e)
{
    e.Skip();
    m_dvListCtrlStackTrace->DeleteAllItems();

    const wxArrayString &calls = e.GetStrings();
    for(size_t i=0; i<calls.GetCount(); ++i) {
        wxArrayString elements = ::wxStringTokenize(calls.Item(i), "|", wxTOKEN_RET_EMPTY);
        if ( elements.GetCount() == 4 ) {
            wxVector<wxVariant> cols;
            cols.push_back( ::MakeIconText(elements.Item(0), ((int)i == e.GetInt()) ? m_images.Bitmap("m_bmpArrowActive") : m_images.Bitmap("m_bmpArrowDisabled")) ); // Level
            cols.push_back( elements.Item(1) ); // Where
            cols.push_back( ::URIToFileName(elements.Item(2)) ); // File
            cols.push_back( elements.Item(3) ); // Line
            m_dvListCtrlStackTrace->AppendItem(cols);
        }
    }
}

void PHPDebugPane::OnCallStackItemActivated(wxDataViewEvent& event)
{
    if ( event.GetItem().IsOk() ) {
        // Open the file - we use an event to do so
        wxVariant depth, filename, lineNumber;
        int row = m_dvListCtrlStackTrace->ItemToRow(event.GetItem());
        m_dvListCtrlStackTrace->GetValue(depth, row, 0);
        m_dvListCtrlStackTrace->GetValue(filename, row, 2);
        m_dvListCtrlStackTrace->GetValue(lineNumber, row, 3);
        long nLine(-1);
        long nDepth(-1);
        lineNumber.GetString().ToLong(&nLine);
        depth.GetString().ToLong(&nDepth);

        PHPEvent eventOpenFile(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED);
        eventOpenFile.SetLineNumber( nLine );
        eventOpenFile.SetInt(nDepth);
        eventOpenFile.SetFileName( filename.GetString() );
        EventNotifier::Get()->AddPendingEvent( eventOpenFile );
    }
}

void PHPDebugPane::OnClearAll(wxCommandEvent& event)
{
    wxUnusedVar( event );
    PHPEvent eventDelAllBP(wxEVT_PHP_DELETE_ALL_BREAKPOINTS);
    EventNotifier::Get()->AddPendingEvent( eventDelAllBP );
}

void PHPDebugPane::OnClearAllUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dvListCtrlBreakpoints->GetItemCount() && XDebugManager::Get().IsDebugSessionRunning() );
}

void PHPDebugPane::OnDeleteBreakpoint(wxCommandEvent& event)
{
    wxUnusedVar( event );

    // Send event for every breakpoint id
    wxDataViewItemArray items;
    m_dvListCtrlBreakpoints->GetSelections( items );
    for(size_t i=0; i<items.GetCount(); ++i) {

        XDebugBreakpoint bp = GetBreakpoint( items.Item(i) );
        PHPEvent eventDelBP(wxEVT_PHP_DELETE_BREAKPOINT);
        eventDelBP.SetInt( bp.GetBreakpointId() );
        eventDelBP.SetFileName( bp.GetFileName() );
        eventDelBP.SetLineNumber( bp.GetLine() );
        EventNotifier::Get()->AddPendingEvent( eventDelBP );

    }
}

void PHPDebugPane::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlBreakpoints->GetSelectedItemsCount() && XDebugManager::Get().IsDebugSessionRunning() );
}

XDebugBreakpoint PHPDebugPane::GetBreakpoint(const wxDataViewItem& item) const
{
    wxVariant id, filename, lineNumber;
    int row = m_dvListCtrlBreakpoints->ItemToRow(item);
    m_dvListCtrlBreakpoints->GetValue(id, row, 0);
    m_dvListCtrlBreakpoints->GetValue(filename, row, 1);
    m_dvListCtrlBreakpoints->GetValue(lineNumber, row, 2);
    long nId(-1);
    long nLine(-1);

    lineNumber.GetString().ToLong(&nLine);
    id.GetString().ToCLong(&nId);
    XDebugBreakpoint bp(filename.GetString(), nLine);
    bp.SetBreakpointId( nId );
    return bp;
}

void PHPDebugPane::OnRefreshBreakpointsView(XDebugEvent& e)
{
    e.Skip();
    LexerConf::Ptr_t lex =  EditorConfigST::Get()->GetLexer("html");
    if ( lex ) {
        m_dvListCtrlBreakpoints->SetFont( lex->GetFontForSyle(wxSTC_HPHP_DEFAULT) );
        m_dvListCtrlStackTrace->SetFont( lex->GetFontForSyle(wxSTC_HPHP_DEFAULT) );
    }

    // Load the breakpoints table
    m_dvListCtrlBreakpoints->DeleteAllItems();
    const XDebugBreakpoint::List_t &bps = XDebugManager::Get().GetBreakpointsMgr().GetBreakpoints();
    XDebugBreakpoint::List_t::const_iterator iter = bps.begin();
    for( ; iter != bps.end(); ++iter ) {
        wxVector<wxVariant> cols;
        cols.push_back( wxString() << iter->GetBreakpointId() );
        cols.push_back( iter->GetFileName() );
        cols.push_back( wxString() << iter->GetLine() );
        m_dvListCtrlBreakpoints->AppendItem( cols );
    }
}

void PHPDebugPane::OnBreakpointItemActivated(wxDataViewEvent& event)
{
    XDebugBreakpoint bp = GetBreakpoint(event.GetItem());
    PHPEvent eventOpenFile(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED);
    eventOpenFile.SetLineNumber( bp.GetLine() );
    eventOpenFile.SetFileName( bp.GetFileName() );
    EventNotifier::Get()->AddPendingEvent( eventOpenFile );
}

void PHPDebugPane::SelectTab(const wxString& title)
{
    size_t count = m_auiBook->GetPageCount();
    for(size_t i=0; i<count; ++i) {
        if ( m_auiBook->GetPageText(i) == title ) {
            m_auiBook->SetSelection( i );
            break;
        }
    }
}

void PHPDebugPane::OnXDebugSessionEnded(XDebugEvent& e)
{
    e.Skip();
    // Clear the stacktrace view
    m_dvListCtrlStackTrace->DeleteAllItems();
}
