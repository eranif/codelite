/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_srchctl.h
// Purpose:     XRC resource handler for wxSearchCtrl
// Author:      Sander Berents
// Created:     2007/07/12
// RCS-ID:      $Id: xh_srchctrl.h 48140 2007-08-16 21:10:14Z VZ $
// Copyright:   (c) 2007 Sander Berents
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MYXH_SRCH_H_
#define _WX_MYXH_SRCH_H_

#include <wx/xrc/xmlres.h>

#if wxUSE_XRC && wxUSE_SEARCHCTRL

class MyWxSearchCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxSearchCtrlXmlHandler();

    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // wxUSE_XRC && wxUSE_SEARCHCTRL

#endif // _WX_XH_SRCH_H_
