#include "DapSettingsPage.hpp"

#define UPDATE_TEXT_CB(SETTER_METHOD)                   \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        wxString str_value;                             \
        if(value.GetAs(&str_value)) {                   \
            DapEntry d;                                 \
            m_store.Get(m_entry.GetName(), &d);         \
            d.SETTER_METHOD(str_value);                 \
            m_store.Set(d);                             \
        }                                               \
    }

#define UPDATE_BOOL_CB(SETTER_METHOD)                   \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        bool bool_value;                                \
        if(value.GetAs(&bool_value)) {                  \
            DapEntry d;                                 \
            m_store.Get(m_entry.GetName(), &d);         \
            d.SETTER_METHOD(bool_value);                \
            m_store.Set(d);                             \
        }                                               \
    }

DapSettingsPage::DapSettingsPage(wxWindow* win, clDapSettingsStore& store, const DapEntry& entry)
    : clPropertiesPage(win)
    , m_store(store)
    , m_entry(entry)
{
    AddHeader(_("General"));
    AddProperty(_("Command"), m_entry.GetCommand(), UPDATE_TEXT_CB(SetCommand));
    AddProperty(_("Connection string"), m_entry.GetConnectionString(), UPDATE_TEXT_CB(SetConnectionString));

    const std::vector<wxString> launch_choices = { "Launch", "Attach" };
    wxString launch_type = entry.GetLaunchType() == DapLaunchType::LAUNCH ? "Launch" : "Attach";
    AddProperty(_("Launch type"), launch_choices, launch_type, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString value_str;
        if(value.GetAs(&value_str)) {
            DapEntry d;
            m_store.Get(m_entry.GetName(), &d);
            if(value_str == "Launch") {
                d.SetLaunchType(DapLaunchType::LAUNCH);
            } else {
                d.SetLaunchType(DapLaunchType::ATTACH);
            }
            m_store.Set(d);
        }
    });

    AddHeader(_("File path options"));
    AddProperty(_("Use relative paths"), m_entry.UseRelativePath(), UPDATE_BOOL_CB(SetUseRelativePath));

#ifdef __WXMSW__
    AddProperty(_("Use forward slash"), m_entry.UseForwardSlash(), UPDATE_BOOL_CB(SetUseForwardSlash));
    AddProperty(_("Use volume"), m_entry.UseVolume(), UPDATE_BOOL_CB(SetUseVolume));
#endif

    AddHeader(_("Environment variables"));
    AddProperty(_("Server environment"), m_entry.GetEnvironment(), UPDATE_TEXT_CB(SetEnvironment));

    const std::vector<wxString> format_choices = { "List", "Dictionary", "None" };
    int sel = wxNOT_FOUND;
    switch(m_entry.GetEnvFormat()) {
    case dap::EnvFormat::LIST:
        sel = 0;
        break;
    case dap::EnvFormat::DICTIONARY:
        sel = 1;
        break;
    case dap::EnvFormat::NONE:
        sel = 2;
        break;
    }

    AddProperty(_("Environment format"), format_choices, sel, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString value_str;
        if(value.GetAs(&value_str)) {
            DapEntry d;
            m_store.Get(m_entry.GetName(), &d);
            if(value_str == "List") {
                d.SetEnvFormat(dap::EnvFormat::LIST);
            } else if(value_str == "Dictionary") {
                d.SetEnvFormat(dap::EnvFormat::DICTIONARY);
            } else {
                d.SetEnvFormat(dap::EnvFormat::NONE);
            }
            m_store.Set(d);
        }
    });
}

DapSettingsPage::~DapSettingsPage() {}
