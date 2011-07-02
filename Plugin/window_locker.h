#ifndef __WINDOW_LOCKER__H__
#define __WINDOW_LOCKER__H__

#include "codelite_exports.h"
#include <wx/window.h>

class WXDLLIMPEXP_SDK clWindowUpdateLocker 
{
	wxWindow *m_win;
public:
	clWindowUpdateLocker(wxWindow *win) : m_win(win) {
#if wxVERSION_NUMBER < 2900
	#ifndef __WXGTK__
		m_win->Freeze();
	#endif
#endif
	}
	
	~clWindowUpdateLocker() {
#if wxVERSION_NUMBER < 2900
	#ifndef __WXGTK__
		m_win->Thaw();
	#endif
#endif
	}
};

#endif // __WINDOW_LOCKER__H__

