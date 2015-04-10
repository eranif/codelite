#include "SSHTerminal.h"
#if USE_SFTP

#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "cl_ssh.h"
#include <wx/msgdlg.h>
#include "windowattrmanager.h"
#include "event_notifier.h"

wxDEFINE_EVENT(wxEVT_SSH_TERMINAL_CLOSING, clCommandEvent);
#define EVT_COMMAND_SSH(evt, func) wx__DECLARE_EVT1(evt, wxID_ANY, clCommandEventHandler(func))

BEGIN_EVENT_TABLE(SSHTerminal, SSHTerminalBase)
EVT_COMMAND_SSH(wxEVT_SSH_COMMAND_OUTPUT, SSHTerminal::OnSshOutput)
EVT_COMMAND_SSH(wxEVT_SSH_COMMAND_ERROR, SSHTerminal::OnSshCommandDoneWithError)
EVT_COMMAND_SSH(wxEVT_SSH_COMMAND_COMPLETED, SSHTerminal::OnSshCommandDone)
END_EVENT_TABLE()

#define BUFF_STATE_NORMAL 0
#define BUFF_STATE_IN_ESC 1

static void __RemoveTerminalColoring(const wxString& buffer, wxString& modbuffer)
{
    modbuffer.Clear();
    short state = BUFF_STATE_NORMAL;
    wxString::const_iterator iter = buffer.begin();
    for(; iter != buffer.end(); ++iter) {
        wxChar ch = *iter;
        if(ch == 7) continue; // BELL
        
        switch(state) {
        case BUFF_STATE_NORMAL:
            if(ch == 0x1B) { // found ESC char
                state = BUFF_STATE_IN_ESC;

            } else {
                modbuffer << ch;
            }
            break;
        case BUFF_STATE_IN_ESC:
            if(ch == 'm') { // end of color sequence
                state = BUFF_STATE_NORMAL;
            }
            break;
        }
    }
}

SSHTerminal::SSHTerminal(wxWindow* parent, clSSH::Ptr_t ssh)
    : SSHTerminalBase(parent)
    , m_ssh(ssh)
{
    // initialize text / font
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcOutput);
    }
    m_stcOutput->SetEditable(false);
    SetName("SSHTerminal");
    WindowAttrManager::Load(this);
    m_ssh->ExecuteShellCommand(this, "\n");
}

SSHTerminal::~SSHTerminal()
{
    clCommandEvent event(wxEVT_SSH_TERMINAL_CLOSING);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
    
}

void SSHTerminal::OnSshOutput(clCommandEvent& event)
{
    AppendText(event.GetString());
    m_textCtrl1->CallAfter(&wxTextCtrl::SetFocus);
}

void SSHTerminal::OnSshCommandDoneWithError(clCommandEvent& event) { AppendText(event.GetString()); }
void SSHTerminal::OnSshCommandDone(clCommandEvent& event) { m_textCtrl1->CallAfter(&wxTextCtrl::SetFocus); }

void SSHTerminal::OnSendCommand(wxCommandEvent& event)
{
    try {
        m_ssh->ExecuteShellCommand(this, m_textCtrl1->GetValue() + "\n");

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SSH", wxICON_ERROR | wxOK | wxCENTER);
    }
    m_textCtrl1->ChangeValue("");
}

void SSHTerminal::AppendText(const wxString& text)
{
    wxString modBuffer;
    __RemoveTerminalColoring(text, modBuffer);
    m_stcOutput->SetEditable(true);
    m_stcOutput->AppendText(modBuffer);
    m_stcOutput->ScrollToEnd();
    m_stcOutput->SetEditable(false);
    m_stcOutput->CallAfter(&wxTextCtrl::SetFocus);
}

void SSHTerminal::Clear()
{
    m_stcOutput->SetEditable(true);
    m_stcOutput->ClearAll();
    m_stcOutput->SetEditable(false);
}
void SSHTerminal::OnClear(wxCommandEvent& event)
{
    m_stcOutput->SetEditable(true);
    m_stcOutput->ClearAll();
    m_stcOutput->SetEditable(false);
}
void SSHTerminal::OnClearUI(wxUpdateUIEvent& event) { event.Enable(!m_stcOutput->IsEmpty()); }
#endif
