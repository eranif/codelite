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

DapSettingsPage::DapSettingsPage(wxWindow* win, clDapSettingsStore& store, const DapEntry& entry)
    : clPropertiesPage(win)
    , m_store(store)
    , m_entry(entry)
{
    AddHeader(m_entry.GetName() + _(" settings"));
    AddProperty(_("Command"), m_entry.GetCommand(), UPDATE_TEXT_CB(SetCommand));
    AddProperty(_("Connection string"), m_entry.GetConnectionString(), UPDATE_TEXT_CB(SetConnectionString));
    AddProperty(_("Environment"), m_entry.GetEnvironment(), UPDATE_TEXT_CB(SetEnvironment));
}

DapSettingsPage::~DapSettingsPage() {}
