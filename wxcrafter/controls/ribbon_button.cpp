#include "ribbon_button.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "file_ficker_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"

RibbonButtonBase::RibbonButtonBase(int type)
    : wxcWidget(type)
{
    wxArrayString kind;
    kind.Add("wxRIBBON_BUTTON_NORMAL");
    kind.Add("wxRIBBON_BUTTON_DROPDOWN");
    kind.Add("wxRIBBON_BUTTON_HYBRID");
    kind.Add("wxRIBBON_BUTTON_TOGGLE");

    m_isButtonBar = !(type == ID_WXRIBBONTOOL || type == ID_WXRIBBONDROPDOWNTOOL || type == ID_WXRIBBONHYBRIDTOOL ||
                      type == ID_WXRIBBONTOGGLETOOL);

    int selection = 0;
    if(type == ID_WXRIBBONBUTTON || type == ID_WXRIBBONTOOL) {
        selection = 0;
    } else if(type == ID_WXRIBBONDROPDOWNBUTTON || type == ID_WXRIBBONDROPDOWNTOOL) {
        selection = 1;
    } else if(type == ID_WXRIBBONHYBRIDBUTTON || type == ID_WXRIBBONHYBRIDTOOL) {
        selection = 2;
    } else if(type == ID_WXRIBBONTOGGLEBUTTON || type == ID_WXRIBBONTOGGLETOOL) {
        selection = 3;
    }

    AddProperty(new StringProperty(PROP_LABEL, _("Button"), _("The button label")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Select the bitmap file")));
    AddProperty(new StringProperty(PROP_HELP, "Help String", _("Help string")));
    AddProperty(new ChoiceProperty(PROP_KIND, kind, selection, _("The button type")));

    wxCrafter::ResourceLoader bl;
    if(m_isButtonBar) {
        m_properties.Item(PROP_BITMAP_PATH)->SetValue(bl.GetPlaceHolderImagePath().GetFullPath());

    } else {
        m_properties.Item(PROP_BITMAP_PATH)->SetValue(bl.GetPlaceHolder16ImagePath().GetFullPath());
    }

    if(m_isButtonBar) {
        RegisterEvent("wxEVT_COMMAND_RIBBONBUTTON_CLICKED", "wxRibbonButtonBarEvent",
                      _("Triggered when the normal (non-dropdown) region of a button on the button bar is clicked."));
        RegisterEvent("wxEVT_COMMAND_RIBBONBUTTON_DROPDOWN_CLICKED", "wxRibbonButtonBarEvent",
                      _("Triggered when the dropdown region of a button on the button bar is clicked. "
                        "wxRibbonButtonBarEvent::PopupMenu() should be called by the event handler if it wants to "
                        "display a popup menu (which is what most dropdown buttons should be doing)."));
        m_namePattern = "m_ribbonButton";

    } else {
        DelProperty(PROP_LABEL);
        RegisterEvent("wxEVT_COMMAND_RIBBONTOOL_CLICKED", "wxRibbonToolBarEvent",
                      _("Triggered when the normal (non-dropdown) region of a tool on the toolbar is clicked."));
        RegisterEvent("wxEVT_COMMAND_RIBBONTOOL_DROPDOWN_CLICKED", "wxRibbonToolBarEvent",
                      _("Triggered when the dropdown region of a tool on the toolbar is clicked. "
                        "wxRibbonToolBarEvent::PopupMenu() should be called by the event handler if it wants to "
                        "display a popup menu (which is what most dropdown buttons should be doing)."));
        m_namePattern = "m_ribbonTool";
    }
    SetName(GenerateName());
}

RibbonButtonBase::~RibbonButtonBase() {}

wxString RibbonButtonBase::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString code;

    if(m_isButtonBar) {
        code << GetParent()->GetName() << "->AddButton(" << GetId() << ", " << Label() << ", "
             << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << ", "
             << wxCrafter::UNDERSCORE(PropertyString(PROP_HELP)) << ", " << PropertyString(PROP_KIND) << ");\n";

    } else {
        code << GetParent()->GetName() << "->AddTool(" << GetId() << ", "
             << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << ", "
             << "wxNullBitmap, " << wxCrafter::UNDERSCORE(PropertyString(PROP_HELP)) << ", "
             << PropertyString(PROP_KIND) << ");\n";
    }
    return code;
}

void RibbonButtonBase::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString RibbonButtonBase::GetWxClassName() const { return ""; }

void RibbonButtonBase::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(m_isButtonBar) {
        text << "<object class=\"button\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\">";

    } else {
        text << "<object class=\"tool\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\">";
    }

    text << XRCBitmap() << XRCLabel();

    wxString kind = PropertyString(PROP_KIND);
    if(kind == "wxRIBBON_BUTTON_DROPDOWN") {
        text << "<dropdown>1</dropdown>";

    } else if(kind == "wxRIBBON_BUTTON_HYBRID") {
        text << "<hybrid>1</hybrid>";
    }

    text << XRCSuffix();
}

wxString RibbonButtonBase::GetCppName() const { return GetParent()->GetName(); }

// ----------------------------------------------

RibbonButton::RibbonButton()
    : RibbonButtonBase(ID_WXRIBBONBUTTON)
{
}

RibbonButton::~RibbonButton() {}

// ----------------------------------------------

RibbonButtonHybrid::RibbonButtonHybrid()
    : RibbonButtonBase(ID_WXRIBBONHYBRIDBUTTON)
{
}

RibbonButtonHybrid::~RibbonButtonHybrid() {}

// -----------------------------------------------

RibbonButtonDropdown::RibbonButtonDropdown()
    : RibbonButtonBase(ID_WXRIBBONDROPDOWNBUTTON)
{
}

RibbonButtonDropdown::~RibbonButtonDropdown() {}

// -------------------------------------------

RibbonButtonToggle::RibbonButtonToggle()
    : RibbonButtonBase(ID_WXRIBBONTOGGLEBUTTON)
{
}

RibbonButtonToggle::~RibbonButtonToggle() {}

// -------------------------------------------
RibbonTool::RibbonTool()
    : RibbonButtonBase(ID_WXRIBBONTOOL)
{
}

RibbonTool::~RibbonTool() {}

// -------------------------------------------

RibbonToolToggle::RibbonToolToggle()
    : RibbonButtonBase(ID_WXRIBBONTOGGLETOOL)
{
}

RibbonToolToggle::~RibbonToolToggle() {}

// -------------------------------------------

RibbonToolHybrid::RibbonToolHybrid()
    : RibbonButtonBase(ID_WXRIBBONHYBRIDTOOL)
{
}

RibbonToolHybrid::~RibbonToolHybrid() {}

// -------------------------------------------

RibbonToolDropdown::RibbonToolDropdown()
    : RibbonButtonBase(ID_WXRIBBONDROPDOWNTOOL)
{
}

RibbonToolDropdown::~RibbonToolDropdown() {}
