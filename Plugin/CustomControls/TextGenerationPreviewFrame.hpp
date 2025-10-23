#pragma once

#include "CustomControls.hpp"
#include "IndicatorPanel.hpp"
#include "MarkdownStyler.hpp"
#include "codelite_exports.h"

enum class PreviewKind {
    kDefault,
    kCommentGeneration,
};

class WXDLLIMPEXP_SDK TextGenerationPreviewFrame : public TextGenerationPreviewFrameBase
{
public:
    TextGenerationPreviewFrame(PreviewKind kind, wxWindow* parent = nullptr);
    ~TextGenerationPreviewFrame() override;

    void UpdateProgress(const wxString& message);
    void StartProgress(const wxString& message);
    void StopProgress(const wxString& message);
    void AppendText(const wxString& text);
    void Reset();
    void InitialiseFor(PreviewKind kind);

protected:
    void OnCloseWindow(wxCloseEvent& event) override;
    void OnClose(wxCommandEvent& event) override;
    void OnCopy(wxCommandEvent& event) override;
    void OnCopyUI(wxUpdateUIEvent& event) override;

    PreviewKind m_kind{PreviewKind::kDefault};
    IndicatorPanel* m_indicator_panel{nullptr};
};
