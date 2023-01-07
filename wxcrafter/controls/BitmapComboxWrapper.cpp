#include "BitmapComboxWrapper.h"

#include "BitmapTextArrayProperty.h"
#include "BmpTextSelectorDlg.h"
#include "allocator_mgr.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

BitmapComboxWrapper::BitmapComboxWrapper()
    : wxcWidget(ID_WXBITMAPCOMBOBOX)
{
    SetPropertyString(_("Common Settings"), "wxBitmapComboBox");
    AddProperty(new BitmapTextArrayProperty(PROP_CB_CHOICES, "", _("Combobox drop down choices")));
    AddProperty(new StringProperty(
        PROP_SELECTION, "-1",
        _("The zero-based position of any initially selected string, or -1 if none are to be selected")));
    AddProperty(new StringProperty(PROP_VALUE, "", _("The combobox initial value")));

    RegisterEventCommand("wxEVT_COMMAND_COMBOBOX_SELECTED",
                         _("Process a wxEVT_COMMAND_COMBOBOX_SELECTED event, when an item on the list is selected. "
                           "Note that calling GetValue returns the new value of selection."));
    RegisterEventCommand("wxEVT_COMMAND_TEXT_UPDATED",
                         _("Process a wxEVT_COMMAND_TEXT_UPDATED event, when the combobox text changes."));
    RegisterEventCommand("wxEVT_COMMAND_TEXT_ENTER",
                         _("Process a wxEVT_COMMAND_TEXT_ENTER event, when <RETURN> is pressed in the combobox."));

    PREPEND_STYLE(wxCB_READONLY, false);
    PREPEND_STYLE(wxCB_SORT, false);
    PREPEND_STYLE(wxTE_PROCESS_ENTER, false);

    m_namePattern = "m_bmpComboBox";
    SetName(GenerateName());
}

BitmapComboxWrapper::~BitmapComboxWrapper() {}

wxcWidget* BitmapComboxWrapper::Clone() const { return new BitmapComboxWrapper(); }

wxString BitmapComboxWrapper::CppCtorCode() const
{
    wxArrayString labels, bitmaps;
    wxString options = PropertyString(PROP_CB_CHOICES);
    BmpTextVec_t arr = BmpTextSelectorDlg::FromString(options);
    for(size_t i = 0; i < arr.size(); ++i) {
        wxcCodeGeneratorHelper::Get().AddBitmap(arr.at(i).first);
        bitmaps.Add(wxcCodeGeneratorHelper::Get().BitmapCode(arr.at(i).first));
        labels.Add(arr.at(i).second);
    }
    wxString code;
    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID()
         << ", wxEmptyString"
         << ", wxDefaultPosition, " << SizeAsString() << ", wxArrayString(), " << StyleFlags("0") << ");\n";

    // Append the items
    for(size_t i = 0; i < labels.GetCount(); ++i) {
        code << GetName() << "->Append(" << wxCrafter::UNDERSCORE(labels.Item(i)) << ", " << bitmaps.Item(i) << ");\n";
    }
    int sel = PropertyInt(PROP_SELECTION);
    if(sel != wxNOT_FOUND && sel < (int)labels.GetCount()) {
        code << GetName() << "->SetSelection(" << sel << ");\n";
    }
    return code;
}

void BitmapComboxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/bmpcbox.h>"); }

wxString BitmapComboxWrapper::GetWxClassName() const { return "wxBitmapComboBox"; }

void BitmapComboxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node) { wxUnusedVar(node); }
void BitmapComboxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node) { wxUnusedVar(node); }
void BitmapComboxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node) { wxUnusedVar(node); }

void BitmapComboxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString options = PropertyString(PROP_CB_CHOICES);
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();
    BmpTextVec_t arr = BmpTextSelectorDlg::FromString(options);
    for(size_t i = 0; i < arr.size(); ++i) {
        text << "<object class=\"ownerdrawnitem\">";
        text << "<text>" << wxCrafter::CDATA(arr.at(i).second) << "</text>";
        text << XRCBitmap("bitmap", arr.at(i).first);
        text << "</object>";
    }

    if(!HasStyle(wxCB_READONLY) && !PropertyString(PROP_HINT).empty()) {
        // set hint for non-readonly combobox
        text << "<hint>" << wxCrafter::CDATA(PropertyString(PROP_HINT)) << "</hint>";
    }
    text << XRCSelection() << XRCSuffix();
}
