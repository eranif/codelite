#include "RearrangeListWrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"

RearrangeListWrapper::RearrangeListWrapper()
{
    SetPropertyString("wxCheckListBox", "wxRearrangeList");
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
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));

    wxString optionsArr, orderArr;
    optionsArr << GetName() << wxT("Arr");
    orderArr << GetName() << wxT("OrderArr");

    code << wxT("wxArrayInt ") << orderArr << wxT(";\n");
    code << wxT("wxArrayString ") << optionsArr << wxT(";\n");
    for(size_t i = 0; i < options.GetCount(); i++) {
        code << optionsArr << ".Add(" << wxCrafter::WXT(options.Item(i)) << ");\n";
        code << orderArr << ".Add(" << i << ");\n";
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << orderArr << ", " << optionsArr
         << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();
    return code;
}

void RearrangeListWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/rearrangectrl.h>");
}

wxString RearrangeListWrapper::GetWxClassName() const { return "wxRearrangeList"; }
