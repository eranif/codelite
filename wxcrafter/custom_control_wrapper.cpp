#include "custom_control_wrapper.h"

#include "allocator_mgr.h"
#include "file_logger.h"
#include "wxc_settings.h"
#include "xmlutils.h"

#include <macromanager.h>
#include <wx/regex.h>

CustomControlWrapper::CustomControlWrapper()
    : wxcWidget(ID_WXCUSTOMCONTROL)
{
    DelProperty(_("Control Specific Settings"));
    AddProperty(new CategoryProperty(m_templInfoName, "Custom Control"));
    m_namePattern = wxT("m_custom");

    // Generate a name if there isn't one; but don't overwrite e.g. an imported one
    if(!GetName().empty()) {
        SetName(GenerateName());
    }
}

CustomControlWrapper::~CustomControlWrapper() {}

wxcWidget* CustomControlWrapper::Clone() const { return new CustomControlWrapper(); }

wxString CustomControlWrapper::CppCtorCode() const
{
    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(m_templInfoName);
    if(cct.IsValid() == false)
        return wxT("");

    // support for Subclass
    wxString subclass = PropertyString(PROP_SUBCLASS_NAME);
    subclass.Trim().Trim(false);

    wxString cppCode = cct.GetAllocationLine();
    cppCode.Trim().Trim(false);

    // cppCode contains the allocation line
    // something like:
    // $name = new MyCustomClassName(...);
    static wxRegEx reNewClass("new[ \\t]+([A-Za-z_]{1}[A-Za-z0-9_]*[ \\t]*\\()");

    if(!subclass.IsEmpty() && reNewClass.IsValid() && reNewClass.Matches(cppCode)) {
        reNewClass.Replace(&cppCode, "new " + subclass + "(");
    }

    if(!cppCode.EndsWith(wxT(";"))) {
        cppCode << wxT(";");
    }

    // Replace $name with the actual control c++ name
    cppCode = MacroManager::Instance()->Replace(cppCode, wxT("name"), GetName(), true);
    cppCode = MacroManager::Instance()->Replace(cppCode, wxT("parent"), GetWindowParent(), true);
    cppCode = MacroManager::Instance()->Replace(cppCode, wxT("id"), GetId(), true);
    return cppCode;
}

void CustomControlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(m_templInfoName);
    if(cct.IsValid() == false)
        return;

    wxString inclLine = cct.GetIncludeFile();
    if(inclLine.IsEmpty())
        return;

    inclLine.Trim().Trim(false);
    if(inclLine.EndsWith(wxT(";"))) {
        inclLine.RemoveLast();
    }
    headers.Add(inclLine);
}

wxString CustomControlWrapper::GetWxClassName() const
{
    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(m_templInfoName);
    if(cct.IsValid() == false)
        return wxT("");

    return cct.GetClassName();
}

void CustomControlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(m_templInfoName);
    if(cct.IsValid() && !cct.GetXrcPreviewClass().IsEmpty())
        text << wxT("<object class=\"") << cct.GetXrcPreviewClass() << wxT("\" name=\"") << GetName() << wxT("\">");
    else
        text << wxT("<object class=\"unknown\" name=\"") << GetName() << wxT("\">");

    text << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}

void CustomControlWrapper::Serialize(JSONElement& json) const
{
    wxcWidget::Serialize(json);
    json.addProperty(wxT("m_templInfoName"), m_templInfoName);
    ms_customControlsUsed.insert(
        std::make_pair(m_templInfoName, wxcSettings::Get().FindByControlName(m_templInfoName)));
}

void CustomControlWrapper::UnSerialize(const JSONElement& json)
{
    m_templInfoName = json.namedObject(wxT("m_templInfoName")).toString();
    DoUpdateEvents();
    wxcWidget::UnSerialize(json);
    m_properties.Item("Custom Control")->SetValue(m_templInfoName);
}

/*
void CustomControlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxString wxclassname, include, construction, declaration, name, xrcclassname, settings;
    xrcclassname = XmlUtils::ReadString(node, wxT("name"));     // I suppose that's right: "ID_CUSTOM1"
    name = XmlUtils::ReadString(node, wxT("subclass"));  // I suppose that's right: "MyCustomClass"

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("creating_code"));
    if (propertynode) {
        construction = propertynode->GetNodeContent());
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("include_file"));
    if (propertynode) {
        include = propertynode->GetNodeContent());
    }

    // Hmm. I can't find any way to find the superclass's wx name e.g. wxPanel, so I'll abort this atm
}*/

void CustomControlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxString include, construction, declaration, name, xrcclassname, settings;
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "name");
    if(propertynode) {
        name = propertynode->GetNodeContent();
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "declaration");
    if(propertynode) {
        declaration = propertynode->GetNodeContent();
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "include");
    if(propertynode) {
        include = propertynode->GetNodeContent();
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "construction");
    if(propertynode) {
        construction = propertynode->GetNodeContent();
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "settings");
    if(propertynode) {
        settings = propertynode->GetNodeContent();
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "class");
    if(propertynode) {
        xrcclassname = propertynode->GetNodeContent();
    }

    // The wxFB fields aren't the same as the wxC ones, so we need to do some guesswork here:
    wxString wxclassname = declaration.BeforeFirst('*').Trim(); // Get the real classname from 'MyFoo * m_foo;'

    if(wxclassname.empty() || construction.empty() ||
       declaration.empty()) { // settings and xrcclassname are optional; 'include' probably is too
        clWARNING() << "Failed to load a Custom Control from wxFB: not all necessary data was available" << endl;
        return;
    }

    CustomControlTemplate controlData;
    controlData.SetAllocationLine(construction);
    controlData.SetClassName(wxclassname);
    controlData.SetIncludeFile(include);
    // controlData.Set/**/Class(settings);  wxC doesn't do this atm
    controlData.SetXrcPreviewClass(xrcclassname);
    SetTemplInfoName(wxclassname);
    wxcSettings::Get().RegisterCustomControl(controlData);
    wxcSettings::Get().Save();
}

void CustomControlWrapper::SetTemplInfoName(const wxString& templInfoName)
{
    this->m_templInfoName = templInfoName;
    DoUpdateEvents();
}

void CustomControlWrapper::DoUpdateEvents()
{
    m_controlEvents.Clear();
    m_connectedEvents.Clear();

    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(m_templInfoName);
    if(cct.IsValid() == false)
        return;

    const wxStringMap_t& events = cct.GetEvents();
    wxStringMap_t::const_iterator iter = events.begin();
    for(; iter != events.end(); ++iter) {
        RegisterEvent(iter->first, iter->second, "");
        // m_controlEvents.Add(ConnectDetails(iter->first, iter->second, "", wxString() << iter->second << "Handler"));
    }
}

void CustomControlWrapper::DoDeepCopy(const wxcWidget& rhs, enum DuplicatingOptions nametypesToChange,
                                      const std::set<wxString>& existingNames, const wxString& chosenName,
                                      const wxString& chosenInheritedName, const wxString& chosenFilename)
{
    wxcWidget::DoDeepCopy(rhs, nametypesToChange, existingNames, chosenName, chosenInheritedName, chosenFilename);
    /// copy some custom control specific content
    const CustomControlWrapper& origin = static_cast<const CustomControlWrapper&>(rhs);
    m_templInfoName = origin.m_templInfoName;
}
