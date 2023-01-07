#include "RearrangeListWrapper.h"

#include "allocator_mgr.h"
#include "wxgui_helpers.h"

RearrangeListWrapper::RearrangeListWrapper()
{
    SetPropertyString(_("Common Settings"), "wxRearrangeList");
    SetType(ID_WXREARRANGELIST);
    m_namePattern = "m_rearrangelist";
    SetName(GenerateName());
}

RearrangeListWrapper::~RearrangeListWrapper() {}

wxcWidget* RearrangeListWrapper::Clone() const { return new RearrangeListWrapper(); }

void RearrangeListWrapper::LoadPropertiesFromXRC(const wxXmlNode* node) { wxUnusedVar(node); }

void RearrangeListWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node) { wxUnusedVar(node); }

void RearrangeListWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node) { wxUnusedVar(node); }

void RearrangeListWrapper::ToXRC(wxString& text, XRC_TYPE type) const { CheckListBoxWrapper::ToXRC(text, type); }

wxString RearrangeListWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), ";");

    wxString optionsArr, orderArr;
    optionsArr << GetName() << "Arr";
    orderArr << GetName() << "OrderArr";

    code << "wxArrayInt " << orderArr << ";\n";
    code << "wxArrayString " << optionsArr << ";\n";
    for(size_t i = 0; i < options.GetCount(); i++) {
        code << optionsArr << ".Add(" << wxCrafter::UNDERSCORE(options.Item(i)) << ");\n";
        code << orderArr << ".Add(" << i << ");\n";
    }

    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID() << ", "
         << "wxDefaultPosition, " << SizeAsString() << ", " << orderArr << ", " << optionsArr << ", " << StyleFlags("0")
         << ");\n";
    code << CPPCommonAttributes();
    return code;
}

void RearrangeListWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/rearrangectrl.h>");
}

wxString RearrangeListWrapper::GetWxClassName() const { return "wxRearrangeList"; }
