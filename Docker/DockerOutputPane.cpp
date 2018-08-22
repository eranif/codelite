#include "ColoursAndFontsManager.h"
#include "DockerOutputPane.h"
#include "bitmap_loader.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

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
    m_toolbar->AddTool(wxID_CLEAR, _("Clear"), clGetManager()->GetStdIcons()->LoadBitmap("clear"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL,
                    [&](wxCommandEvent& e) {
                        wxUnusedVar(e);
                        Clear();
                    },
                    wxID_CLEAR);
    m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Enable(!m_stc->IsEmpty()); }, wxID_CLEAR);

    m_toolbarContainers->AddTool(wxID_CLEAR, _("Kill Container"),
                                 clGetManager()->GetStdIcons()->LoadBitmap("execute_stop"));
    m_toolbarContainers->AddTool(wxID_CLOSE_ALL, _("Kill All"), clGetManager()->GetStdIcons()->LoadBitmap("clear"));
    m_toolbarContainers->Realize();
    m_toolbarContainers->Bind(wxEVT_TOOL, &DockerOutputPane::OnKillContainer, this, wxID_CLEAR);
    m_toolbarContainers->Bind(wxEVT_UPDATE_UI, &DockerOutputPane::OnKillContainerUI, this, wxID_CLEAR);
    m_toolbarContainers->Bind(wxEVT_TOOL, &DockerOutputPane::OnKillAllContainers, this, wxID_CLOSE_ALL);
    m_toolbarContainers->Bind(wxEVT_UPDATE_UI, &DockerOutputPane::OnKillAllContainersUI, this, wxID_CLOSE_ALL);
    
}

DockerOutputPane::~DockerOutputPane() {}

void DockerOutputPane::Clear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void DockerOutputPane::AddOutputTextWithEOL(const wxString& msg)
{
    wxString message = msg;
    if(!message.EndsWith("\n")) { message << "\n"; }
    AddOutputTextRaw(message);
}

void DockerOutputPane::AddOutputTextRaw(const wxString& msg)
{
    m_stc->SetReadOnly(false);
    m_stc->AddText(msg);
    m_stc->SetReadOnly(true);
    m_stc->ScrollToEnd();
}

void DockerOutputPane::OnKillContainer(wxCommandEvent& event)
{
}

void DockerOutputPane::OnKillContainerUI(wxUpdateUIEvent& event)
{
}

void DockerOutputPane::OnKillAllContainers(wxCommandEvent& event)
{
}

void DockerOutputPane::OnKillAllContainersUI(wxUpdateUIEvent& event)
{
}
