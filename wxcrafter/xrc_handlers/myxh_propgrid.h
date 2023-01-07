/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_notbk.h
// Purpose:     XML resource handler for wxNotebook
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_PROPGRID_H_
#define _WX_XH_PROPGRID_H_

#include <wx/xrc/xmlres.h>
#include <wx/propgrid/property.h>

class wxPGProperty;
class wxPropertyGrid;
class wxPropertyGridManager;

class MyWxPropGridXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxPropGridXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

protected:
    void HandlePgProperty(wxPGProperty* parent);
    wxArrayString GetArray(const wxXmlNode* node) const;
    wxPGProperty* DoAppendProperty(wxPGProperty* parent, wxPGProperty* prop);

private:
    bool m_isInside;
    wxPropertyGridManager* m_pgmgr;
    wxPGProperty* m_propertyParent;
};

#endif // _WX_XH_PROPGRID_H_
