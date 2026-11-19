#include "editable_list_box_wrapper.h"

#include "Properties/multi_strings_property.h"
#include "Properties/string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xml/xmlutils.h"

#include <wx/editlbox.h>

EditableListBoxWrapper::EditableListBoxWrapper()
    : wxcWidget(ID_WXEDITABLELISTBOX)
{
    // wxEL_DEFAULT_STYLE is wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE
    PREPEND_STYLE(wxEL_ALLOW_NEW, true);
    PREPEND_STYLE(wxEL_ALLOW_EDIT, true);
    PREPEND_STYLE(wxEL_ALLOW_DELETE, true);
    PREPEND_STYLE(wxEL_NO_REORDER, false);

    // wxEditableListBox does not send any events of its own: the events come from its child list control

    SetPropertyString(_("Common Settings"), "wxEditableListBox");
    Add<StringProperty>(PROP_LABEL, _("Editable List Box"), _("The label shown above the list"));
    Add<MultiStringsProperty>(PROP_OPTIONS, _("The List Box Items. A semi-colon list of strings"));

    m_namePattern = wxT("m_editableListBox");
    SetName(GenerateName());
}

wxString EditableListBoxWrapper::CppCtorCode() const
{
    wxString code;
    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID() << ", "
         << Label() << ", wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("wxEL_DEFAULT_STYLE") << ");\n";
    code << CPPCommonAttributes();

    const wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), ";");
    if (!options.IsEmpty()) {
        const wxString optionsArr = GetName() + "Arr";
        code << "wxArrayString " << optionsArr << ";\n";
        for (const auto& option : options) {
            code << optionsArr << ".Add(" << wxCrafter::UNDERSCORE(option) << ");\n";
        }
        code << GetName() << "->SetStrings(" << optionsArr << ");\n";
    }
    return code;
}

void EditableListBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/editlbox.h>"); }

wxString EditableListBoxWrapper::GetWxClassName() const { return "wxEditableListBox"; }

void EditableListBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCLabel() << XRCContentItems(false)
         << XRCSuffix();
}

void EditableListBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "label");
    if (propertynode) {
        SetPropertyString(PROP_LABEL, propertynode->GetNodeContent());
    }

    // <content><item>one</item><item>two</item></content>
    propertynode = XmlUtils::FindFirstByTagName(node, "content");
    if (propertynode) {
        wxArrayString items;
        for (wxXmlNode* child = propertynode->GetChildren(); child; child = child->GetNext()) {
            if (child->GetName() == "item") {
                items.Add(child->GetNodeContent());
            }
        }
        SetPropertyString(PROP_OPTIONS, wxCrafter::Join(items, ";"));
    }
}
