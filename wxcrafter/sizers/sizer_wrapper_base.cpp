#include "sizer_wrapper_base.h"

#include "wxgui_helpers.h"

SizerWrapperBase::SizerWrapperBase()
    : wxcWidget(-1)
{
    AddProperty(new BoolProperty(PROP_KEEP_CLASS_MEMBER, false,
                                 _("When enabled, this sizer is kept as a class member and become accessible")));

    DelProperty(PROP_WINDOW_ID);
    DelProperty(PROP_SIZE);
    DelProperty(PROP_BG);
    DelProperty(PROP_FG);
    DelProperty(PROP_FONT);
    DelProperty(PROP_TOOLTIP);
    DelProperty(_("Initial State"));
    DelProperty(PROP_STATE_HIDDEN);
    DelProperty(PROP_STATE_DISABLED);
    DelProperty(PROP_HAS_FOCUS);
    DelProperty(_("Subclass"));
    DelProperty(PROP_SUBCLASS_NAME);
    DelProperty(PROP_SUBCLASS_INCLUDE);

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);
}

SizerWrapperBase::~SizerWrapperBase() {}

void SizerWrapperBase::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/sizer.h>")); }

bool SizerWrapperBase::IsMainSizer() const { return m_parent && !m_parent->IsSizer(); }

wxString SizerWrapperBase::GenerateMinSizeCode() const
{
    wxString code;
    wxSize sz = wxCrafter::DecodeSize(PropertyString(PROP_MINSIZE));
    if(sz != wxDefaultSize) {
        code << GetName() << "->SetMinSize(" << wxCrafter::EncodeSize(sz) << ");\n";
    }
    return code;
}

wxString SizerWrapperBase::GenerateMinSizeXRC() const
{
    if(!IsMainSizer()) {
        return wxEmptyString;
    }

    wxString code;
    wxSize sz = wxCrafter::DecodeSize(m_parent->PropertyString(PROP_MINSIZE));
    if(sz != wxDefaultSize) {
        code << "<minsize>" << wxCrafter::EncodeSize(sz) << "</minsize>\n";
    }
    return code;
}

bool SizerWrapperBase::KeepAsClassMember() const { return IsPropertyChecked(PROP_KEEP_CLASS_MEMBER); }
