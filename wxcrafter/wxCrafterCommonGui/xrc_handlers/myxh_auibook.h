/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_notbk.h
// Purpose:     XML resource handler for wxNotebook
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/xrc/xmlreshandler.h>

class wxAuiNotebook;

class MyWxAuiNotebookXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxAuiNotebookXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    bool m_isInside = false;
    wxAuiNotebook* m_notebook = nullptr;
};
