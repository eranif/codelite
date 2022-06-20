#ifndef DAPSETTINGSPAGE_HPP
#define DAPSETTINGSPAGE_HPP

#include "clDapSettingsStore.hpp"
#include "clPropertiesPage.hpp"

class DapSettingsPage : public clPropertiesPage
{
    clDapSettingsStore& m_store;
    DapEntry m_entry;

public:
    DapSettingsPage(wxWindow* win, clDapSettingsStore& store, const DapEntry& entry);
    virtual ~DapSettingsPage();
};

#endif // DAPSETTINGSPAGE_HPP
