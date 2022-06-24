#include "DapSettingsPage.hpp"

#if USE_SFTP
#include "sftp_settings.h"
#endif

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
    AddHeader(m_entry.GetName() + _(" settings"));
    AddProperty(_("Command"), m_entry.GetCommand(), UPDATE_TEXT_CB(SetCommand));
    AddProperty(_("Connection string"), m_entry.GetConnectionString(), UPDATE_TEXT_CB(SetConnectionString));
    AddProperty(_("Environment"), m_entry.GetEnvironment(), UPDATE_TEXT_CB(SetEnvironment));
    AddProperty(_("Use relative paths"), m_entry.UseRelativePath(), UPDATE_BOOL_CB(SetUseRelativePath));

    std::vector<wxString> path_choices = { "Native", "Linux" };
    AddProperty(_("Path format"), path_choices, m_entry.IsUsingUnixPath() ? 1 : 0,
                [this](const wxString& label, const wxAny& value) {
                    wxUnusedVar(label);
                    wxString str_value;
                    if(value.GetAs(&str_value)) {
                        DapEntry d;
                        m_store.Get(m_entry.GetName(), &d);
                        d.SetUseUnixPath(str_value == "Linux");
                        m_store.Set(d);
                    }
                });

#if USE_SFTP
    SFTPSettings settings;
    settings.Load();

    wxArrayString choices;
    choices.reserve(settings.GetAccounts().size());

    for(const auto& account : settings.GetAccounts()) {
        choices.Add(account.GetName());
    }

    AddProperty(_("Is Remote"), m_entry.IsRemote(), UPDATE_BOOL_CB(SetRemote));
    AddProperty(_("SSH Account"), choices, m_entry.GetSshAccount(), UPDATE_TEXT_CB(SetSshAccount));
#else
#endif
}

DapSettingsPage::~DapSettingsPage() {}
