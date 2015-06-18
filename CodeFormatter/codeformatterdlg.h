//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatterdlg.h
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
#ifndef __codeformatterdlg__
#define __codeformatterdlg__

#include "codeformatterdlgbase.h"
#include "formatoptions.h"

class IManager;
class CodeFormatter;

class CodeFormatterDlg : public CodeFormatterBaseDlg
{
    FormatOptions m_options;
    CodeFormatter* m_cf;
    wxString m_sampleCode;
    bool m_isDirty;
    IManager* m_mgr;

protected:
    virtual void OnChoicephpformatterChoiceSelected(wxCommandEvent& event);
    virtual void OnPHPCSFixerOptionsUpdated(wxStyledTextEvent& event);
    virtual void OnPharFileSelected(wxFileDirPickerEvent& event);
    virtual void OnPhpFileSelected(wxFileDirPickerEvent& event);
    virtual void OnChoicecxxengineChoiceSelected(wxCommandEvent& event);
    virtual void OnFormatOnSave(wxCommandEvent& event);
    virtual void OnPgmgrastylePgChanged(wxPropertyGridEvent& event);
    virtual void OnPgmgrclangPgChanged(wxPropertyGridEvent& event);
    virtual void OnPgmgrphpPgChanged(wxPropertyGridEvent& event);
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnCustomAstyleFlags(wxCommandEvent& event);
    virtual void OnApplyUI(wxUpdateUIEvent& event);
    void OnOK(wxCommandEvent& e);
    void OnHelp(wxCommandEvent& e);
    void InitDialog();
    void UpdatePreview();

public:
    /** Constructor */
    CodeFormatterDlg(wxWindow* parent,
                     IManager* mgr,
                     CodeFormatter* cf,
                     const FormatOptions& opts,
                     const wxString& sampleCode);
    ~CodeFormatterDlg();
    FormatOptions GetOptions() const { return m_options; }
};

#endif // __codeformatterdlg__
