#include "ClangOutputTab.h"
#include "event_notifier.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include <codelite_events.h>
#include "tags_options_data.h"
#include "frame.h"
#include "clang_code_completion.h"
#include "ctags_manager.h"
#include "cl_config.h"
#include "globals.h"

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
    
    ::clRecalculateSTCHScrollBar(m_stc);
    // defaults
    m_choiceCache->Clear();
    m_choiceCache->Append(TagsOptionsData::CLANG_CACHE_LAZY);
    m_choiceCache->Append(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD);
    m_choiceCache->Select(1);

    // Get the initial value of the clang code completion
    TagsOptionsData tod;
    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem(&tod);
    m_checkBoxEnableClang->SetValue(tod.GetClangOptions() & CC_CLANG_ENABLED);
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
    DoClear();
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
    
    ::clRecalculateSTCHScrollBar(m_stc);
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

void ClangOutputTab::OnPolicyUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxEnableClang->IsChecked()); }

void ClangOutputTab::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    int where = m_choiceCache->FindString(options.GetClangCachePolicy());
    if(where != wxNOT_FOUND) {
        m_choiceCache->Select(where);
    }
    m_checkBoxEnableClang->SetValue(options.GetClangOptions() & CC_CLANG_ENABLED);
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
    event.Enable(!ClangCodeCompletion::Instance()->IsCacheEmpty() && m_checkBoxEnableClang->IsChecked());
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
void ClangOutputTab::OnEnableClang(wxCommandEvent& event)
{
    TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    size_t clangOptions = options.GetClangOptions();
    if(event.IsChecked()) {
        clangOptions |= CC_CLANG_ENABLED;
    } else {
        clangOptions &= ~CC_CLANG_ENABLED;
    }

    // Apply the changes
    options.SetClangOptions(clangOptions);
    TagsManagerST::Get()->SetCtagsOptions(options);
}
