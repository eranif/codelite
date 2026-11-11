/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_srchctl.h
// Purpose:     XRC resource handler for wxSearchCtrl
// Author:      Sander Berents
// Created:     2007/07/12
// RCS-ID:      $Id: xh_srchctrl.h 48140 2007-08-16 21:10:14Z VZ $
// Copyright:   (c) 2007 Sander Berents
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/xrc/xmlreshandler.h>

#if wxUSE_XRC && wxUSE_SEARCHCTRL

class MyWxSearchCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxSearchCtrlXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // wxUSE_XRC && wxUSE_SEARCHCTRL
