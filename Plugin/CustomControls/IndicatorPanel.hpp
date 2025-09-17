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
