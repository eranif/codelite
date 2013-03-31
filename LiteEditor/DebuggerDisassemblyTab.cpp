#include "DebuggerDisassemblyTab.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "debuggerobserver.h"

DebuggerDisassemblyTab::DebuggerDisassemblyTab(wxWindow* parent, const wxString& label)
    : DebuggerDisassemblyTabBase(parent)
    , m_title(label)
{
    m_stc->MarkerDefine(7, wxSTC_MARK_BACKGROUND, wxNullColour, EditorConfigST::Get()->GetOptions()->GetDebuggerMarkerLine());
    m_stc->SetReadOnly(true);
    //m_stc->HideSelection(true);
    
    m_stc->SetYCaretPolicy(wxSTC_CARET_SLOP, 30);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, wxCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, wxCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
    
    LexerConfPtr lex = EditorConfigST::Get()->GetLexer("Assembly");
    if ( lex ) {
        lex->Apply( m_stc, true );
    }
}

DebuggerDisassemblyTab::~DebuggerDisassemblyTab()
{
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, wxCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, wxCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
}

void DebuggerDisassemblyTab::OnOutput(wxCommandEvent& e)
{
    e.Skip();
    m_stc->SetReadOnly(false);
    m_lines.clear();
    m_stc->ClearAll();
    
    // Parse the output
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
    if ( ded ) {
        
        m_lines.insert(m_lines.end(), 
                       ded->m_disassembleLines.begin(), 
                       ded->m_disassembleLines.end() );
                       
        for(size_t i=0; i<ded->m_disassembleLines.size(); ++i) {
            m_stc->AppendText( ded->m_disassembleLines.at(i).m_address + "  " + ded->m_disassembleLines.at(i).m_inst + "\n" );
        }
    }
    m_stc->SetReadOnly(true);
}

void DebuggerDisassemblyTab::OnCurLine(wxCommandEvent& e)
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
            m_stc->MarkerAdd(curline, 7);
            
            int pos = m_stc->PositionFromLine(curline);
            m_stc->SetSelection(pos, pos);
            m_stc->SetCurrentPos( pos );
            m_stc->EnsureCaretVisible();
            m_textCtrlCurFunction->ChangeValue( entry.m_function );
        }
    }
}
