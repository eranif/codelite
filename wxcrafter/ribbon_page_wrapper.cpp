#include "ribbon_page_wrapper.h"

#include "allocator_mgr.h"
#include "bool_property.h"
#include "file_ficker_property.h"
#include "string_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"

#include <wx/ribbon/page.h>

RibbonPageWrapper::RibbonPageWrapper()
    : wxcWidget(ID_WXRIBBONPAGE)
    , m_selected(false)
{
    SetPropertyString(_("Common Settings"), "wxRibbonPage");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Page Icon")));
    AddProperty(new StringProperty(PROP_LABEL, "Page", _("Page Label")));
    AddProperty(new BoolProperty(PROP_SELECTED, false, _("Selected")));
    m_namePattern = "m_ribbonPage";
    SetName(GenerateName());
}

RibbonPageWrapper::~RibbonPageWrapper() {}

wxcWidget* RibbonPageWrapper::Clone() const { return new RibbonPageWrapper(); }

wxString RibbonPageWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << GetId() << ", "
            << Label() << ", " << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << ", "
            << StyleFlags("0") << ");\n";
    cppCode << CPPCommonAttributes();
    if(PropertyBool(PROP_SELECTED) == "true") {
        cppCode << GetParent()->GetName() << "->SetActivePage( " << GetName() << " );\n";
    }
    return cppCode;
}

void RibbonPageWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/ribbon/page.h>"); }

wxString RibbonPageWrapper::GetWxClassName() const { return "wxRibbonPage"; }

void RibbonPageWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCBitmap("icon") << XRCLabel() << XRCCommonAttributes() << XRCStyle() << XRCSize()
         << "<selected>" << (m_selected ? 1 : 0) << "</selected>";

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString RibbonPageWrapper::DoGenerateCppCtorCode_End() const
{
    // return wxString() << GetName() << "->Realize();\n\n";
    return "";
}

void RibbonPageWrapper::Select(bool b) { m_selected = b; }

bool RibbonPageWrapper::IsSelected() const { return m_selected; }
