///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/string.h
// Purpose:     MyGtkString class declaration
// Author:      Vadim Zeitlin
// Created:     2006-10-19
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifdef __WXGTK__
#ifndef _GTK_PRIVATE_STRING_H_
#define _GTK_PRIVATE_STRING_H_

// ----------------------------------------------------------------------------
// Convenience class for g_freeing a gchar* on scope exit automatically
// ----------------------------------------------------------------------------
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif
class MyGtkString
{
public:
    explicit MyGtkString(gchar *s) : m_str(s) { }
    ~MyGtkString() { g_free(m_str); }

    const gchar *c_str() const { return m_str; }

    operator gchar *() const { return m_str; }

private:
    gchar *m_str;

    DECLARE_NO_COPY_CLASS(MyGtkString)
};

#endif // _WX_GTK_PRIVATE_STRING_H_


#endif //__WXGTK__
