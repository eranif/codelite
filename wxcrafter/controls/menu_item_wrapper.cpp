#include "menu_item_wrapper.h"

#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_property.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "winid_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

MenuItemWrapper::MenuItemWrapper()
    : wxcWidget(ID_WXMENUITEM)
{
    static int counter = 0;

    wxString label;
    label << wxT("Item") << (++counter);

    RegisterEventCommand(wxT("wxEVT_MENU"), _("Menu item has been clicked"));

    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    SetPropertyString(_("Common Settings"), "wxMenuItem");
    AddProperty(new CategoryProperty(_("Common Settings")));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_NAME, "", _("C++ variable name")));
    AddProperty(new CategoryProperty(_("Menu Item")));
    AddProperty(new StringProperty(PROP_LABEL, label, _("The menu item label")));
    AddProperty(new StringProperty(PROP_ACCELERATOR, "", _("Keyboard Shortcut")));
    AddProperty(new StringProperty(PROP_HELP, "", _("Short help string")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Menu item image")));

    m_namePattern = wxT("m_menuItem");
    SetName(GenerateName());

    AddProperty(new ChoiceProperty(PROP_KIND, wxCrafter::GetToolTypes(), 0,
                                   _("The type of menu item: normal, radio, checkable or separator")));
    AddProperty(new BoolProperty(PROP_CHECKED, false, _("For a checkable menu item, should this be checked")));
}

MenuItemWrapper::~MenuItemWrapper() {}

wxcWidget* MenuItemWrapper::Clone() const { return new MenuItemWrapper(); }

wxString MenuItemWrapper::CppCtorCode() const
{
    wxString code;
    wxString label;

    wxcWidget* parent = GetParent(); // Find the first non-menu parent, to check it's kosher
    while(parent && (parent->GetType() == ID_WXMENU || parent->GetType() == ID_WXSUBMENU)) {
        parent = parent->GetParent();
    }
    if(parent && parent->GetType() == ID_WXTOOLBARITEM) {
        // An item in a toolbaritem dropdown menu (probably an auitoolbar, as that's not yet implemented for the
        // standard one)
        // This is handled in a completely different way, so don't emit anything
        return code;
    }

    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString ID = WindowID();

    wxString actualKind;
    wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(PropertyString(PROP_KIND));
    switch(toolType) {
    case wxCrafter::TOOL_TYPE_SEPARATOR:
        ID = wxT("wxID_SEPARATOR");
        break;

    case wxCrafter::TOOL_TYPE_NORMAL:
        actualKind = wxT("wxITEM_NORMAL");
        break;

    case wxCrafter::TOOL_TYPE_RADIO:
        actualKind = wxT("wxITEM_RADIO");
        break;

    default:
    case wxCrafter::TOOL_TYPE_CHECK:
        actualKind = wxT("wxITEM_CHECK");
        break;
    }

    if(ID == wxT("wxID_SEPARATOR")) {
        code << GetWindowParent() << wxT("->AppendSeparator();\n");

    } else {
        label << PropertyString(PROP_LABEL);
        wxString accel = PropertyString(PROP_ACCELERATOR).Trim().Trim(false);
        if(!accel.IsEmpty()) {
            label << wxT("\\t") << accel;
        }
        label.Trim().Trim(false);

        code << GetName() << wxT(" = new ") << GetWxClassName() << wxT("(") << GetWindowParent() << wxT(", ")
             << WindowID() << wxT(", ") << wxCrafter::UNDERSCORE(label) << wxT(", ")
             << wxCrafter::UNDERSCORE(PropertyString(PROP_HELP)) << wxT(", ");

        code << actualKind << wxT(");\n");

        wxString bmpcode = wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH));
        if(
#if defined(__WXGTK__)
            (toolType ==
             wxCrafter::TOOL_TYPE_NORMAL) && // gtk asserts if you try to give a bitmap to a check/radio menuitem
#endif
            (bmpcode != wxT("wxNullBitmap"))) {
            code << GetName() << wxT("->SetBitmap(") << bmpcode << wxT(");\n");
        }

        code << GetWindowParent() << wxT("->Append(") << GetName() << wxT(");\n");

        if((actualKind == wxT("wxITEM_CHECK")) && (PropertyString(PROP_CHECKED) == "1")) {
            code << GetName() << "->Check();\n";
        }
    }
    return code;
}

void MenuItemWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/menu.h>")); }

wxString MenuItemWrapper::GetWxClassName() const { return wxT("wxMenuItem"); }

void MenuItemWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    bool isSeparator = (WindowID() == wxT("wxID_SEPARATOR") || PropertyString(PROP_KIND) == ITEM_SEPARATOR);

    if(isSeparator) {
        text << wxT("<object class=\"separator\" />");

    } else {
        text << XRCPrefix() << XRCKind() << XRCLabel();
        if(
#if defined(__WXGTK__)
            (PropertyString(PROP_KIND) ==
             ITEM_NORMAL) && // gtk asserts if you try to give a bitmap to a check/radio menuitem
#endif
            (!PropertyFile(PROP_BITMAP_PATH).empty())) {
            text << XRCBitmap();
        }
        text << wxT("<accel>") << wxCrafter::CDATA(PropertyString(PROP_ACCELERATOR)) << wxT("</accel>");
        if((PropertyString(PROP_KIND) == ITEM_CHECK) && (PropertyString(PROP_CHECKED) == "1")) {
            text << wxT("<checked>") << "1" << wxT("</checked>");
        }
        // Add the help string
        text << "<help>" << wxCrafter::CDATA(PropertyString(PROP_HELP)) << "</help>";
        text << XRCSuffix();
    }
}

wxString MenuItemWrapper::XRCKind() const
{
    wxString kind = PropertyString(PROP_KIND);
    if(kind == ITEM_RADIO)
        return wxT("<radio>1</radio>");

    else if(kind == ITEM_CHECK)
        return wxT("<checkable>1</checkable>");

    else
        return wxT("");
}

void MenuItemWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    DoLoadXRCProperties(node);
}

void MenuItemWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    DoLoadXRCProperties(node);
}

void MenuItemWrapper::DoLoadXRCProperties(const wxXmlNode* node)
{
    // Cope with separators. This will be a menuitem, but we must set its type
    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "separator") {
        SetPropertyString(PROP_KIND, ITEM_SEPARATOR);
        return; // Nothing else needed for a separator
    }

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("checkable"));
    if(propertynode && propertynode->GetNodeContent() == "1") {
        SetPropertyString(PROP_KIND, ITEM_CHECK);
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("checked"));
    if(propertynode && propertynode->GetNodeContent() == "1") {
        SetPropertyString(PROP_CHECKED, "1");
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("radio"));
    if(propertynode && propertynode->GetNodeContent() == "1") {
        SetPropertyString(PROP_KIND, ITEM_RADIO);
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "bitmap");
    if(propertynode) {
        ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_MENU");
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "accel");
    if(propertynode) {
        SetPropertyString(PROP_ACCELERATOR, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "help");
    if(propertynode) {
        SetPropertyString(PROP_HELP, propertynode->GetNodeContent());
    }
}

void MenuItemWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // Next cope with separators. This will be a menuitem, but we must set its type
    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "separator") {
        SetPropertyString(PROP_KIND, ITEM_SEPARATOR);
        return; // Nothing else needed for a separator
    }

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "kind");
    if(propertynode) {
        wxString value = propertynode->GetNodeContent();
        if(value == "wxITEM_CHECK") {
            SetPropertyString(PROP_KIND, ITEM_CHECK);
        } else if(value == "wxITEM_RADIO") {
            SetPropertyString(PROP_KIND, ITEM_RADIO);
        } else {
            SetPropertyString(PROP_KIND, ITEM_NORMAL);
        }
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_MENU");
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "shortcut");
    if(propertynode) {
        SetPropertyString(PROP_ACCELERATOR, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "help");
    if(propertynode) {
        SetPropertyString(PROP_HELP, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", wxT("checked"));
    if(propertynode && propertynode->GetNodeContent() == "1") {
        SetPropertyString(PROP_CHECKED, "1");
    }
}
