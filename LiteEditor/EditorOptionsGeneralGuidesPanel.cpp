#include "EditorOptionsGeneralGuidesPanel.h"

#include "cl_config.h"
#include "editor_config.h"

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader("Line numbers");
    AddProperty("Display line numbers", m_options->GetDisplayLineNumbers(), UPDATE_BOOL_CB(SetDisplayLineNumbers));
    AddProperty("Highlight current line number", m_options->IsLineNumberHighlightCurrent(),
                UPDATE_BOOL_CB(SetLineNumberHighlightCurrent));
    AddProperty("Use relative line numbers", m_options->GetRelativeLineNumbers(),
                UPDATE_BOOL_CB(SetRelativeLineNumbers));

    AddHeader("What to highlight?");
    AddProperty("Indentation lines", m_options->GetShowIndentationGuidelines(),
                UPDATE_BOOL_CB(SetShowIndentationGuidelines));
    AddProperty("Matching braces", m_options->GetHighlightMatchedBraces(), UPDATE_BOOL_CB(SetHighlightMatchedBraces));
    AddProperty("Modified lines", m_options->IsTrackChanges(), UPDATE_BOOL_CB(SetTrackChanges));

    AddHeader("Caret line");
    AddProperty("Enable background colour", m_options->GetHighlightCaretLine(), UPDATE_BOOL_CB(SetHighlightCaretLine));
    AddProperty("Background colour", m_options->GetCaretLineColour(), UPDATE_COLOUR_CB(SetCaretLineColour));

    AddHeader(_("Right margin indicator"));
    AddProperty(_("Show right margin indicator"), m_options->IsShowRightMarginIndicator(),
                UPDATE_BOOL_CB(SetShowRightMarginIndicator));
    AddProperty(_("Indicator column"), m_options->GetRightMarginColumn(), UPDATE_INT_CB(SetRightMarginColumn));

    AddHeader("Debugger line");
    AddProperty("Enable background colour", m_options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line),
                UPDATE_OPTION_CB(Opt_Mark_Debugger_Line));
    AddProperty("Background colour", m_options->GetDebuggerMarkerLine(), UPDATE_COLOUR_CB(SetDebuggerMarkerLine));

    AddHeader("Whitespace");
    {
        wxArrayString options;
        options.Add("Invisible");
        options.Add("Visible always");
        options.Add("Visible after indentation");
        AddProperty("Indentation visibility", options, m_options->GetShowWhitspaces(),
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
        wxArrayString options;
        options.Add("Default");
        options.Add("Mac (CR)");
        options.Add("Unix (LF)");
        AddProperty("EOL Style", options, m_options->GetEolMode(), UPDATE_TEXT_CB(SetEolMode));
    }

    long line_spacing = clConfig::Get().Read("extra_line_spacing", (int)0);
    AddProperty("Line spacing", line_spacing, [&](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        long spacing;
        if(value.GetAs(&spacing)) {
            clConfig::Get().Write("extra_line_spacing", static_cast<int>(spacing));
        }
    });
}

EditorOptionsGeneralGuidesPanel::~EditorOptionsGeneralGuidesPanel() {}
