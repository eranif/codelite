#include "ClangOutputTab.h"
#include "ColoursAndFontsManager.h"
#include "clToolBarButtonBase.h"
#include "cl_config.h"
#include "clang_code_completion.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "tags_options_data.h"
#include <codelite_events.h>

ClangOutputTab::ClangOutputTab(wxWindow* parent)
    : ClangOutputTabBase(parent)
{
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ClangOutputTab::OnInitDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(ClangOutputTab::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CLANG_CODE_COMPLETE_MESSAGE,
                                  clCommandEventHandler(ClangOutputTab::OnClangOutput), NULL, this);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_stc); }
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ClangOutputTab::OnFileSaved, this);
    ::clRecalculateSTCHScrollBar(m_stc);

    // Get the initial value of the clang code completion
    TagsOptionsData tod;
    clConfig ccConfig("code-completion.conf");
    ccConfig.ReadItem(&tod);
    m_checkBoxEnableClang->SetValue(tod.GetClangOptions() & CC_CLANG_ENABLED);
    m_toolbar578->FindById(ID_TOOL_CLEAR_ALL)->SetBmp(clGetManager()->GetStdIcons()->LoadBitmap("clean"));
    m_toolbar578->FindById(ID_TOOL_CLEAR_LOG)->SetBmp(clGetManager()->GetStdIcons()->LoadBitmap("clear"));
    m_toolbar578->Realize();

    SetSize(-1, -1);
    if(GetSizer()) { GetSizer()->Fit(this); }
}

ClangOutputTab::~ClangOutputTab()
{
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ClangOutputTab::OnInitDone), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTED, clBuildEventHandler(ClangOutputTab::OnBuildStarted), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_CLANG_CODE_COMPLETE_MESSAGE,
                                     clCommandEventHandler(ClangOutputTab::OnClangOutput), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ClangOutputTab::OnFileSaved, this);
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
    const TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    if(options.GetClangOptions() & CC_CLANG_INLINE_ERRORS) {
        IEditor* editor = ::clGetManager()->GetActiveEditor();

        // event.GetInt() == 1 means that this is a real error message and not just a status message
        // these kind of messages are also shown in the editor
        if(event.GetInt() && editor) {
            editor->SetCodeCompletionAnnotation(event.GetString(), editor->GetCurrentLine());
        }
    }
}

void ClangOutputTab::DoAppendText(const wxString& text)
{
    m_stc->SetReadOnly(false);
    if(!m_stc->IsEmpty() && m_stc->GetCharAt(m_stc->GetLastPosition() - 1) != '\n') { m_stc->AppendText("\n"); }
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

    // Clear editor annotations
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    if(editor) { editor->GetCtrl()->AnnotationClearAll(); }
}

void ClangOutputTab::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
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

void ClangOutputTab::OnEnableClangUI(wxUpdateUIEvent& event)
{
    const TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    event.Check(options.GetClangOptions() & CC_CLANG_ENABLED);
}

void ClangOutputTab::OnShowAnnotations(wxCommandEvent& event)
{
    TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    size_t clangOptions = options.GetClangOptions();
    if(event.IsChecked()) {
        clangOptions |= CC_CLANG_INLINE_ERRORS;
    } else {
        clangOptions &= ~CC_CLANG_INLINE_ERRORS;
    }

    // Apply the changes
    options.SetClangOptions(clangOptions);
    TagsManagerST::Get()->SetCtagsOptions(options);

    // Clear any annotations we have
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    if(editor) { editor->GetCtrl()->AnnotationClearAll(); }
}

void ClangOutputTab::OnShowAnnotationsUI(wxUpdateUIEvent& event)
{
    const TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    event.Enable(options.GetClangOptions() & CC_CLANG_ENABLED);
}

void ClangOutputTab::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    // Clear editor annotations
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    if(editor) { editor->GetCtrl()->AnnotationClearAll(); }
}
