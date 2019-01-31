#include "tool_bar_item_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "event_notifier.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "menu_item_wrapper.h"
#include "menu_wrapper.h"
#include "multi_strings_property.h"
#include "winid_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/aui/auibar.h>

wxDECLARE_EVENT(wxEVT_EVENTS_PROPERTIES_UPDATED, wxCommandEvent);

ToolBarItemWrapper::ToolBarItemWrapper(int type)
    : wxcWidget(type)
{
    // id, label, bitmap, kind, tooltip, help string
    RegisterEventCommand(wxT("wxEVT_COMMAND_TOOL_CLICKED"),
                         _("Process a wxEVT_COMMAND_TOOL_CLICKED event (a synonym for wxEVT_COMMAND_MENU_SELECTED). "
                           "Pass the id of the tool"));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TOOL_DROPDOWN_CLICKED"),
                         _("Process a wxEVT_COMMAND_TOOL_DROPDOWN_CLICKED event"));

    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    wxCrafter::ResourceLoader bmps;
    AddProperty(new CategoryProperty(_("Common Settings"), "wxToolBarItem"));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_NAME, "", _("C++ variable name")));

    AddProperty(new CategoryProperty(_("ToolBar Item")));
    AddProperty(new StringProperty(PROP_LABEL, _("Tool Label"), _("The tool label")));
    AddProperty(new MultiStringsProperty(PROP_TOOLTIP, _("Tooltip"), wxT("\\n"),
                                         _("Short help string. This will appear as the tool's tooltip")));
    AddProperty(new StringProperty(PROP_HELP, "", _("Long help string. This will be displayed in the statusbar")));
    AddProperty(
        new BitmapPickerProperty(PROP_BITMAP_PATH, bmps.GetPlaceHolder16ImagePath().GetFullPath(), _("Bitmap")));
    AddProperty(new FilePickerProperty(PROP_DISABLED_BITMAP_PATH, "", _("Disabled bitmap")));
    AddProperty(new ChoiceProperty(PROP_KIND, wxCrafter::GetToolTypes(true), 0, _("The tool kind")));
    AddProperty(
        new BoolProperty(PROP_DROPDOWN_MENU, true, _("Use wxCrafter to construct the menu called by the dropdown")));

    m_namePattern = wxT("m_toolbarItem");
    SetName(GenerateName());
}

ToolBarItemWrapper::~ToolBarItemWrapper() {}

wxcWidget* ToolBarItemWrapper::Clone() const { return new ToolBarItemWrapper(); }

static wxString GenerateCppCtorCodeRecursively(wxcWidget* widget) // Helper function for CppCtorCode() dropdown menus
{
    wxString code;
    if(wxCrafter::GetToolType(widget->PropertyString(PROP_KIND)) == wxCrafter::TOOL_TYPE_SEPARATOR) {
        code << widget->CppCtorCode();
    } else {
        wxString childcode = widget->CppCtorCode();
        code << childcode;
    }
    wxcWidget::List_t::const_iterator iter = widget->GetChildren().begin();
    for(; iter != widget->GetChildren().end(); ++iter) {
        if(wxCrafter::GetToolType((*iter)->PropertyString(PROP_KIND)) != wxCrafter::TOOL_TYPE_SEPARATOR) {
            wxString childcode = (*iter)->CppCtorCode();
            code << childcode;
        }
        wxcWidget::List_t::const_iterator childiter = (*iter)->GetChildren().begin();
        for(; childiter != (*iter)->GetChildren().end(); ++childiter) {
            code << GenerateCppCtorCodeRecursively(*childiter);
        }
    }

    return code;
}

wxString ToolBarItemWrapper::CppCtorCode() const
{
    wxString code;
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_DISABLED_BITMAP_PATH));

    bool isAuiItem = IsParentAuiToolbar();
    wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(PropertyString(PROP_KIND));
    wxCrafter::TOOL_TYPE realToolType = toolType;

    if(isAuiItem && toolType == wxCrafter::TOOL_TYPE_DROPDOWN) {
        /// wxAuiTooBar does not support wxITEM_DROPDOWN
        /// convert the type to wxITEM_NORMAL
        toolType = wxCrafter::TOOL_TYPE_NORMAL;
    }

    wxString functionArgs;
    wxString disabledBmp = wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_DISABLED_BITMAP_PATH));
    functionArgs << WindowID() << wxT(", ") << CPPLabel() << wxT(", ")
                 << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(", ") << disabledBmp
                 << ", " << wxCrafter::ToolTypeToWX(toolType) << ", "
                 << wxString(wxCrafter::UNDERSCORE(PropertyString(PROP_TOOLTIP))) << wxT(", ")
                 << wxString(wxCrafter::UNDERSCORE(PropertyString(PROP_HELP))) << wxT(", ") << "NULL";

    wxString functionName = wxT("AddTool");

    switch(toolType) {
    case wxCrafter::TOOL_TYPE_SEPARATOR:
        functionName = wxT("AddSeparator");
        functionArgs.Clear();
        break;

    default:
    case wxCrafter::TOOL_TYPE_NORMAL:
    case wxCrafter::TOOL_TYPE_RADIO:
    case wxCrafter::TOOL_TYPE_CHECK:
    case wxCrafter::TOOL_TYPE_DROPDOWN:
        functionName = wxT("AddTool");
        break;
    }

    code << GetWindowParent() << "->" << functionName << wxT("(") << functionArgs << wxT(");\n");
    if(realToolType == wxCrafter::TOOL_TYPE_DROPDOWN && isAuiItem) {
        // wxAuiToolBarItem* AddTool(...) is >= wx3.0, so we have to locate the new tool ourselves
        code << "wxAuiToolBarItem* " << GetName() << " = " << GetWindowParent() << "->FindToolByIndex("
             << GetWindowParent() << "->GetToolCount()-1);\n";
        code << "if (" << GetName() << ") {\n";
        code << "    " << GetName() << "->SetHasDropDown(true);\n";
        if(HasDefaultDropdown()) {
            wxcWidget* menu = GetChildren().front();
            wxASSERT(menu && menu->GetType() == ID_WXMENU);
            if(menu && menu->GetType() == ID_WXMENU) {
                wxString menuname = menu->GetName();
                code << "    " << menuname << " = new wxMenu;\n";
                if(menu && menu->GetType() == ID_WXMENU) {
                    // Add each of the wxcWidget menu's children to the generated menu
                    wxcWidget* me = menu->GetParent();
                    menu->SetParent(NULL); // Allow menu code to be emitted here; there's code to prevent this if the
                                           // parent's a dropdown
                    wxString menucode;
                    wxcWidget::List_t::const_iterator iter = menu->GetChildren().begin();
                    for(; iter != menu->GetChildren().end(); ++iter) {
                        wxString childmenucode =
                            GenerateCppCtorCodeRecursively(*iter); // Must be recursive here; submenus can nest
                        childmenucode.Replace("\n", "\n    ");
                        menucode << childmenucode;
                    }
                    code << "    " << menucode;
                    menu->SetParent(me);
                }
                code << "\n    m_dropdownMenus.insert(std::make_pair( " << GetName()
                     << wxT("->GetId(), " << menuname << ") );\n");
            }
        }
        code << "}\n";
    }

    return code;
}

void ToolBarItemWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/toolbar.h>")); }

wxString ToolBarItemWrapper::GetWxClassName() const { return wxT(""); }

void ToolBarItemWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(PropertyString(PROP_KIND));
    wxString tip = PropertyString(PROP_TOOLTIP);
    wxString help = PropertyString(PROP_HELP);

    if(toolType == wxCrafter::TOOL_TYPE_SEPARATOR) {
        text << wxT("<object class=\"separator\"/>");

    } else if(toolType == wxCrafter::TOOL_TYPE_SPACE) {
        text << wxT("<object class=\"space\"/>");

    } else {
        text << wxT("<object class=\"tool\" name=\"") << GetName() << wxT("\">");

        if(toolType == wxCrafter::TOOL_TYPE_CHECK) {
            text << wxT("<toggle>1</toggle>");

        } else if(toolType == wxCrafter::TOOL_TYPE_RADIO) {
            text << wxT("<radio>1</radio>");

        } else if(toolType == wxCrafter::TOOL_TYPE_DROPDOWN) {
            text << "<dropdown>";
            if(IsParentAuiToolbar()) { ChildrenXRC(text, type); }
            text << "</dropdown>";
        }

        text << XRCLabel();
        if(!tip.empty()) { text << wxT("<tooltip>") << wxCrafter::CDATA(tip) << wxT("</tooltip>"); }
        text << wxT("<longhelp>") << wxCrafter::CDATA(help) << wxT("</longhelp>");
        text << XRCBitmap();
        text << wxT("</object>");
    }
}

void ToolBarItemWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // Next cope with separators. This will be a toolbaritem, but we must set its type
    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "separator") {
        SetPropertyString(PROP_KIND, ITEM_SEPARATOR);
        return; // Nothing else needed for a separator
    }

    if(classname == "space") {
        SetPropertyString(PROP_KIND, ITEM_SPACE);

        // 'space' plus 'width' means an auitoolbar non-stretch spacer
        wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "width");
        if(propertynode) {
            SetPropertyString(PROP_WIDTH, propertynode->GetNodeContent());

        } else {
            // 'space' without 'width' must be a stretchspacer. The default proportion is 1, however an auitoolbar might
            // specify a different one
            wxString ppn = "1";
            propertynode = XmlUtils::FindFirstByTagName(node, "proportion");
            if(propertynode) { ppn = propertynode->GetNodeContent(); }
            SetPropertyString("Proportion:", ppn);
        }
    }

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("toggle"));
    if(propertynode && propertynode->GetNodeContent() == "1") { SetPropertyString(PROP_KIND, ITEM_CHECK); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("radio"));
    if(propertynode && propertynode->GetNodeContent() == "1") { SetPropertyString(PROP_KIND, ITEM_RADIO); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("hasdropdown")); // The previous way
    if(propertynode && propertynode->GetNodeContent() == "1") { SetPropertyString(PROP_KIND, ITEM_DROPDOWN); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("dropdown"));
    if(propertynode) {
        SetPropertyString(PROP_KIND, ITEM_DROPDOWN);
        ImportFromXrc imp(NULL);
        imp.ProcessNamedNode(propertynode, this, "wxMenu"); // This should load any contained dropdown menu

        UpdateRegisteredEventsIfNeeded();
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "bitmap");
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_TOOLBAR"); }

    propertynode = XmlUtils::FindFirstByTagName(node, "bitmap2");
    if(propertynode) {
        ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_DISABLED_BITMAP_PATH, "wxART_TOOLBAR");
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "longhelp");
    if(propertynode) { SetPropertyString(PROP_HELP, propertynode->GetNodeContent()); }
}

void ToolBarItemWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // Next cope with separators. This will be a toolbaritem, but we must set its type
    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "separator") {
        SetPropertyString(PROP_KIND, ITEM_SEPARATOR);
        return; // Nothing else needed for a separator
    }

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("check")); // Not 'toggle'
    if(propertynode && propertynode->GetNodeContent() == "1") { SetPropertyString(PROP_KIND, ITEM_CHECK); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("radio"));
    if(propertynode && propertynode->GetNodeContent() == "1") { SetPropertyString(PROP_KIND, ITEM_RADIO); }

    propertynode = XmlUtils::FindFirstByTagName(node, "bitmap");
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_TOOLBAR"); }

    propertynode = XmlUtils::FindFirstByTagName(node, "bitmap2");
    if(propertynode) {
        ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_DISABLED_BITMAP_PATH, "wxART_TOOLBAR");
    }

    propertynode = XmlUtils::FindFirstByTagName(node, "longhelp");
    if(propertynode) { SetPropertyString(PROP_HELP, propertynode->GetNodeContent()); }
}

void ToolBarItemWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // Next cope with separators. This will be a toolbaritem, but we must set its type
    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "toolSeparator") {
        SetPropertyString(PROP_KIND, ITEM_SEPARATOR);
        return; // Nothing else needed for a separator
    }

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "kind");
    if(propertynode) { SetPropertyString(PROP_KIND, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_TOOLBAR");
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "statusbar"); // Yes, 'statusbar' is wxFB's 'help' label
    if(propertynode) { SetPropertyString(PROP_HELP, propertynode->GetNodeContent()); }
}

void ToolBarItemWrapper::OnPropertiesUpdated()
{
    if(IsParentAuiToolbar()) {
        if(wxCrafter::GetToolType(PropertyString(PROP_KIND)) == wxCrafter::TOOL_TYPE_DROPDOWN) {
            // If this was a change of type to a dropdown, we need to delete any stale connected event (which will have
            // the wrong signature)
            if(GetChildren().empty()) { RemoveEvent("wxEVT_COMMAND_TOOL_CLICKED"); }
        } else {
            // If this was a change of type from a dropdown, we again need to delete any stale connected event
            if(!GetChildren().empty()) { RemoveEvent("wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN"); }
        }
    }
}

void ToolBarItemWrapper::UpdateRegisteredEventsIfNeeded()
{
    bool isAuiItem = IsParentAuiToolbar();
    wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(PropertyString(PROP_KIND));

    if(isAuiItem && toolType == wxCrafter::TOOL_TYPE_DROPDOWN) {
        // A wxAuiItem AND a dropdown menu item -> show only wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN
        m_controlEvents.Clear();
        RegisterEvent(wxT("wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN"), "wxAuiToolBarEvent",
                      _("Process a wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN event"));

    } else if(isAuiItem) {
        /// Not a dropdown but a wxAuiItem - show only 'Click' event
        m_controlEvents.Clear();
        RegisterEventCommand(wxT("wxEVT_COMMAND_TOOL_CLICKED"),
                             _("Process a wxEVT_COMMAND_TOOL_CLICKED event (a synonym for "
                               "wxEVT_COMMAND_MENU_SELECTED). Pass the id of the tool"));

    } else if(!isAuiItem && !(toolType == wxCrafter::TOOL_TYPE_DROPDOWN)) {
        /// Not a dropdown nor wxAuiItem - show only 'Click' event
        m_controlEvents.Clear();
        RegisterEventCommand(wxT("wxEVT_COMMAND_TOOL_CLICKED"),
                             _("Process a wxEVT_COMMAND_TOOL_CLICKED event (a synonym for "
                               "wxEVT_COMMAND_MENU_SELECTED). Pass the id of the tool"));
    }
}

bool ToolBarItemWrapper::HasDefaultDropdown() const
{
    wxCrafter::TOOL_TYPE toolType = wxCrafter::GetToolType(PropertyString(PROP_KIND));
    return (toolType == wxCrafter::TOOL_TYPE_DROPDOWN) && !m_children.empty();
}

// -------------------------------------------
// Separator item
// -------------------------------------------

ToolBarItemSeparatorWrapper::ToolBarItemSeparatorWrapper()
    : ToolBarItemWrapper(ID_WXTOOLBARITEM_SEPARATOR)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    wxArrayString kinds;
    kinds.Add(ITEM_SEPARATOR);

    AddCategory("ToolBar Item Separator");
    AddProperty(new StringProperty(PROP_NAME, "", "Name"));
    AddProperty(new ChoiceProperty(PROP_KIND, kinds, 0, _("The tool kind")));

    m_namePattern = "m_tbSeparator";
    SetName(GenerateName());
}

ToolBarItemSeparatorWrapper::~ToolBarItemSeparatorWrapper() {}

wxString ToolBarItemSeparatorWrapper::CppCtorCode() const
{
    return wxString() << GetWindowParent() << "->AddSeparator();\n";
}

// -------------------------------------------
// StretchSpace item
// -------------------------------------------

ToolBarItemSpaceWrapper::ToolBarItemSpaceWrapper()
    : ToolBarItemWrapper(ID_WXTOOLBARITEM_STRETCHSPACE)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    wxArrayString kinds;
    kinds.Add(ITEM_SPACE);

    AddCategory("ToolBar Item Space");
    AddProperty(new StringProperty(PROP_NAME, "", "Name"));
    AddProperty(new ChoiceProperty(PROP_KIND, kinds, 0, _("The tool kind")));

    m_namePattern = "m_toolbarSpacer";
    SetName(GenerateName());
}

wxString ToolBarItemSpaceWrapper::CppCtorCode() const
{
    return wxString() << GetWindowParent() << "->AddStretchableSpace();\n";
}

// -------------------------------------------
// AuiNonStretchSpace item
// -------------------------------------------

AuiToolBarItemNonStretchSpaceWrapper::AuiToolBarItemNonStretchSpaceWrapper()
    : ToolBarItemWrapper(ID_WXAUITOOLBARITEM_SPACE)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    AddCategory("AuiToolBar Item Space");
    AddProperty(new StringProperty(PROP_NAME, "", "Name"));
    AddProperty(new StringProperty(PROP_WIDTH, "0", _("The width of the space in pixels")));

    m_namePattern = "m_auitbarNonstretchSpace";
    SetName(GenerateName());
}

wxString AuiToolBarItemNonStretchSpaceWrapper::CppCtorCode() const
{
    return wxString() << GetWindowParent() << "->AddSpacer(" << wxString(PropertyString(PROP_WIDTH)) << ");\n";
}

void AuiToolBarItemNonStretchSpaceWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"space\" name=\"") << GetName() << wxT("\">") << wxT("<width>")
         << PropertyString(PROP_WIDTH) << wxT("</width>") << wxT("</object>");
}

// -------------------------------------------
// AuiStretchSpace item (which has a 'proportion' parameter that's missing in the wxToolBar equivalent)
// -------------------------------------------

AuiToolBarItemSpaceWrapper::AuiToolBarItemSpaceWrapper()
    : ToolBarItemWrapper(ID_WXAUITOOLBARITEM_STRETCHSPACE)
{
    m_properties.DeleteValues();
    m_sizerFlags.Clear();

    AddCategory("ToolBar Item Space");
    AddProperty(new StringProperty(PROP_NAME, "", "Name"));
    AddProperty(new StringProperty("Proportion:", "1", _("How stretchable the space is. The normal value is 1.")));

    m_namePattern = "m_auitbarStretchSpace";
    SetName(GenerateName());
}

wxString AuiToolBarItemSpaceWrapper::CppCtorCode() const
{
    return wxString() << GetWindowParent() << "->AddStretchSpacer(" << PropertyString("Proportion:") << ");\n";
}

void AuiToolBarItemSpaceWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"space\" name=\"") << GetName() << wxT("\">") << wxT("<proportion>")
         << PropertyString("Proportion:") << wxT("</proportion>") << wxT("</object>");
}
// -------------------------------------------
// AuiLabel item
// -------------------------------------------

AuiToolBarLabelWrapper::AuiToolBarLabelWrapper(int type)
    : wxcWidget(type)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    wxCrafter::ResourceLoader bmps;
    AddProperty(new CategoryProperty("wxAuiToolBar Label"));
    AddProperty(new WinIdProperty());
    AddProperty(new StringProperty(PROP_NAME, "", _("C++ variable name")));
    AddProperty(new StringProperty(PROP_LABEL, _("My toolbar label"), _("The label's text")));
    AddProperty(new StringProperty(PROP_WIDTH, "-1", _("Optionally, specify the label's width")));

    m_namePattern = wxT("m_auitbarLabel");
    SetName(GenerateName());
}

wxcWidget* AuiToolBarLabelWrapper::Clone() const { return new AuiToolBarLabelWrapper(); }

wxString AuiToolBarLabelWrapper::CppCtorCode() const
{
    wxString code;

    wxString functionArgs;
    functionArgs << WindowID() << wxT(", ") << CPPLabel() << wxT(", ") << wxString(PropertyString(PROP_WIDTH));
    wxString functionName = "AddLabel";
    code << GetWindowParent() << "->" << functionName << "(" << functionArgs << ");\n";
    return code;
}

void AuiToolBarLabelWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/toolbar.h>"));
}

wxString AuiToolBarLabelWrapper::GetWxClassName() const { return wxT(""); }

void AuiToolBarLabelWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"label\" name=\"") << GetName() << wxT("\">") << XRCLabel() << wxT("<width>")
         << PropertyString(PROP_WIDTH) << wxT("</width>") << wxT("</object>");
}

void AuiToolBarLabelWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    if(classname == "label") {
        wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "width");
        if(propertynode) { SetPropertyString(PROP_WIDTH, propertynode->GetNodeContent()); }
    }
}
