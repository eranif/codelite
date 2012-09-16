#include "code_completion_box.h"
#include "event_notifier.h"
#include "cc_box.h"
#include "ctags_manager.h"

CodeCompletionBox::CodeCompletionBox()
    : m_ccBox(NULL)
{
    EventNotifier::Get()->Connect(wxEVT_CMD_CODE_COMPLETE_BOX_DISMISSED, wxCommandEventHandler(CodeCompletionBox::OnCCBoxDismissed), NULL, this);
}

CodeCompletionBox::~CodeCompletionBox()
{
}

CodeCompletionBox& CodeCompletionBox::Get()
{
    static CodeCompletionBox mgr;
    return mgr;
}

void CodeCompletionBox::Display(LEditor* editor, const TagEntryPtrVector_t& tags, const wxString& word, bool isKeywordList, wxEvtHandler* owner)
{
    if ( m_ccBox ) {
        Hide();
    }
    
    m_ccBox = new CCBox(editor, false, TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE);
    m_ccBox->Show(tags, word, isKeywordList, owner);
}

void CodeCompletionBox::OnCCBoxDismissed(wxCommandEvent& e)
{
    e.Skip();
    if ( e.GetEventObject() == m_ccBox ) {
        m_ccBox = NULL;
    }
}

void CodeCompletionBox::Hide()
{
    if ( m_ccBox ) {
        m_ccBox->Destroy();
    }
    m_ccBox = NULL;
}

bool CodeCompletionBox::IsShown() const
{
    return m_ccBox && m_ccBox->IsShown();
}

bool CodeCompletionBox::SelectWord(const wxString& word)
{
    if ( IsShown() ) {
        return m_ccBox->SelectWord(word);
    } else {
        return false;
    }
}

void CodeCompletionBox::InsertSelection()
{
    if ( IsShown() ) {
        m_ccBox->InsertSelection();
    }
}

void CodeCompletionBox::Next()
{
    if ( IsShown() ) {
        m_ccBox->Next();
    }
}

void CodeCompletionBox::NextPage()
{
    if ( IsShown() ) {
        m_ccBox->NextPage();
    }
}

void CodeCompletionBox::Previous()
{
    if ( IsShown() ) {
        m_ccBox->Previous();
    }
}

void CodeCompletionBox::PreviousPage()
{
    if ( IsShown() ) {
        m_ccBox->PreviousPage();
    }
}

