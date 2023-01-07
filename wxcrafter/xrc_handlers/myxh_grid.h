/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_grid.h
// Purpose:     XML resource handler for wxGrid
// Author:      Agron Selimaj
// Created:     2005/08/11
// RCS-ID:      $Id: xh_grid.h 41590 2006-10-03 14:53:40Z VZ $
// Copyright:   (c) 2005 Agron Selimaj, Freepour Controls Inc.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MYXH_GRD_H_
#define _WX_MYXH_GRD_H_

#include "wx/xrc/xmlres.h"

class MyWxGridXmlHandler : public wxXmlResourceHandler
{
protected:
    wxString GetNodeContent(const wxString& name);

public:
    MyWxGridXmlHandler();

    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_XH_GRD_H_
