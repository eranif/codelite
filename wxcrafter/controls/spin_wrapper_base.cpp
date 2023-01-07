#include "spin_wrapper_base.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

SpinWrapperBase::SpinWrapperBase(int ID)
    : wxcWidget(ID)
{
}

SpinWrapperBase::~SpinWrapperBase() {}

void SpinWrapperBase::EnsureSaneValues(wxString& min, wxString& max, int& imin, int& imax, int& val) const
{
    // Protect against empty range fields, which would otherwise generate code like spin->SetRange(0, );
    // Also against max < min, which annoys gtk so much it throws its toys out of the pram and segfaults
    min = PropertyString(PROP_MINVALUE);
    if(min.empty()) { min = "0"; }
    max = PropertyString(PROP_MAXVALUE);
    if(max.empty()) { max = "100"; }
    imin = wxCrafter::ToNumber(min, 0);
    imax = wxCrafter::ToNumber(max, 100);
    if(imax < imin) {
        imax = imin + 100; // Well, why not?
        max = wxCrafter::ToString(imax);
    }
    // For 'value', ensure it's in range, otherwise gtk will be unhappy
    val = PropertyInt(PROP_VALUE);
    if(val < imin) {
        val = imin;
    } else if(val > imax) {
        val = imax;
    }
}

void SpinWrapperBase::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("min"));
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("max"));
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void SpinWrapperBase::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("min"));
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("max"));
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void SpinWrapperBase::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "min");
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "max");
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    // XRC only does "value". wxFB does this plus initial. I presume they mean the same thing; perhaps 'value' sets the
    // spinctrl textctrl too
    propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) {
        wxString value = propertynode->GetNodeContent();
        if(!value.empty()) {
            SetPropertyString(PROP_VALUE, value);
        } else {
            propertynode = XmlUtils::FindNodeByName(node, "property", "initial");
            if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
        }
    }
}
