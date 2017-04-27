#include "vim_manager.h"
#include "event_notifier.h"
#include "macros.h"
#include "ieditor.h"
#include "imanager.h"
#include "globals.h"
//#include "manager.h"
//#include "frame.h"
//#include "cl_editor.h"

VimManager::VimManager()
	: mInsertMode(false)
{

	mEditor = NULL;

	// Event notifier (see WordCompleatiotionDictionary constructor)
	EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED,
							   &VimManager::OnEditorChanged, this);
}

VimManager::~VimManager()
{

	EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED,
								 &VimManager::OnEditorChanged, this);

}


void VimManager::OnEditorChanged(wxCommandEvent &event)
{
	
	if ( mEditor != NULL){
		ctrl->Unbind(wxEVT_KEY_DOWN, &VimManager::OnEditorModified, this);
		ctrl->Unbind(wxEVT_KEY_UP, &VimManager::OnKeyUp, this);
		mInsertMode = false;
	}

	mEditor = ::clGetManager()->GetActiveEditor();
	CHECK_PTR_RET(mEditor);
	
	ctrl = mEditor->GetCtrl();
	ctrl->Bind(wxEVT_KEY_DOWN, &VimManager::OnEditorModified, this);
	ctrl->Bind(wxEVT_KEY_UP, &VimManager::OnKeyUp, this);
	
	event.Skip(true);
}


void VimManager::OnKeyUp(wxKeyEvent &event)
{
	// TODO: detect shift up!

	if ( isShiftPressed( event.GetKeyCode() ) ) {
		shift_pressed = false;
	}
	
}


void VimManager::OnEditorModified(wxKeyEvent &event)
{

	int keycode = event.GetKeyCode();
	char buf[64];

	//snprintf ( buf, 64, "\n\n * code = %d * \n\n", keycode );
	//ctrl->InsertText ( 0, buf ); 

	if ( isShiftPressed( keycode ) ) {
		shift_pressed = true;
		event.Skip ( keycode );
		return;
	}
	
	if ( arrowMove( keycode ) ){
		// if arrow or page navigation propagate event
		event.Skip( true );
		return;
	}
	
	if ( changeIntoInsMode( keycode ) ){
		// if change mode do not propagate event
		event.Skip(false);
		return;
	}

	// normal key, differentiate according to modus
	if ( mInsertMode ){
		event.Skip( insertMode( keycode ) );
	}else{
		event.Skip( modaleMode( keycode ) );
	}
	
}


bool VimManager::changeIntoInsMode(int code)
{
	const int shift_ascii = 32;
	bool propagate_event = false;

	switch( code ) {
		
	case WXK_ESCAPE: // 'd'
		if ( mInsertMode ){
			mInsertMode = false;
			propagate_event = true;
		}
	 	break;
	case 'i' - shift_ascii:// 'i'
		if ( !mInsertMode ){
			mInsertMode = true;
			propagate_event = true;
		}
	 	//mEditor->AppendText( "\ni ..." );
		break;
	case 'a' - shift_ascii:
		if ( !mInsertMode ){
			if ( ! shift_pressed ){
				mInsertMode = true;
				//ctrl->GotoPos( ctrl->GetCurrentPos() + 1 );
				ctrl->CharRight();
				propagate_event = true;
			}else{
				mInsertMode = true;
				//ctrl->GotoPos( ctrl->GetCurrentPos() + 1 );
				ctrl->LineEnd();
				propagate_event = true;
			}
		}
		break;
	default:
		break;
	}

	return propagate_event;
}

/**
 * This handle event with arrows, page up7down etc. which works
 * in vim in every mode
 */
bool VimManager::arrowMove(int code)
{
	switch( code ) {

	case WXK_HOME:
    case WXK_END:
    case WXK_PAGEUP:
    case WXK_PAGEDOWN:
    case WXK_NUMPAD_DOWN:
    case WXK_NUMPAD_UP:
    case WXK_NUMPAD_LEFT:
    case WXK_NUMPAD_RIGHT:
    case WXK_NUMPAD_HOME:
    case WXK_NUMPAD_END:
    case WXK_NUMPAD_PAGEUP:
    case WXK_NUMPAD_PAGEDOWN:
    case WXK_NUMPAD_DELETE:
    case WXK_NUMPAD_INSERT:
    case WXK_DELETE:
	case WXK_LEFT:
	case WXK_RIGHT:
	case WXK_UP:
	case WXK_DOWN: return true;
	default: return false;

	}
	
}

bool VimManager::isShiftPressed(int keycode)
{
	bool is_modifier = false;

	switch ( keycode ) {
	case WXK_SHIFT:
		is_modifier = true;
		break;
	}
	return is_modifier;
}

bool VimManager::insertMode(int keycode)
{
	bool propagate_event = true;
	
	return propagate_event;
}

bool VimManager::modaleMode(int keycode)
{

	const int shift_ascii = 32;
	bool propagate_event = false;
	
	switch ( keycode ) {

		/***************** CURSOR MOTION *******************/	
	case 'j' - shift_ascii:
		//ctrl->GotoLine( ctrl->GetCurrentLine() + 1 );
		ctrl->LineDown();
		break;
	case 'k' - shift_ascii:
		ctrl->LineUp();
		break;
	case 'l' - shift_ascii:
		ctrl->CharRight();
		break;
	case 'h' - shift_ascii:
		ctrl->CharLeft();
		break;
	case 'g' - shift_ascii:
		if ( shift_pressed ){
			ctrl->DocumentEnd();
		}else{

		}
		break;
		/****************** EDITING **************************/
	

	}
	
	return propagate_event;
}

