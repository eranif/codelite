#include "vim_manager.h"

/**
 * Default constructor
 */

VimManager::VimManager()
	: mCurrCmd(),
	  mLastCmd(),
	  mTmpBuf()
{
	
	mCtrl   = NULL;
	EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED,
							   &VimManager::OnEditorChanged, this);
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
	/* change of the editor*/
	if ( mCtrl != NULL ) {
		mCtrl->Unbind( wxEVT_CHAR, &VimManager::OnCharEvt, this );
		mCtrl->Unbind( wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this );
	}

	IEditor* mEditor = ::clGetManager()->GetActiveEditor();
	CHECK_PTR_RET( mEditor );
	mCtrl = mEditor->GetCtrl();
	
	mCtrl->Bind( wxEVT_CHAR, &VimManager::OnCharEvt, this );
	mCtrl->Bind( wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this );
}

void VimManager::OnKeyDown(wxKeyEvent &event){

	wxChar ch = event.GetUnicodeKey();
	bool skip_event = true;

	if ( ch != WXK_NONE ) {

		switch ( ch ) {
		case WXK_ESCAPE:
			skip_event = mCurrCmd.OnEscapeDown();
			break;
		default:
			skip_event = true;
			break;
		}	

	} else { 
		skip_event = true;
	}

	event.Skip( skip_event );
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
		/*FIXME: the caret does not change!*/
		if ( mCurrCmd.get_current_modus() == VIM_MODI::NORMAL_MODUS ) {
			mCtrl->SetCaretStyle(10);
		} else {
			mCtrl->SetCaretStyle(1);
		}
		
		mCurrCmd.ResetCommand();
	}
		
	event.Skip( skip_event );
}


void VimManager::Issue_cmd()
{
	for ( int i = 0; i < mCurrCmd.getNumRepeat(); ++i)
		mCurrCmd.Command_call( mCtrl );
}



