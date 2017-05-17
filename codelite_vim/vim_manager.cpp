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
	//						   &VimManager::OnEditorChanged, this);
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

	//FIXME
	//if ( clMainFrame::Get() && clMainFrame::Get()->GetMainBook() ) {
	//	LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	//}
	
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
			skip_event = mCurrCmd.OnEscapeDown();
			break;
		case WXK_RETURN:
			skip_event = mCurrCmd.OnReturnDown( &mEditor, m_mgr);
			if ( mEditor == NULL )
				mCtrl = NULL;
			break;
		default:
			skip_event = true;
			break;
		}	

	} else { 
		skip_event = true;
	}

	updateView();
	
	event.Skip( skip_event );
}

void VimManager::updateView()
{

	if ( mCtrl == NULL )
		return;
	
	if ( mCurrCmd.get_current_modus() == VIM_MODI::NORMAL_MODUS ) {
		mCtrl->SetCaretStyle(mCaretNormStyle);
		//::clGetManager()->GetStatusBar()->SetMessage("<N>");
		m_mgr->GetStatusBar()->SetLanguage("NORMAL");
	} else if ( mCurrCmd.get_current_modus() == VIM_MODI::COMMAND_MODUS ) {
		m_mgr->GetStatusBar()->SetLanguage(mCurrCmd.getTmpBuf());
	} else {
		mCtrl->SetCaretStyle(mCaretInsStyle);
		//::clGetManager()->GetStatusBar()->SetMessage("<I>");
	}
	
}

void VimManager::OnCharEvt(wxKeyEvent &event)
{

	bool skip_event = true;
	wxChar ch = event.GetUnicodeKey();
	
	if ( ch != WXK_NONE ) {

	 	switch ( ch ) {
	 	case WXK_ESCAPE:
	 		skip_event = mCurrCmd.OnEscapeDown();
	 		break;
	 	default:
			skip_event = mCurrCmd.OnNewKeyDown( ch );
			break;
	 	}

	} else { 
		skip_event = true;
	}

	if ( mCurrCmd.is_cmd_complete() ) {

		Issue_cmd();
		
		if ( mCurrCmd.get_current_modus() != VIM_MODI::REPLACING_MODUS ) {
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
	
	for ( int i = 0; i < mCurrCmd.getNumRepeat(); ++i){
		if ( !mCurrCmd.Command_call( mCtrl ) )
			return; /*If the num repeat is internally implemented do not repeat!*/
	}
}



