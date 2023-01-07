#include "menu_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

MenuWrapper::MenuWrapper()
    : wxcWidget(ID_WXMENU)
{
    m_styles.Clear();
    m_properties.DeleteValues();

#ifdef __WXGTK__
    PREPEND_STYLE_FALSE(wxMENU_TEAROFF);
#endif

    RegisterEvent("wxEVT_MENU_OPEN", "wxMenuEvent", _("The menu is about to open"));
    RegisterEvent("wxEVT_MENU_HIGHLIGHT", "wxMenuEvent", _("A particular menu item has been highlit"));
    RegisterEvent("wxEVT_MENU_HIGHLIGHT_ALL", "wxMenuEvent", _("The currently selected menu item has changed"));
    RegisterEvent("wxEVT_MENU_CLOSE", "wxMenuEvent", _("The menu is about to close"));

    SetPropertyString(_("Common Settings"), "wxMenu");
    AddProperty(new CategoryProperty(_("Menu")));
    AddProperty(new StringProperty(PROP_NAME, "", _("C++ variable name")));
    AddProperty(new StringProperty(PROP_LABEL, _("Menu"), _("A title for the popup menu")));

    m_namePattern = wxT("m_menu");
    SetName(GenerateName());
}

MenuWrapper::~MenuWrapper() {}

wxcWidget* MenuWrapper::Clone() const { return new MenuWrapper(); }

wxString MenuWrapper::CppCtorCode() const
{
    wxString code;

    wxcWidget* ancestor = GetParent(); // Find the first non-menu parent, to check it's kosher
    while(ancestor && (ancestor->GetType() == ID_WXMENU || ancestor->GetType() == ID_WXSUBMENU)) {
        ancestor = ancestor->GetParent();
    }
    if(ancestor && ancestor->GetType() == ID_WXTOOLBARITEM) {
        // A toolbaritem dropdown menu (probably an auitoolbar, as that's not yet implemented for the standard one
        // This is handled in a completely different way, so don't emit anything
        return code;
    }

    code << GetName() << wxT(" = new ") << GetWxClassName() << wxT("();\n");

    wxcWidget* parent = GetParent();
    if(parent) {
        if(parent->GetType() == ID_WXMENUBAR) {

            // This menu is a child of a menubar, add it
            code << GetWindowParent() << wxT("->Append(") << GetName() << wxT(", ")
                 << wxCrafter::UNDERSCORE(PropertyString(PROP_LABEL)) << wxT(");\n");

        } else if(parent->GetType() == ID_WXMENU || parent->GetType() == ID_WXSUBMENU) {

            // Sub menu
            code << GetWindowParent() << wxT("->AppendSubMenu(") << GetName() << wxT(", ") << Label() << wxT(");\n");
        }
    }
    return code;
}

void MenuWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/menu.h>")); }

wxString MenuWrapper::GetWxClassName() const { return wxT("wxMenu"); }

void MenuWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCLabel() << XRCStyle();

    ChildrenXRC(text, type);

    text << XRCSuffix();
}
