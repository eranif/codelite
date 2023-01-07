/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_notbk.cpp
// Purpose:     XRC resource for wxAuiNotebook
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "myxh_auibook.h"
#include <wx/aui/auibook.h>
#include <wx/log.h>
#include <wx/sizer.h>

class MyAuiBook : public wxAuiNotebook
{
    wxSize m_sz;

public:
    MyAuiBook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE)
        : wxAuiNotebook(parent, id, pos, size, style)
    {
        m_sz = size;
    }

    virtual ~MyAuiBook() {}
    void Resize() { SetSizeHints(m_sz); }
};

MyWxAuiNotebookXmlHandler::MyWxAuiNotebookXmlHandler()
    : wxXmlResourceHandler()
    , m_isInside(false)
    , m_notebook(NULL)
{
    XRC_ADD_STYLE(wxAUI_NB_DEFAULT_STYLE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_SPLIT);
    XRC_ADD_STYLE(wxAUI_NB_TAB_MOVE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_EXTERNAL_MOVE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_FIXED_WIDTH);
    XRC_ADD_STYLE(wxAUI_NB_SCROLL_BUTTONS);
    XRC_ADD_STYLE(wxAUI_NB_WINDOWLIST_BUTTON);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_BUTTON);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_ON_ALL_TABS);
    XRC_ADD_STYLE(wxAUI_NB_TOP);
    XRC_ADD_STYLE(wxAUI_NB_BOTTOM);
    AddWindowStyles();
}

wxObject* MyWxAuiNotebookXmlHandler::DoCreateResource()
{
    if(m_class == wxT("notebookpage")) {
        wxXmlNode* n = GetParamNode(wxT("object"));

        if(!n) n = GetParamNode(wxT("object_ref"));

        if(n) {
            bool old_ins = m_isInside;
            m_isInside = false;
            wxObject* item = CreateResFromNode(n, m_notebook, NULL);
            m_isInside = old_ins;
            wxWindow* wnd = wxDynamicCast(item, wxWindow);

            if(wnd) {
                wxBitmap bmp = wxNullBitmap;
                if(HasParam(wxT("bitmap"))) { bmp = GetBitmap(wxT("bitmap"), wxART_OTHER); }
                m_notebook->AddPage(wnd, GetText(wxT("label")), GetBool(wxT("selected")), bmp);

            } else
                wxLogError(wxT("Error in resource."));

            return wnd;
        } else {
            wxLogError(wxT("Error in resource: no control within notebook's <page> tag."));
            return NULL;
        }
    }

    else {
        wxAuiNotebook* nb = new MyAuiBook(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(wxT("style")));
        nb->SetName(GetName());
        SetupWindow(nb);

        wxAuiNotebook* old_par = m_notebook;
        m_notebook = nb;
        bool old_ins = m_isInside;
        m_isInside = true;
        CreateChildren(m_notebook, true /*only this handler*/);
        ((MyAuiBook*)nb)->Resize();
        m_isInside = old_ins;
        m_notebook = old_par;
        return nb;
    }
}

bool MyWxAuiNotebookXmlHandler::CanHandle(wxXmlNode* node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxAuiNotebook"))) ||
            (m_isInside && IsOfClass(node, wxT("notebookpage"))));
}
