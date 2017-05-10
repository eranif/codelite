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
	current_modus = VIM_MODI::NORMAL_MODUS;
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
	}

	IEditor* mEditor = ::clGetManager()->GetActiveEditor();
	CHECK_PTR_RET( mEditor );
	mCtrl = mEditor->GetCtrl();
	
	mCtrl->Bind( wxEVT_CHAR, &VimManager::OnCharEvt, this );
}

void VimManager::OnCharEvt(wxKeyEvent &event)
{
	wxChar ch = event.GetKeyCode();
	if ( ch == 'a' )
		printf("OK!\n");
}
