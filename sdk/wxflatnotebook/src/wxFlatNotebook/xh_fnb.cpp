/////////////////////////////////////////////////////////////////////////////
// Name:        xh_fnb.cpp
// Purpose:     XRC resource for wxFlatNotebook
// Author:      Armel Asselin
// Created:     2006/10/30
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Armel Asselin (copied from Vaclav Slavik xh_notbk)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "xh_notbk.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "wx/wxFlatNotebook/xh_fnb.h"

#include "wx/log.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/sizer.h"

IMPLEMENT_DYNAMIC_CLASS(wxFlatNotebookXmlHandler, wxXmlResourceHandler)

wxFlatNotebookXmlHandler::wxFlatNotebookXmlHandler()
: wxXmlResourceHandler(), m_isInside(false), m_notebook(NULL)
{
    XRC_ADD_STYLE(wxFNB_DEFAULT_STYLE);
    XRC_ADD_STYLE(wxFNB_VC71);
    XRC_ADD_STYLE(wxFNB_FANCY_TABS);
    XRC_ADD_STYLE(wxFNB_TABS_BORDER_SIMPLE);
    XRC_ADD_STYLE(wxFNB_NO_X_BUTTON);
    XRC_ADD_STYLE(wxFNB_NO_NAV_BUTTONS);
    XRC_ADD_STYLE(wxFNB_MOUSE_MIDDLE_CLOSES_TABS);
    XRC_ADD_STYLE(wxFNB_BOTTOM);
    XRC_ADD_STYLE(wxFNB_NODRAG);
    XRC_ADD_STYLE(wxFNB_VC8);
    XRC_ADD_STYLE(wxFNB_X_ON_TAB);
    XRC_ADD_STYLE(wxFNB_BACKGROUND_GRADIENT);
    XRC_ADD_STYLE(wxFNB_COLORFUL_TABS);
    XRC_ADD_STYLE(wxFNB_DCLICK_CLOSES_TABS);
    XRC_ADD_STYLE(wxFNB_SMART_TABS);
    XRC_ADD_STYLE(wxFNB_DROPDOWN_TABS_LIST);
    XRC_ADD_STYLE(wxFNB_ALLOW_FOREIGN_DND);

    AddWindowStyles();
}

wxObject *wxFlatNotebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("notebookpage"))
    {
        wxXmlNode *n = GetParamNode(wxT("object"));

        if ( !n )
            n = GetParamNode(wxT("object_ref"));

        if (n)
        {
            bool old_ins = m_isInside;
            m_isInside = false;
            wxObject *item = CreateResFromNode(n, m_notebook, NULL);
            m_isInside = old_ins;
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
            {
                m_notebook->AddPage(wnd, GetText(wxT("label")),
                                         GetBool(wxT("selected")));
                if ( HasParam(wxT("bitmap")) )
                {
                    wxBitmap bmp = GetBitmap(wxT("bitmap"), wxART_OTHER);
                    wxFlatNotebookImageList *imgList = m_notebook->GetImageList();
                    if ( imgList == NULL )
                    {
                        imgList = new wxFlatNotebookImageList ( );
                        m_notebook->SetImageList( imgList );
                    }
                    imgList->Add(bmp);
                    m_notebook->SetPageImageIndex(m_notebook->GetPageCount()-1, (int)imgList->Count()-1 );
                }
            }
            else
                wxLogError(wxT("Error in resource."));
            return wnd;
        }
        else
        {
            wxLogError(wxT("Error in resource: no control within notebook's <page> tag."));
            return NULL;
        }
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxFlatNotebook)

        nb->Create(m_parentAsWindow,
                   GetID(),
                   GetPosition(), GetSize(),
                   GetStyle(wxT("style")),
                   GetName());

        SetupWindow(nb);

        wxFlatNotebook *old_par = m_notebook;
        m_notebook = nb;
        bool old_ins = m_isInside;
        m_isInside = true;
        CreateChildren(m_notebook, true/*only this handler*/);
        m_isInside = old_ins;
        m_notebook = old_par;

        return nb;
    }
}

bool wxFlatNotebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxFlatNotebook"))) ||
            (m_isInside && IsOfClass(node, wxT("notebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_NOTEBOOK
