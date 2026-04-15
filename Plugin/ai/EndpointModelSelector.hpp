#pragma once

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <memory>
#include <wx/choice.h>
#include <wx/control.h>
#include <wx/window.h>

class WXDLLIMPEXP_SDK EndpointModelSelector : public wxControl
{
public:
    explicit EndpointModelSelector(wxWindow* parent);
    ~EndpointModelSelector() override;

    void UpdateChoices();
    inline bool IsOk() const
    {
        return m_choiceModels->GetSelection() != wxNOT_FOUND && m_choiceEndpoints->GetSelection() != wxNOT_FOUND;
    }

    wxString GetModel() const { return m_choiceModels->GetStringSelection(); }
    wxString GetEndpoint() const { return m_choiceEndpoints->GetStringSelection(); }

private:
    void UpdateModelsForEndpoint(const wxString& endpoint);
    void OnModelChanged(wxCommandEvent& event);
    void OnEndpointChanged(wxCommandEvent& event);
    void OnBusyUI(wxUpdateUIEvent& event);
    void OnLLMConfigUpdate(clLLMEvent& event);

    wxChoice* m_choiceEndpoints{nullptr};
    wxChoice* m_choiceModels{nullptr};

    /**
     * @brief Internal helper to calculate a reasonable control width
     * @param reference_text Text used to calculate the width
     * @return The calculated width in pixels
     */
    int CalculateControlWidth(const wxString& reference_text) const;
};
