//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : my_menu_bar.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "my_menu_bar.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

MyMenuBar::MyMenuBar()
	: m_mb(NULL)
{
}

MyMenuBar::~MyMenuBar()
{
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
	if (m_mb && m_mb->m_widget) {
		// Release our reference and set m_widget to NULL this is to avoid freeing the widget again in the d-tor of wxWindow
		m_mb->m_widget = NULL;
	}
#endif
}

void MyMenuBar::Set(wxMenuBar* mb)
{
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
	m_mb = mb;
#else
	wxUnusedVar(mb);
#endif

}
