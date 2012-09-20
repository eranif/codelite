#include "code_completion_box.h"
#include "event_notifier.h"
#include "cc_box.h"
#include "ctags_manager.h"
#include "cc_box_tip_window.h"
#include "code_completion_manager.h"

CodeCompletionBox::CodeCompletionBox()
    : m_ccBox(NULL)
    , m_tip(NULL)
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
    if ( !m_ccBox ) {
        DoCreateBox(editor);
    }

    if ( !CodeCompletionManager::Get().GetWordCompletionRefreshNeeded() ) {
        if ( m_ccBox ) {
            Hide();
        }
        DoCreateBox(editor);
    } else {

        // turn off the flag
        if ( !m_ccBox ) {
            CodeCompletionManager::Get().SetWordCompletionRefreshNeeded(false);
            return;
        }
    }

    m_ccBox->Show(tags, word, isKeywordList, owner);
    // Show takes into account the return value of 'GetWordCompletionRefreshNeeded'
    // this is why we reset the flag *after* the call to Show(..)
    CodeCompletionManager::Get().SetWordCompletionRefreshNeeded(false);
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
        // Cancel the calltip as well
        CancelTip();
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

void CodeCompletionBox::CancelTip()
{
    if ( m_tip ) {
        m_tip->Destroy();
        m_tip = NULL;
    }
}

void CodeCompletionBox::ShowTip(const wxString& msg, LEditor* editor)
{
    CancelTip();

    if ( !editor )
        return;
    wxPoint pt = editor->PointFromPosition( editor->GetCurrentPos() );
    wxPoint displayPt = editor->ClientToScreen(pt);
    m_tip = new CCBoxTipWindow(wxTheApp->GetTopWindow(), msg, 1);
    m_tip->PositionAt(displayPt);
}

void CodeCompletionBox::RegisterImage(const wxString& kind, const wxBitmap& bmp)
{
    // sanity
    if ( bmp.IsOk() == false || kind.IsEmpty() )
        return;
    
    BitmapMap_t::iterator iter = m_bitmaps.find(kind);
    if ( iter != m_bitmaps.end() ) {
        m_bitmaps.erase(iter);
    }
    m_bitmaps.insert(std::make_pair(kind, bmp));
}

void CodeCompletionBox::DoCreateBox(LEditor* editor)
{
    m_ccBox = new CCBox(editor, false, TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE);
    BitmapMap_t::const_iterator iter = m_bitmaps.begin();
    for(; iter != m_bitmaps.end(); ++iter ) {
        m_ccBox->RegisterImageForKind(iter->first, iter->second);
    }
}

