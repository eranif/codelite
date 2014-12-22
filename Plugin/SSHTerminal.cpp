#include "SSHTerminal.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "cl_ssh.h"
#include <wx/msgdlg.h>

BEGIN_EVENT_TABLE(SSHTerminal, SSHTerminalBase)
wx__DECLARE_EVT1(wxEVT_SSH_COMMAND_OUTPUT, wxID_ANY, clCommandEventHandler(SSHTerminal::OnSshOutput))
wx__DECLARE_EVT1(wxEVT_SSH_COMMAND_ERROR, wxID_ANY, clCommandEventHandler(SSHTerminal::OnSshCommandDoneWithError))
wx__DECLARE_EVT1(wxEVT_SSH_COMMAND_COMPLETED, wxID_ANY, clCommandEventHandler(SSHTerminal::OnSshCommandDone))
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
        switch(state)
        {
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
}

SSHTerminal::~SSHTerminal() {}

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
