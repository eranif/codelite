#include "ClangOutputTab.h"
#include "event_notifier.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include <codelite_events.h>
#include "tags_options_data.h"
#include "frame.h"
#include "clang_code_completion.h"
#include "ctags_manager.h"

ClangOutputTab::ClangOutputTab(wxWindow* parent)
    : ClangOutputTabBase(parent)
{
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ClangOutputTab::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(ClangOutputTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CLANG_CODE_COMPLETE_MESSAGE, clCommandEventHandler(ClangOutputTab::OnClangOutput), NULL, this);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stc);
    }

    // defaults
    m_choiceCache->Clear();
    m_choiceCache->Append(TagsOptionsData::CLANG_CACHE_LAZY);
    m_choiceCache->Append(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD);
    m_choiceCache->Select(1);
}

ClangOutputTab::~ClangOutputTab()
{
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ClangOutputTab::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_BUILD_STARTED, clBuildEventHandler(ClangOutputTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CLANG_CODE_COMPLETE_MESSAGE, clCommandEventHandler(ClangOutputTab::OnClangOutput), NULL, this);
}

void ClangOutputTab::OnBuildStarted(clBuildEvent& event)
{
    event.Skip(); // Must call this
    DoClear();
}

void ClangOutputTab::OnClangOutput(clCommandEvent& event)
{
    event.Skip();
    DoAppendText(event.GetString());
}

void ClangOutputTab::DoAppendText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    if(!m_stc->IsEmpty() && m_stc->GetCharAt(m_stc->GetLastPosition() - 1) != '\n') {
        m_stc->AppendText("\n");
    }
    m_stc->AppendText(text);
    int pos = m_stc->GetLastPosition();
    m_stc->SetCurrentPos(pos);
    m_stc->SetSelectionStart(pos);
    m_stc->SetSelectionEnd(pos);
    m_stc->ScrollToEnd();
    m_stc->SetReadOnly(true);
}

void ClangOutputTab::DoClear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void ClangOutputTab::OnPolicy(wxCommandEvent& event)
{
    // Cache policy changed
    clMainFrame::Get()->GetTagsOptions().SetClangCachePolicy(m_choiceCache->GetStringSelection());
    TagsManagerST::Get()->SetCtagsOptions(clMainFrame::Get()->GetTagsOptions());
}

void ClangOutputTab::OnPolicyUI(wxUpdateUIEvent& event)
{
    event.Enable(clMainFrame::Get()->GetTagsOptions().GetClangOptions() & CC_CLANG_ENABLED);
}

void ClangOutputTab::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    int where = m_choiceCache->FindString(clMainFrame::Get()->GetTagsOptions().GetClangCachePolicy());
    if(where != wxNOT_FOUND) {
        m_choiceCache->Select(where);
    }
}
void ClangOutputTab::OnClearCache(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->ClearCache();
#endif
}

void ClangOutputTab::OnClearCacheUI(wxUpdateUIEvent& event)
{
#if HAS_LIBCLANG
    event.Enable(!ClangCodeCompletion::Instance()->IsCacheEmpty());
#else
    event.Enable(false);
#endif
}

void ClangOutputTab::OnClearText(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoClear();
}

void ClangOutputTab::OnClearTextUI(wxUpdateUIEvent& event) { event.Enable(!m_stc->IsEmpty()); }
