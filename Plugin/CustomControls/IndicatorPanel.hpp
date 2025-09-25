#pragma once

#include "codelite_exports.h"

#include <wx/activityindicator.h>
#include <wx/panel.h>
#include <wx/stattext.h>

class WXDLLIMPEXP_SDK IndicatorPanel : public wxPanel
{
public:
    IndicatorPanel(wxWindow* parent, const wxString& initialText = wxEmptyString);
    ~IndicatorPanel() override = default;

    void Start(const wxString& message);
    void Stop(const wxString& message);
    bool IsRunning() const;
    void SetMessage(const wxString& message);

private:
    wxActivityIndicator* m_activityIndicator{nullptr};
    wxStaticText* m_statusMessage{nullptr};
};

struct WXDLLIMPEXP_SDK IndicatorPanelLocker {
    IndicatorPanel* m_indicatorPanel{nullptr};
    wxString m_stop_message;
    explicit IndicatorPanelLocker(IndicatorPanel* ip, const wxString& message, const wxString& stop_message)
        : m_indicatorPanel(ip)
        , m_stop_message(stop_message)
    {
        if (m_indicatorPanel && !m_indicatorPanel->IsRunning()) {
            m_indicatorPanel->Start(message);
        } else {
            m_indicatorPanel->SetMessage(message);
        }
    }

    ~IndicatorPanelLocker()
    {
        if (m_indicatorPanel && m_indicatorPanel->IsRunning()) {
            m_indicatorPanel->Stop(m_stop_message);
        }
    }
};
