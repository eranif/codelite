#include "ColoursAndFontsManager.h"
#include "DockerOutputPane.h"
#include "codelite_events.h"
#include "event_notifier.h"

DockerOutputPane::DockerOutputPane(wxWindow* parent)
    : DockerOutputPaneBase(parent)
{
    m_stc->SetReadOnly(true);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stc);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, [&](wxCommandEvent& event) {
        event.Skip();
        Clear();
    });
    
    m_styler.reset(new clGenericSTCStyler(m_stc));
    {
        wxArrayString words;
        words.Add("successfully");
        m_styler->AddStyle(words, clGenericSTCStyler::kInfo);
    }
    {
        wxArrayString words;
        words.Add("abort ");
        words.Add("Error response from daemon");
        m_styler->AddStyle(words, clGenericSTCStyler::kError);
    }
}

DockerOutputPane::~DockerOutputPane() {}

void DockerOutputPane::Clear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void DockerOutputPane::AddTextWithEOL(const wxString& msg)
{
    wxString message = msg;
    if(!message.EndsWith("\n")) {
        message << "\n";
    }
    AddTextRaw(message);
}

void DockerOutputPane::AddTextRaw(const wxString& msg)
{
    m_stc->SetReadOnly(false);
    m_stc->AddText(msg);
    m_stc->SetReadOnly(true);
    m_stc->ScrollToEnd();
}
