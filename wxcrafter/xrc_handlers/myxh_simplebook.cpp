#include "myxh_simplebook.h"
#include "wxgui_helpers.h"
#include <wx/log.h>
#include <wx/simplebook.h>
#include <wx/sizer.h>

MyWxSimplebookXmlHandler::MyWxSimplebookXmlHandler()
    : wxXmlResourceHandler()
    , m_isInside(false)
    , m_notebook(NULL)
{
    AddWindowStyles();
}

wxObject* MyWxSimplebookXmlHandler::DoCreateResource()
{
    if(m_class == wxT("simplebookpage")) {
        wxXmlNode* n = GetParamNode(wxT("object"));

        if(!n) n = GetParamNode(wxT("object_ref"));

        if(n) {
            bool old_ins = m_isInside;
            m_isInside = false;
            wxObject* item = CreateResFromNode(n, m_notebook, NULL);
            m_isInside = old_ins;
            wxWindow* wnd = wxDynamicCast(item, wxWindow);

            if(wnd) {
                m_notebook->AddPage(wnd, GetText(wxT("label")), GetBool(wxT("selected")), wxNOT_FOUND);

            } else
                wxLogError(wxT("Error in resource."));

            return wnd;
        } else {
            wxLogError(wxT("Error in resource: no control within notebook's <page> tag."));
            return NULL;
        }
    }

    else {
        wxSimplebook* nb =
            new wxSimplebook(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(wxT("style")));
        wxString showEffect = GetText("effect", false);
        nb->SetEffect(wxCrafter::ShowEffectFromString(showEffect));
        nb->SetName(GetName());
        SetupWindow(nb);

        wxSimplebook* old_par = m_notebook;
        m_notebook = nb;
        bool old_ins = m_isInside;
        m_isInside = true;
        CreateChildren(m_notebook, true /*only this handler*/);
        m_isInside = old_ins;
        m_notebook = old_par;
        return nb;
    }
}

bool MyWxSimplebookXmlHandler::CanHandle(wxXmlNode* node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxSimplebook"))) ||
            (m_isInside && IsOfClass(node, wxT("simplebookpage"))));
}
