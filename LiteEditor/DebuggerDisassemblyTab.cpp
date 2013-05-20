#include "DebuggerDisassemblyTab.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "debuggerobserver.h"
#include "plugin.h"
#include "manager.h"
#include "breakpointsmgr.h"
#include "macros.h"
#include "frame.h"
#include "debuggerpane.h"
#include "breakpointdlg.h"

#define CURLINE_MARKER    7
#define BREAKPOINT_MARKER 8
#define BREAKPOINT_MARKER_MASK 1 << BREAKPOINT_MARKER

extern const char *stop_xpm[]; // Breakpoint

DebuggerDisassemblyTab::DebuggerDisassemblyTab(wxWindow* parent, const wxString& label)
    : DebuggerDisassemblyTabBase(parent)
    , m_title(label)
{
    m_stc->MarkerDefine(CURLINE_MARKER, wxSTC_MARK_BACKGROUND, wxNullColour, EditorConfigST::Get()->GetOptions()->GetDebuggerMarkerLine());
    m_stc->SetReadOnly(true);
    m_stc->SetMarginSensitive(2, true); // 2 is the symbol margin
    
    m_stc->MarkerDefineBitmap(BREAKPOINT_MARKER, wxBitmap(wxImage(stop_xpm)));

    m_stc->SetYCaretPolicy(wxSTC_CARET_SLOP, 30);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,  clCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, clCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_QUERY_FILELINE,     clCommandEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(DebuggerDisassemblyTab::OnDebuggerStopped), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED, wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL, this);

    LexerConfPtr lex = EditorConfigST::Get()->GetLexer("Assembly");
    if ( lex ) {
        lex->Apply( m_stc, true );
    }
}

DebuggerDisassemblyTab::~DebuggerDisassemblyTab()
{
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,  clCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, clCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_QUERY_FILELINE,     clCommandEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(DebuggerDisassemblyTab::OnDebuggerStopped), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED, wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL, this);
}

void DebuggerDisassemblyTab::OnOutput(clCommandEvent& e)
{
    e.Skip();
    DoClear();
    m_stc->SetReadOnly(false);
    
    BreakpointInfoVec_t memBps;
    wxStringSet_t       addressSet;
    ManagerST::Get()->GetBreakpointsMgr()->GetAllMemoryBreakpoints( memBps );
    for(size_t i=0; i<memBps.size(); ++i) {
        addressSet.insert( memBps.at(i).memory_address );
    }
    // Parse the output
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>( e.GetClientObject() );
    if ( ded ) {

        m_lines.insert(m_lines.end(),
                       ded->m_disassembleLines.begin(),
                       ded->m_disassembleLines.end() );

        for(size_t i=0; i<ded->m_disassembleLines.size(); ++i) {
            m_stc->AppendText( ded->m_disassembleLines.at(i).m_address + "  " + ded->m_disassembleLines.at(i).m_inst + "\n" );
            // restore breakpoints
            if ( addressSet.count( ded->m_disassembleLines.at(i).m_address) ) {
                m_stc->MarkerAdd(i, BREAKPOINT_MARKER);
            }
        }
    }
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
    m_stc->SetReadOnly(true);
}

void DebuggerDisassemblyTab::OnCurLine(clCommandEvent& e)
{
    e.Skip();
    size_t curline = wxString::npos;
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
    if ( ded && ded->m_disassembleLines.size() ) {
        DisassembleEntry entry = ded->m_disassembleLines.at(0);
        for(size_t i=0; i<m_lines.size(); ++i) {
            if ( m_lines.at(i).m_address == entry.m_address ) {
                curline = i;
                break;
            }
        }

        if ( curline != wxString::npos ) {
            m_stc->MarkerAdd(curline, CURLINE_MARKER);

            int pos = m_stc->PositionFromLine(curline);
            m_stc->SetCurrentPos( pos );
            m_stc->SetSelection(pos, pos);
            DoCentrLine(curline);
            //m_stc->EnsureCaretVisible();
            m_textCtrlCurFunction->ChangeValue( entry.m_function );
        }
    }
}

void DebuggerDisassemblyTab::OnDebuggerStopped(wxCommandEvent& e)
{
    e.Skip();
    DoClear();
}

void DebuggerDisassemblyTab::DoClear()
{
    m_stc->SetReadOnly(false);
    m_lines.clear();
    m_stc->ClearAll();
    m_textCtrlCurFunction->ChangeValue("");
    m_stc->SetReadOnly(true);
}

void DebuggerDisassemblyTab::OnQueryFileLineDone(clCommandEvent& e)
{
    e.Skip();

    if ( /*IsShown()*/ true ) {
        wxString file;
        int line = wxNOT_FOUND;

        DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
        if ( ded ) {
            file = ded->m_file;
            line = ded->m_line;

            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
            if ( dbgr && dbgr->IsRunning() ) {
                dbgr->Disassemble(file, line);
            }
        }
    }
}

void DebuggerDisassemblyTab::DoCentrLine(int line)
{
    // we want to make 'first' centered
    int numLinesOnScreen = m_stc->LinesOnScreen();
    int linesAboveIt = numLinesOnScreen / 2;

    line = line - linesAboveIt;
    if ( line < 0 )
        line = 0;

    m_stc->SetFirstVisibleLine( line );
    m_stc->ClearSelections();
}

void DebuggerDisassemblyTab::OnMarginClicked(wxStyledTextEvent& event)
{
    /// get the address of the line
    int nLine = m_stc->LineFromPosition(event.GetPosition());
    wxString line = m_stc->GetLine(nLine);
    wxString address = line.BeforeFirst(' ').Trim(true).Trim(false);;
    
    if ( m_stc->MarkerGet(nLine) & BREAKPOINT_MARKER_MASK ) {
        
        // we already got a marker there
        m_stc->MarkerDelete(nLine, BREAKPOINT_MARKER);
        ManagerST::Get()->GetBreakpointsMgr()->DelBreakpointByAddress( address );
        
    } else {
        m_stc->MarkerAdd(nLine, BREAKPOINT_MARKER);
        ManagerST::Get()->GetBreakpointsMgr()->AddBreakpointByAddress( address );
    }
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void DebuggerDisassemblyTab::OnAllBreakpointsDeleted(wxCommandEvent& e)
{
    e.Skip();
    /// all breakpoints were deleted, update the UI
    m_stc->MarkerDeleteAll(BREAKPOINT_MARKER);
}
