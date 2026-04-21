/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_toolbk.h
// Purpose:     XML resource handler for wxToolbook
// Author:      Andrea Zanellato
// Created:     2009/12/12
// Copyright:   (c) 2010 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXCRAFTER_WX_XH_TOOLBK_H_
#define WXCRAFTER_WX_XH_TOOLBK_H_

#include <wx/xrc/xmlreshandler.h>

#if wxUSE_XRC && wxUSE_TOOLBOOK

class WXDLLIMPEXP_FWD_CORE wxToolbook;

class MYwxToolbookXmlHandler : public wxXmlResourceHandler
{
public:
    MYwxToolbookXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    bool m_isInside;
    wxToolbook* m_toolbook;
};

#endif // wxUSE_XRC && wxUSE_TOOLBOOK

#endif // _WX_XH_TOOLBK_H_