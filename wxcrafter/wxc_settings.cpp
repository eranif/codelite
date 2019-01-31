#include "FreeTrialVersionDlg.h"
#include "serial_number.h"
#include "wxc_settings.h"
#include "wxgui_helpers.h"
#include <wx/app.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

wxcSettings::wxcSettings()
    : m_flags(0)
    , m_ts(time(NULL))
    , m_initCompleted(false)
{
    Load();
}

wxcSettings::~wxcSettings() {}

wxcSettings& wxcSettings::Get()
{
    static wxcSettings _settings;
    return _settings;
}

void wxcSettings::Load()
{
    wxFileName fn(wxCrafter::GetConfigFile());
    JSONRoot root(fn);
    if(root.isOk()) {
        time_t nowT = wxDateTime::Now().GetTicks();
        m_flags = root.toElement().namedObject(wxT("m_annoyDialogs")).toInt(m_flags);
        m_flags &= ~USE_TABBED_MODE;
        m_sashPosition = root.toElement().namedObject(wxT("m_sashPosition")).toInt(150);
        m_secondarySashPos = root.toElement().namedObject(wxT("m_secondarySashPos")).toInt(-1);
        m_treeviewSashPos = root.toElement().namedObject(wxT("m_treeviewSashPos")).toInt(-1);
        m_username = root.toElement().namedObject("username").toString();
        m_serialNumber = root.toElement().namedObject("serialNumber").toString();
        m_history = root.toElement().namedObject("recentFiles").toArrayString();
        m_ts = root.toElement().namedObject("activationCode").toInt(nowT);

        JSONElement arr = root.toElement().namedObject(wxT("m_templateClasses"));
        m_templateClasses.clear();
        for(int i = 0; i < arr.arraySize(); ++i) {
            CustomControlTemplate templateControl;
            templateControl.FromJSON(arr.arrayItem(i));
            templateControl.SetControlId(::wxNewEventType());
            m_templateClasses.insert(std::make_pair(templateControl.GetClassName(), templateControl));
        }
    }
}

void wxcSettings::Save()
{
    wxFileName fn(wxCrafter::GetConfigFile());

    JSONRoot root(cJSON_Object);
    m_flags &= ~USE_TABBED_MODE;
    root.toElement().addProperty(wxT("m_annoyDialogs"), (int)m_flags);
    root.toElement().addProperty(wxT("m_sashPosition"), m_sashPosition);
    root.toElement().addProperty(wxT("m_secondarySashPos"), m_secondarySashPos);
    root.toElement().addProperty(wxT("m_treeviewSashPos"), m_treeviewSashPos);
    root.toElement().addProperty("serialNumber", m_serialNumber);
    root.toElement().addProperty("username", m_username);
    root.toElement().addProperty("recentFiles", m_history);
    root.toElement().addProperty("activationCode", (int)m_ts);

    JSONElement arr = JSONElement::createArray(wxT("m_templateClasses"));
    root.toElement().append(arr);

    CustomControlTemplateMap_t::const_iterator iter = m_templateClasses.begin();
    for(; iter != m_templateClasses.end(); ++iter) {
        arr.append(iter->second.ToJSON());
    }
    root.save(fn);
}

void wxcSettings::RegisterCustomControl(CustomControlTemplate& cct)
{
    CustomControlTemplateMap_t::iterator iter = m_templateClasses.find(cct.GetClassName());
    if(iter != m_templateClasses.end()) { m_templateClasses.erase(iter); }
    cct.SetControlId(::wxNewEventType());
    m_templateClasses.insert(std::make_pair(cct.GetClassName(), cct));
}

CustomControlTemplate wxcSettings::FindByControlId(int controlId) const
{
    CustomControlTemplateMap_t::const_iterator iter = m_templateClasses.begin();
    for(; iter != m_templateClasses.end(); ++iter) {
        if(iter->second.GetControlId() == controlId) { return iter->second; }
    }
    return CustomControlTemplate();
}

CustomControlTemplate wxcSettings::FindByControlName(const wxString& name) const
{
    CustomControlTemplateMap_t::const_iterator iter = m_templateClasses.find(name);
    if(iter == m_templateClasses.end()) return CustomControlTemplate();
    return iter->second;
}

void wxcSettings::DeleteCustomControl(const wxString& name)
{
    CustomControlTemplateMap_t::iterator iter = m_templateClasses.find(name);
    if(iter == m_templateClasses.end()) return;

    m_templateClasses.erase(iter);
}

void wxcSettings::MergeCustomControl(const JSONElement& arr)
{
    for(int i = 0; i < arr.arraySize(); ++i) {
        CustomControlTemplate templateControl;
        templateControl.FromJSON(arr.arrayItem(i));
        templateControl.SetControlId(::wxNewEventType());

        if(m_templateClasses.count(templateControl.GetClassName())) {
            // an instance with this name already exists, replace it
            m_templateClasses.erase(templateControl.GetClassName());
        }
        m_templateClasses.insert(std::make_pair(templateControl.GetClassName(), templateControl));
    }

    // Apply the changes
    Save();
}

JSONElement wxcSettings::GetCustomControlsAsJSON(const wxArrayString& controls) const
{
    JSONElement arr = JSONElement::createArray(wxT("m_templateClasses"));
    CustomControlTemplateMap_t::const_iterator iter = m_templateClasses.begin();
    for(; iter != m_templateClasses.end(); ++iter) {
        if(controls.Index(iter->first) != wxNOT_FOUND) arr.append(iter->second.ToJSON());
    }
    return arr;
}

bool wxcSettings::IsLicensed() const { return true; }

bool wxcSettings::IsLicensed2() const
{
    // SerialNumber sn;
    // return sn.Isvalid2(m_serialNumber, m_username) && (m_flags & LICENSE_ACTIVATED);
    return IsLicensed();
}

bool wxcSettings::IsRegistered() const
{
    SerialNumber sn;
    bool isLicensed = sn.IsValid(m_serialNumber, m_username) && (m_flags & LICENSE_ACTIVATED);
    return isLicensed;
}

#define ONE_HOUR (60 * 60 * 1)

void wxcSettings::ShowNagDialogIfNeeded()
{
#if STANDALONE_BUILD
    if(!IsRegistered()) {
        // Free trial version
        static time_t lastCheck = 0;
        time_t curtime = time(NULL);
        if(((curtime - lastCheck) > ONE_HOUR) || true) {
            lastCheck = curtime;
            FreeTrialVersionDlg dlg(wxCrafter::TopFrame());
            dlg.ShowModal();
            switch(dlg.GetAnswer()) {
            case FreeTrialVersionDlg::kPurchase:
                ::wxLaunchDefaultBrowser("http://wxcrafter.codelite.org/checkout.php");
                break;
            default:
                break;
            }
        }
    }
#else
    // CodeLite's plugin, nothing to be done here
    return;
#endif
}

// ----------------------------------------------------------------------
// CustomControlTemplate
// ----------------------------------------------------------------------

CustomControlTemplate::CustomControlTemplate()
    : m_controlId(wxNOT_FOUND)
{
}

CustomControlTemplate::~CustomControlTemplate() {}

void CustomControlTemplate::FromJSON(const JSONElement& json)
{
    m_includeFile = json.namedObject("m_includeFile").toString();
    m_allocationLine = json.namedObject("m_allocationLine").toString();
    m_className = json.namedObject("m_className").toString();
    m_xrcPreviewClass = json.namedObject("m_xrcPreviewClass").toString();
    m_events = json.namedObject("m_events").toStringMap();
}

JSONElement CustomControlTemplate::ToJSON() const
{
    JSONElement obj = JSONElement::createObject();
    obj.addProperty("m_includeFile", m_includeFile);
    obj.addProperty("m_allocationLine", m_allocationLine);
    obj.addProperty("m_className", m_className);
    obj.addProperty("m_xrcPreviewClass", m_xrcPreviewClass);
    obj.addProperty("m_events", m_events);
    return obj;
}
