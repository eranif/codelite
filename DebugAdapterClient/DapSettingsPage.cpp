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
    AddProperty(_("Environment"), m_entry.GetEnvironment(), UPDATE_TEXT_CB(SetEnvironment));

    AddHeader(_("File path options"));
    AddProperty(_("Use relative paths"), m_entry.UseRelativePath(), UPDATE_BOOL_CB(SetUseRelativePath));
    AddProperty(_("Use forward slash"), m_entry.UseForwardSlash(), UPDATE_BOOL_CB(SetUseForwardSlash));
#ifdef __WXMSW__
    AddProperty(_("Use volume"), m_entry.UseVolume(), UPDATE_BOOL_CB(SetUseVolume));
#endif
}

DapSettingsPage::~DapSettingsPage() {}
