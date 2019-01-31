#include "menu_bar_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/menu.h>

MenuBarWrapper::MenuBarWrapper()
    : wxcWidget(ID_WXMENUBAR)
{
    SetPropertyString(_("Common Settings"), "wxMenuBar");
    m_styles.Clear();
    PREPEND_STYLE_FALSE(wxMB_DOCKABLE);

    m_namePattern = wxT("m_menuBar");
    SetName(GenerateName());
}

MenuBarWrapper::~MenuBarWrapper() {}

wxcWidget* MenuBarWrapper::Clone() const { return new MenuBarWrapper(); }

wxString MenuBarWrapper::CppCtorCode() const
{
    wxString code;
    code << GetName() << wxT(" = new ") << GetWxClassName() << wxT("(") << StyleFlags(wxT("0")) << wxT(");\n");
    if(GetParent() && GetParent()->GetType() == ID_WXFRAME) {
        code << wxT("this->SetMenuBar(") << GetName() << wxT(");\n");
    }
    return code;
}

void MenuBarWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/menu.h>")); }

wxString MenuBarWrapper::GetWxClassName() const { return wxT("wxMenuBar"); }

void MenuBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString menuBarXRC;
    if(type == XRC_DESIGNER) {
        return;
        // menuBarXRC << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"MENU_BAR_ID\">");
    } else {
        menuBarXRC << XRCPrefix();
    }
    menuBarXRC << XRCStyle();
    ChildrenXRC(menuBarXRC, type);
    menuBarXRC << XRCSuffix();
    text << menuBarXRC;
}

wxString MenuBarWrapper::DesignerXRC() const
{
    wxString menuBarXRC;
    menuBarXRC << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"MENU_BAR_ID\">");
    menuBarXRC << XRCStyle();
    ChildrenXRC(menuBarXRC, XRC_DESIGNER);
    menuBarXRC << XRCSuffix();
    return menuBarXRC;
}
