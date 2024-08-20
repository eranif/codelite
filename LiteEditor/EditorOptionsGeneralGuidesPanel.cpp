#include "EditorOptionsGeneralGuidesPanel.h"

#include "StdToWX.h"
#include "cl_config.h"
#include "editor_config.h"

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Line numbers"));
    AddProperty(_("Display line numbers"), m_options->GetDisplayLineNumbers(), UPDATE_BOOL_CB(SetDisplayLineNumbers));
    AddProperty(_("Highlight current line number"), m_options->IsLineNumberHighlightCurrent(),
                UPDATE_BOOL_CB(SetLineNumberHighlightCurrent));
    AddProperty(_("Use relative line numbers"), m_options->GetRelativeLineNumbers(),
                UPDATE_BOOL_CB(SetRelativeLineNumbers));

    AddHeader(_("What to highlight?"));
    AddProperty(_("Indentation lines"), m_options->GetShowIndentationGuidelines(),
                UPDATE_BOOL_CB(SetShowIndentationGuidelines));
    AddProperty(_("Matching braces"), m_options->GetHighlightMatchedBraces(),
                UPDATE_BOOL_CB(SetHighlightMatchedBraces));
    AddProperty(_("Modified lines"), m_options->IsTrackChanges(), UPDATE_BOOL_CB(SetTrackChanges));

    AddHeader(_("Caret line"));
    AddProperty(_("Highlight caret line"), m_options->GetHighlightCaretLine(), UPDATE_BOOL_CB(SetHighlightCaretLine));
#if wxCHECK_VERSION(3, 3, 0)
    AddProperty(_("Highlight with colour"), m_options->IsHighlightCaretLineWithColour(),
                UPDATE_BOOL_CB(SetHighlightCaretLineWithColour));
#endif
    AddProperty(_("Background colour"), m_options->GetCaretLineColour(), UPDATE_COLOUR_CB(SetCaretLineColour));

    AddHeader(_("Right margin indicator"));
    AddProperty(_("Show right margin indicator"), m_options->IsShowRightMarginIndicator(),
                UPDATE_BOOL_CB(SetShowRightMarginIndicator));
    AddProperty(_("Indicator column"), m_options->GetRightMarginColumn(), UPDATE_INT_CB(SetRightMarginColumn));

    AddHeader(_("Debugger line"));
    AddProperty(_("Highlight debugger line"), m_options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line),
                UPDATE_OPTION_CB(Opt_Mark_Debugger_Line));
    AddProperty(_("Background colour"), m_options->GetDebuggerMarkerLine(), UPDATE_COLOUR_CB(SetDebuggerMarkerLine));

    AddHeader(_("Whitespace"));
    {
        wxArrayString options;
        options.Add(_("Invisible"));
        options.Add(_("Visible always"));
        options.Add(_("Visible after indentation"));
        AddProperty(_("Indentation visibility"), options, m_options->GetShowWhitspaces(),
                    [this, options](const wxString& label, const wxAny& value) {
                        wxString value_str;
                        if(value.GetAs(&value_str)) {
                            size_t where = options.Index(value_str);
                            if(where != wxString::npos) {
                                m_options->SetShowWhitspaces(static_cast<int>(where));
                            }
                        }
                    });
    }

    {
        const wxArrayString options = StdToWX::ToArrayString({ "Default", "Mac (CR)", "Unix (LF)", "Windows (CRLF)" });
        AddProperty(_("EOL Style"), options, m_options->GetEolMode(), UPDATE_TEXT_CB(SetEolMode));
    }

    long line_spacing = clConfig::Get().Read("extra_line_spacing", (int)0);
    AddProperty(_("Line spacing"), line_spacing, [&](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        long spacing;
        if(value.GetAs(&spacing)) {
            clConfig::Get().Write("extra_line_spacing", static_cast<int>(spacing));
        }
    });
}

EditorOptionsGeneralGuidesPanel::~EditorOptionsGeneralGuidesPanel() {}
