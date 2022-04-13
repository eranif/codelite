#include "clThemedListCtrl.h"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "clThemedCtrl.hpp"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "lexer_configuration.h"

#include <wx/settings.h>

#ifdef __WXMSW__
#define LIST_STYLE wxDV_ENABLE_SEARCH | wxBORDER_SIMPLE | wxDV_ROW_LINES
#else
#define LIST_STYLE wxDV_ENABLE_SEARCH | wxBORDER_NONE | wxDV_ROW_LINES
#endif

clThemedListCtrlBase::clThemedListCtrlBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                           long style)
    : clDataViewListCtrl(parent, id, pos, size, (style | LIST_STYLE))
{
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedListCtrlBase::OnThemeChanged, this);
    ApplyTheme();

    // Enable keyboard search
    m_keyboard.reset(new clTreeKeyboardInput(this));
}

clThemedListCtrlBase::~clThemedListCtrlBase()
{
    m_keyboard.reset(nullptr);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clThemedListCtrlBase::OnThemeChanged, this);
}

void clThemedListCtrlBase::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedListCtrlBase::ApplyTheme() { cl::ApplyTheme<clThemedListCtrlBase>(this); }

clThemedListCtrl::~clThemedListCtrl() {}

clThemedListCtrl::clThemedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clThemedListCtrlBase(parent, id, pos, size, (style | LIST_STYLE))
{
    SetSortFunction(nullptr);
}

clThemedOrderedListCtrl::~clThemedOrderedListCtrl() {}

clThemedOrderedListCtrl::clThemedOrderedListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                                 const wxSize& size, long style)
    : clThemedListCtrlBase(parent, id, pos, size, (style | LIST_STYLE))
{
    SetSortFunction(nullptr);
}
