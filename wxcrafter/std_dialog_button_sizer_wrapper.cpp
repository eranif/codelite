#include "std_dialog_button_sizer_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "button_wrapper.h"
#include "wxgui_defs.h"
#include "xmlutils.h"
#include <map>

StdDialogButtonSizerWrapper::StdDialogButtonSizerWrapper()
    : wxcWidget(ID_WXSTDDLGBUTTONSIZER)
{
    m_namePattern = "m_stdBtnSizer";
    SetName(GenerateName());
}

StdDialogButtonSizerWrapper::~StdDialogButtonSizerWrapper() {}

wxcWidget* StdDialogButtonSizerWrapper::Clone() const { return new StdDialogButtonSizerWrapper(); }

wxString StdDialogButtonSizerWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "();\n";
    return cppCode;
}

void StdDialogButtonSizerWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/sizer.h>"); }

wxString StdDialogButtonSizerWrapper::GetWxClassName() const { return "wxStdDialogButtonSizer"; }

void StdDialogButtonSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix();
    ChildrenXRC(text, type);
    text << XRCSuffix();
}

void StdDialogButtonSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // There aren't any extras needed for the sdbsizer itself but, unlike XRC, wxFB stores the button info here
    // They aren't treated as real buttons as does wxC; they're just nodes containing 0 or 1
    typedef std::pair<wxString, wxString> Pair_t;
    typedef std::vector<Pair_t> Vector_t;
    static Vector_t s_buttons;
    if(s_buttons.empty()) {
        s_buttons.push_back(Pair_t("OK", "wxID_OK"));
        s_buttons.push_back(Pair_t("Cancel", "wxID_CANCEL"));
        s_buttons.push_back(Pair_t("Apply", "wxID_APPLY"));
        s_buttons.push_back(Pair_t("Help", "wxID_HELP"));
        s_buttons.push_back(Pair_t("ContextHelp", "wxID_CONTEXT_HELP"));
        s_buttons.push_back(Pair_t("Yes", "wxID_YES"));
        s_buttons.push_back(Pair_t("No", "wxID_NO"));
        s_buttons.push_back(Pair_t("Save", "wxID_SAVE"));
        s_buttons.push_back(Pair_t("Close", "wxID_CLOSE"));
    }

    for(Vector_t::iterator iter = s_buttons.begin(); iter != s_buttons.end(); ++iter) {
        wxString value, buttonname = (*iter).first;
        wxXmlNode* child = node->GetChildren();
        while(child) {
            wxString childname(child->GetName());
            if(childname == wxT("property")) {
                wxString attr(XmlUtils::ReadString(child, wxT("name")));

                if(attr == buttonname) {
                    value = child->GetNodeContent();
                    if(value == "1") {
                        wxcWidget* sbwrapper = Allocator::Instance()->Create(ID_WXSTDBUTTON);
                        wxCHECK_RET(sbwrapper, wxT("Failed to create a stdbtnwrapper"));
                        // We've created a button; afaict there're no styles/properties to add to it. Just set its ID
                        sbwrapper->SetId((*iter).second);
                        AddChild(sbwrapper);
                    }
                }
            }
            child = child->GetNext();
        }
    }
}
