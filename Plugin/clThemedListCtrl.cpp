#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "clThemedListCtrl.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include <wx/settings.h>

#ifdef __WXMSW__
#define LIST_STYLE wxDV_ENABLE_SEARCH | wxBORDER_SIMPLE | wxDV_ROW_LINES
#else
#define LIST_STYLE wxDV_ENABLE_SEARCH | wxBORDER_DEFAULT | wxDV_ROW_LINES
#endif

#if 0
static bool SortFunc(clRowEntry* a, clRowEntry* b)
{
    const wxString& label_a = a->GetLabel();
    const wxString& label_b = b->GetLabel();
    return (label_b.CmpNoCase(label_a) < 0);
}
#endif

clThemedListCtrlBase::clThemedListCtrlBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                           long style)
    : clDataViewListCtrl(parent, id, pos, size, (style | LIST_STYLE))
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrlBase::OnThemeChanged, this);
    SetNativeTheme(true);
    ApplyTheme();

    // Enable keyboard search
    m_keyboard.reset(new clTreeKeyboardInput(this));
}

clThemedListCtrlBase::~clThemedListCtrlBase()
{
    m_keyboard.reset(nullptr);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clThemedListCtrlBase::OnThemeChanged, this);
}

void clThemedListCtrlBase::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void clThemedListCtrlBase::ApplyTheme()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    clColours colours;
    if(lexer->IsDark()) {
        colours.InitFromColour(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    } else {
        colours.InitDefaults();
    }

    wxColour baseColour = colours.GetBgColour();
    bool useCustomColour = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustomColour) {
        baseColour = clConfig::Get().Read("BaseColour", baseColour);
        colours.InitFromColour(baseColour);
    }

    // Set the built-in search colours
    wxColour highlightColur = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour textColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    colours.SetMatchedItemBgText(highlightColur);
    colours.SetMatchedItemText(textColour);

    // When not using custom colours, use system defaults
    if(!useCustomColour) {
        wxColour selColour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        colours.SetSelItemBgColour(selColour);
        colours.SetSelItemTextColour(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        colours.SetSelItemBgColourNoFocus(selColour.ChangeLightness(150));
        colours.SetSelItemTextColourNoFocus(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    }
    this->SetColours(colours);
}

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
