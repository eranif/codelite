#include "DapDebuggerSettingsDlg.h"

#include "DapSettingsPage.hpp"
#include "globals.h"
#include "windowattrmanager.h"

DapDebuggerSettingsDlg::DapDebuggerSettingsDlg(wxWindow* parent, clDapSettingsStore& store)
    : DapDebuggerSettingsDlgBase(parent)
    , m_store(store)
{
    const auto& entries = m_store.GetEntries();
    for(const auto& vt : entries) {
        m_notebook->AddPage(new DapSettingsPage(m_notebook, m_store, vt.second), vt.first);
    }

    ::clSetSmallDialogBestSizeAndPosition(this);
    WindowAttrManager::Load(this);
}

DapDebuggerSettingsDlg::~DapDebuggerSettingsDlg() { m_store.Store(); }
