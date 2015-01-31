#include "clStatusBar.h"
#include "macros.h"
#include "event_notifier.h"
#include "ieditor.h"
#include "imanager.h"
#include "drawingutils.h"
#include <wx/settings.h>
#include "codelite_events.h"
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clStatusBarArtNormal : public wxCustomStatusBarArt
{
public:
    clStatusBarArtNormal()
    {
        m_textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_textShadowColour = DrawingUtils::GetPanelBgColour();
        m_penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        m_bgColour = DrawingUtils::GetPanelBgColour();
    }
    virtual ~clStatusBarArtNormal() {}
};

clStatusBar::clStatusBar(wxWindow* parent, IManager* mgr)
    : wxCustomStatusBar(parent)
    , m_mgr(mgr)
{
    SetArt(wxCustomStatusBarArt::Ptr_t(new clStatusBarArtNormal));
    
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clStatusBar::OnUpdateColour, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clStatusBar::OnUpdateColour, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clStatusBar::OnUpdateColour, this);
    
    // Add 2 text fields (in addition to the main one)
    wxCustomStatusBarField::Ptr_t messages(new wxCustomStatusBarFieldText(300));
    AddField(messages);

    wxCustomStatusBarField::Ptr_t lineCol(new wxCustomStatusBarFieldText(250));
    AddField(lineCol);

    wxCustomStatusBarField::Ptr_t language(new wxCustomStatusBarFieldText(100));
    AddField(language);
}

clStatusBar::~clStatusBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clStatusBar::OnUpdateColour, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clStatusBar::OnUpdateColour, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clStatusBar::OnUpdateColour, this);
}

void clStatusBar::SetMessage(const wxString& message, int col)
{
    if(col == -1) {
        SetText(message);

    } else {
        wxCustomStatusBarField::Ptr_t field = GetField(col);
        CHECK_PTR_RET(field);
        field->Cast<wxCustomStatusBarFieldText>()->SetText(message);
        Refresh();
    }
}

void clStatusBar::OnUpdateColour(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    wxCustomStatusBarArt::Ptr_t art(NULL);
    if(editor) {
        wxColour bgColour = editor->GetSTC()->StyleGetBackground(0);
        if(DrawingUtils::IsDark(bgColour)) {
            // Using dark theme background
            art.reset(new wxCustomStatusBarArt);
            SetArt(art);
        } else {
            art.reset(new clStatusBarArtNormal);
            SetArt(art);
        }
    } else {
        // Non editor, set "normal" art
        art.reset(new clStatusBarArtNormal);
        SetArt(art);
    }
    Refresh();
}
