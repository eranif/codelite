//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: unittestreport.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_UNITTESTCPP_UNITTESTREPORT_BASE_CLASSES_H
#define _CODELITE_UNITTESTCPP_UNITTESTREPORT_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include "progressctrl.h"
#include <wx/stattext.h>
#include <wx/dataview.h>
#include "clThemedListCtrl.h"
#include <wx/button.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

// clang-format on

class UnitTestsBasePage : public wxPanel
{
protected:
    ProgressCtrl* m_progressPassed;
    ProgressCtrl* m_progressFailed;
    wxStaticText* m_staticText10;
    wxStaticText* m_staticTextTotalTests;
    wxStaticText* m_staticText12;
    wxStaticText* m_staticTextFailTestsNum;
    wxStaticText* m_staticText14;
    wxStaticText* m_staticTextSuccessTestsNum;
    clThemedListCtrl* m_dvListCtrlErrors;
    wxButton* m_button4;

protected:
    virtual void OnItemActivated(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnClearReport(wxCommandEvent& event) { event.Skip(); }

public:
    ProgressCtrl* GetProgressPassed() { return m_progressPassed; }
    ProgressCtrl* GetProgressFailed() { return m_progressFailed; }
    wxStaticText* GetStaticText10() { return m_staticText10; }
    wxStaticText* GetStaticTextTotalTests() { return m_staticTextTotalTests; }
    wxStaticText* GetStaticText12() { return m_staticText12; }
    wxStaticText* GetStaticTextFailTestsNum() { return m_staticTextFailTestsNum; }
    wxStaticText* GetStaticText14() { return m_staticText14; }
    wxStaticText* GetStaticTextSuccessTestsNum() { return m_staticTextSuccessTestsNum; }
    clThemedListCtrl* GetDvListCtrlErrors() { return m_dvListCtrlErrors; }
    wxButton* GetButton4() { return m_button4; }
    UnitTestsBasePage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~UnitTestsBasePage();
};

#endif
