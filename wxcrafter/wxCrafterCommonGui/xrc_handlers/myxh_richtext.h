/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_richtext.h
// Purpose:     XML resource handler for wxRichTextCtrl
// Author:      Julian Smart
// Created:     2006-11-08
// RCS-ID:      $Id: xh_richtext.h 67254 2011-03-20 00:14:35Z DS $
// Copyright:   (c) 2006 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MYXH_RICHTEXT_H_
#define _WX_MYXH_RICHTEXT_H_

#include <wx/xrc/xmlreshandler.h>

#if wxUSE_XRC && wxUSE_RICHTEXT

class MyWxRichTextCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxRichTextCtrlXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // wxUSE_XRC && wxUSE_RICHTEXT

#endif // _WX_XH_RICHTEXT_H_
