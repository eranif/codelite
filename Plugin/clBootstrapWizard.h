//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clBootstrapWizard.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLBOOSTRAPWIZARD_H
#define CLBOOSTRAPWIZARD_H
#include "clBoostrapWizardBase.h"
#include "codelite_exports.h"
#include <wx/bitmap.h>
#include "ICompilerLocator.h"

class WXDLLIMPEXP_SDK clBootstrapData 
{
public:
    wxString selectedTheme;
    int whitespaceVisibility;
    bool useTabs;
    ICompilerLocator::CompilerVec_t compilers;
};

class WXDLLIMPEXP_SDK clBootstrapWizard : public clBoostrapWizardBase
{
    ICompilerLocator::CompilerVec_t m_compilers;
public:
    static wxBitmap GenerateBitmap(size_t labelIndex);

public:
    clBootstrapWizard(wxWindow* parent);
    virtual ~clBootstrapWizard();
    clBootstrapData GetData();
    wxArrayString GetSelectedPlugins();
    wxArrayString GetUnSelectedPlugins();
        
    bool IsRestartRequired();
    
protected:
    virtual void OnPluginSelectionChanged(wxDataViewEvent& event);
    virtual void OnCancelWizard(wxCommandEvent& event);
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnCheckAllPluginsUI(wxUpdateUIEvent& event);
    virtual void OnToggleCxxPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPlugins(wxCommandEvent& event);
    virtual void OnUnCheckAllPluginsUI(wxUpdateUIEvent& event);
    virtual void OnInstallCompiler(wxCommandEvent& event);
    virtual void OnInstallCompilerUI(wxUpdateUIEvent& event);
    virtual void OnScanForCompilers(wxCommandEvent& event);
    virtual void OnThemeSelected(wxCommandEvent& event);
};
#endif // CLBOOSTRAPWIZARD_H
