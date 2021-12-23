#include "clAuiCaptionEnabler.h"

#include "codelite_events.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "macros.h"

clAuiCaptionEnabler::clAuiCaptionEnabler()
    : m_aui(NULL)
    , m_window(NULL)
    , m_timer(NULL)
    , m_captionAdded(false)
    , m_ensureCaptionsVisible(false)
{
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiCaptionEnabler::OnEditorSettingsChanged, this);
}

clAuiCaptionEnabler::~clAuiCaptionEnabler()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiCaptionEnabler::OnEditorSettingsChanged, this);
    if(m_timer) {
        m_timer->Stop();
        Unbind(wxEVT_TIMER, &clAuiCaptionEnabler::OnTimer, this, m_timer->GetId());
    }
    wxDELETE(m_timer);
}

void clAuiCaptionEnabler::Initialize(wxWindow* window, const wxString& windowCaption, wxAuiManager* aui)
{
#ifdef __WXMSW__
    m_ensureCaptionsVisible = EditorConfigST::Get()->GetOptions()->IsEnsureCaptionsVisible();
    m_window = window;
    m_aui = aui;
    m_caption = windowCaption;

    CHECK_PTR_RET(m_window);
    CHECK_PTR_RET(m_aui);
    CHECK_PTR_RET(!m_caption.IsEmpty());

    m_timer = new wxTimer(this);
    m_timer->Start(100);
    Bind(wxEVT_TIMER, &clAuiCaptionEnabler::OnTimer, this, m_timer->GetId());
#endif
}

void clAuiCaptionEnabler::OnTimer(wxTimerEvent& event)
{
    if(m_ensureCaptionsVisible) {
        // Do the logic here
        // mousePt in screen coords
        wxPoint mousePt = ::wxGetMousePosition();
        wxAuiPaneInfo& pi = m_aui->GetPane(m_caption);
        if(pi.IsOk() && !pi.IsFloating() && !pi.HasCaption()) {
            // rect in screen coords
            wxRect rect = m_window->GetScreenRect();
            // No caption, make it visible
            if(rect.Contains(mousePt)) {
                pi.CaptionVisible();
                m_aui->Update();
                m_captionAdded = true;
            }
        } else if(m_captionAdded && pi.IsOk() && !pi.IsFloating() && pi.HasCaption()) {
            wxRect rect = m_window->GetScreenRect();
            rect.Inflate(30); // Incase the mouse is at the top of the window
            // Remove the caption if needed
            if(!rect.Contains(mousePt)) {
                m_captionAdded = false;
                pi.CaptionVisible(false);
                m_aui->Update();
            }
        }
    }
}

void clAuiCaptionEnabler::OnEditorSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
#ifdef __WXMSW__
    m_ensureCaptionsVisible = EditorConfigST::Get()->GetOptions()->IsEnsureCaptionsVisible();
#endif
}
