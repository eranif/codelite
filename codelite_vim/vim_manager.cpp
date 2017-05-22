#include "vim_manager.h"
#include "clStatusBar.h"

/*EXPERIMENT*/
#include "search_thread.h"
#include <wx/tokenzr.h>
#include <wx/fontmap.h>
#include "event_notifier.h"
#include "dirpicker.h"
#include "manager.h"
#include "frame.h"
#include "macros.h"
#include "findinfilesdlg.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"
#include "windowattrmanager.h"
#include <algorithm>
#include "clWorkspaceManager.h"
#include "FindInFilesLocationsDlg.h"


/**
 * Default constructor
 */

VimManager::VimManager(IManager* manager)
    : mCurrCmd(),
      mLastCmd(),
      mTmpBuf()
{
    
    mCtrl   = NULL;
    m_mgr   = manager;
    mCaretInsStyle  = 1;
    mCaretNormStyle = 2;
    
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED,
                               &VimManager::OnEditorChanged, this);
    //EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING,
    //                         &VimManager::OnEditorChanged, this);
}

/**
 * Default distructor
 */

VimManager::~VimManager()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED,
                               &VimManager::OnEditorChanged, this);

}



/**
 * Method used to see the change of focus of the text editor:
 * 
 */
void VimManager::OnEditorChanged(wxCommandEvent &event)
{

    mEditor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET( mEditor );
    mCtrl = mEditor->GetCtrl();

    mCtrl->Bind( wxEVT_CHAR, &VimManager::OnCharEvt, this );
    mCtrl->Bind( wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this );

    updateView();
}

void VimManager::OnKeyDown(wxKeyEvent &event){

    wxChar ch = event.GetUnicodeKey();
    bool skip_event = true;

    if ( mCtrl == NULL || mEditor == NULL ) {
        event.Skip();
        return;
    }

    if ( ch != WXK_NONE ) {

        switch ( ch ) {
        case WXK_ESCAPE:
            skip_event = mCurrCmd.OnEscapeDown( mCtrl );
            break;
        case WXK_RETURN:
            skip_event = mCurrCmd.OnReturnDown( &mEditor, m_mgr);
            if ( mEditor == NULL )
                mCtrl = NULL;
            break;
        default:
            if ( mCurrCmd.get_current_modus() == VIM_MODI::SEARCH_MODUS ) {
                mCurrCmd.set_current_word( get_current_word() );
                mCurrCmd.set_current_modus( VIM_MODI::NORMAL_MODUS );
            }
            skip_event = true;
            break;
        }   

    } else { 
        skip_event = true;
    }

    updateView();
    
    event.Skip( skip_event );
}

wxString VimManager::get_current_word()
{
    long pos = mCtrl->GetCurrentPos();
    long start = mCtrl->WordStartPosition(pos, true);
    long end = mCtrl->WordEndPosition(pos, true);
    wxString word = mCtrl->GetTextRange(start, end);
    return  word;
}

void VimManager::updateView()
{

    if ( mCtrl == NULL )
        return;
    
    if ( mCurrCmd.get_current_modus() == VIM_MODI::NORMAL_MODUS ) {
        mCtrl->SetCaretStyle(mCaretNormStyle);
        //::clGetManager()->GetStatusBar()->SetMessage("<N>");
        m_mgr->GetStatusBar()->SetMessage("NORMAL");
    } else if ( mCurrCmd.get_current_modus() == VIM_MODI::COMMAND_MODUS ) {
        m_mgr->GetStatusBar()->SetMessage(mCurrCmd.getTmpBuf());
    } else {
        mCtrl->SetCaretStyle(mCaretInsStyle);
        //::clGetManager()->GetStatusBar()->SetMessage("<I>");
    }
    
}

void VimManager::OnCharEvt(wxKeyEvent &event)
{

    bool skip_event = true;
    int modifier_key = event.GetModifiers();
    wxChar ch = event.GetUnicodeKey();

    
    if ( ch != WXK_NONE ) {

        switch ( ch ) {
        case WXK_ESCAPE:
            skip_event = mCurrCmd.OnEscapeDown( mCtrl );
            break;
        default:
            skip_event = mCurrCmd.OnNewKeyDown( ch, modifier_key );
            /*FIXME save here inser tmp buffer!*/
            break;
        }

    } else { 
        skip_event = true;
    }


    if ( mCurrCmd.is_cmd_complete() ) {

       bool repeat_last = mCurrCmd.repeat_last_cmd();

        if ( repeat_last )
            repeat_cmd( );
        else
            Issue_cmd();
        
        if ( mCurrCmd.get_current_modus() != VIM_MODI::REPLACING_MODUS ) {
            if ( repeat_last ) {
                mCurrCmd.reset_repeat_last();
            } else if ( mCurrCmd.save_current_cmd() ){ 
                mLastCmd = mCurrCmd;
            }
            mCurrCmd.ResetCommand();
        }
    }
        
    updateView();
    event.Skip( skip_event );
}


void VimManager::Issue_cmd()
{
    if ( mCtrl == NULL )
        return;
      
    mCurrCmd.issue_cmd( mCtrl );
}


void VimManager::repeat_cmd()
{
    if ( mCtrl == NULL )
        return;
     
    mLastCmd.repeat_issue_cmd( mCtrl, mTmpBuf);

}

