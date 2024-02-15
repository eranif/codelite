#include "clStatusBar.h"

#include "ColoursAndFontsManager.h"
#include "bitmap_loader.h"
#include "clToolBar.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"

#include <algorithm>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/fontenc.h>
#include <wx/fontmap.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

namespace
{
size_t STATUSBAR_PANES_BUTTON_IDX = 0;
size_t STATUSBAR_SCM_IDX = 0;
size_t STATUSBAR_LINE_COL_IDX = 0;
size_t STATUSBAR_ANIMATION_COL_IDX = 0;
size_t STATUSBAR_WHITESPACE_INFO_IDX = 0;
size_t STATUSBAR_EOL_COL_IDX = 0;
size_t STATUSBAR_LANG_COL_IDX = 0;
size_t STATUSBAR_ENCODING_COL_IDX = 0;
size_t STATUSBAR_ICON_COL_IDX = 0;

const wxString SIDEBAR = wxT("Workspace View");
const wxString SECONDARY_SIDEBAR = wxT("Secondary Sidebar");
const wxString BOTTOM_BAR = wxT("Output View");

} // namespace

namespace
{
void GetWhitespaceInfo(wxStyledTextCtrl* ctrl, wxString& whitespace, wxString& eol)
{
    whitespace << (ctrl->GetUseTabs() ? "tabs" : "spaces");
    int eolMode = ctrl->GetEOLMode();
    switch(eolMode) {
    case wxSTC_EOL_CR:
        eol << "CR";
        break;
    case wxSTC_EOL_CRLF:
        eol << "CRLF";
        break;
    case wxSTC_EOL_LF:
        eol << "LF";
        break;
    default:
        break;
    }
}
} // namespace
typedef wxCustomStatusBarArt clStatusBarArtNormal;

clStatusBar::clStatusBar(wxWindow* parent, IManager* mgr)
    : wxCustomStatusBar(parent)
    , m_mgr(mgr)
{
    SetArt(wxCustomStatusBarArt::Ptr_t(new clStatusBarArtNormal("Normal")));

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clStatusBar::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clStatusBar::OnAllEditorsClosed, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTED, &clStatusBar::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &clStatusBar::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clStatusBar::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &clStatusBar::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SHOWING, &clStatusBar::OnGotoAnythingShowing, this);
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clStatusBar::OnActionSelected, this);

    Bind(wxEVT_STATUSBAR_CLICKED, &clStatusBar::OnFieldClicked, this);

    // [spacer][buttons][main text area]..
    wxCustomStatusBarField::Ptr_t button_field(new wxCustomStatusBarControlField(this, CreatePaneButtonsToolbar()));
    STATUSBAR_PANES_BUTTON_IDX = InsertField(0, button_field);

#ifdef __WXMAC__
    // add a spacer first
    InsertField(0, wxCustomStatusBarField::Ptr_t(new wxCustomStatusBarSpacerField(this, 10)));
#endif

    // set the width to include a possible label
    int lable_width = GetTextWidth("_git_source_control_with_long_branch_name");
    wxCustomStatusBarField::Ptr_t sourceControl(
        new wxCustomStatusBarBitmapField(this, clGetScaledSize(30) + lable_width));
    STATUSBAR_SCM_IDX = AddField(sourceControl);

    int lineColWidth = GetTextWidth("Ln 100000, Col 999, Pos 12345678, Len 4821182");
    wxCustomStatusBarField::Ptr_t lineCol(new wxCustomStatusBarFieldText(this, lineColWidth));
    STATUSBAR_LINE_COL_IDX = AddField(lineCol);

    wxCustomStatusBarField::Ptr_t buildAnimation(new wxCustomStatusBarAnimationField(
        this, wxXmlResource::Get()->LoadBitmap("build-animation-sprite"), wxHORIZONTAL, wxSize(80, 7)));
    STATUSBAR_ANIMATION_COL_IDX = AddField(buildAnimation);

    wxCustomStatusBarField::Ptr_t whitespace(new wxCustomStatusBarFieldText(this, clGetScaledSize(80)));
    STATUSBAR_WHITESPACE_INFO_IDX = AddField(whitespace);

    wxCustomStatusBarField::Ptr_t eol(new wxCustomStatusBarFieldText(this, clGetScaledSize(50)));
    STATUSBAR_EOL_COL_IDX = AddField(eol);

    // The longest language that we have is "properties"
    int languageWidth = GetTextWidth("_properties_");
    wxCustomStatusBarField::Ptr_t language(new wxCustomStatusBarFieldText(this, languageWidth));
    STATUSBAR_LANG_COL_IDX = AddField(language);

    wxCustomStatusBarField::Ptr_t encoding(new wxCustomStatusBarFieldText(this, clGetScaledSize(80)));
    STATUSBAR_ENCODING_COL_IDX = AddField(encoding);

    wxCustomStatusBarField::Ptr_t buildStatus(new wxCustomStatusBarBitmapField(this, clGetScaledSize(30)));
    STATUSBAR_ICON_COL_IDX = AddField(buildStatus);

    BitmapLoader* bl = clGetManager()->GetStdIcons();
    m_bmpBuildError = bl->LoadBitmap("error");
    m_bmpBuildWarnings = bl->LoadBitmap("warning");
}

clStatusBar::~clStatusBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clStatusBar::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clStatusBar::OnAllEditorsClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_STARTED, &clStatusBar::OnBuildStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &clStatusBar::OnBuildEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clStatusBar::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &clStatusBar::OnEditorSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SHOWING, &clStatusBar::OnGotoAnythingShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clStatusBar::OnActionSelected, this);
    Unbind(wxEVT_STATUSBAR_CLICKED, &clStatusBar::OnFieldClicked, this);
}

void clStatusBar::SetMessage(const wxString& message, int secondsToLive) { SetText(message, secondsToLive); }

void clStatusBar::OnPageChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateView();
}

void clStatusBar::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateColour();
}

void clStatusBar::DoUpdateColour()
{
    IEditor* editor = m_mgr->GetActiveEditor();
    wxCustomStatusBarArt::Ptr_t art(NULL);
    if(editor) {
        wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
        if(DrawingUtils::IsDark(bgColour)) {
            // Using dark theme background
            art.reset(new clStatusBarArtNormal("Normal"));
            SetArt(art);
        } else {
            art.reset(new clStatusBarArtNormal("Normal"));
            SetArt(art);
        }
    } else {
        // Non editor, set "normal" art
        art.reset(new clStatusBarArtNormal("Normal"));
        SetArt(art);
    }
    Refresh();
}

void clStatusBar::SetLanguage(const wxString& lang)
{
    // Col 2
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_LANG_COL_IDX);
    CHECK_PTR_RET(field);

    wxString ucLang = lang.Upper();
    field->Cast<wxCustomStatusBarFieldText>()->SetText(ucLang);
    field->SetTooltip(lang);
}

void clStatusBar::SetLinePosColumn(const wxString& lineCol) { CallAfter(&clStatusBar::DoSetLinePosColumn, lineCol); }

void clStatusBar::DoSetLinePosColumn(const wxString& message)
{
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_LINE_COL_IDX);
    CHECK_PTR_RET(field);

    auto line_col_text = field->Cast<wxCustomStatusBarFieldText>();
    if(line_col_text->GetText() != message) {
        // new message, set it
        field->Cast<wxCustomStatusBarFieldText>()->SetText(message);
        field->SetTooltip(message);
    }
}

void clStatusBar::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

void clStatusBar::Clear()
{
    SetMessage("");
    SetText("");
    SetBuildBitmap(wxNullBitmap, "");
    StopAnimation();
    SetLanguage("");
    ClearWhitespaceInfo();
    SetEncoding("");
}

void clStatusBar::OnBuildEnded(clBuildEvent& event)
{
    event.Skip();
    StopAnimation();
    if(event.GetErrorCount()) {
        SetBuildBitmap(m_bmpBuildError, _("Build ended with errors\nClick to view"));
    } else if(event.GetWarningCount()) {
        SetBuildBitmap(m_bmpBuildWarnings, _("Build ended with warnings\nClick to view"));
    } else {
        SetBuildBitmap(wxNullBitmap, "");
    }
}

void clStatusBar::OnBuildStarted(clBuildEvent& event)
{
    event.Skip();
    SetBuildBitmap(wxNullBitmap, "");
    StartAnimation(50, "");
}

void clStatusBar::SetBuildBitmap(const wxBitmap& bmp, const wxString& tooltip)
{
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ICON_COL_IDX);
    CHECK_PTR_RET(field);
    field->Cast<wxCustomStatusBarBitmapField>()->SetBitmap(bmp);
    field->SetTooltip(tooltip);
    Refresh();
}

void clStatusBar::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void clStatusBar::StartAnimation(long refreshRate, const wxString& tooltip)
{
    wxUnusedVar(tooltip);
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
    CHECK_PTR_RET(field);
    field->Cast<wxCustomStatusBarAnimationField>()->Start(refreshRate);
    field->SetTooltip(_("Build is in progress\nClick to view the Build Log"));
}

void clStatusBar::StopAnimation()
{
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
    CHECK_PTR_RET(field);
    field->Cast<wxCustomStatusBarAnimationField>()->Stop();
    field->SetTooltip("");
}

void clStatusBar::OnFieldClicked(clCommandEvent& event) { DoFieldClicked(event.GetInt()); }

void clStatusBar::SetWhitespaceInfo()
{
    IEditor* activeEditor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    wxString whitespaceInfo, eolMode;
    GetWhitespaceInfo(activeEditor->GetCtrl(), whitespaceInfo, eolMode);

    {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
        CHECK_PTR_RET(field);

        wxString ws = whitespaceInfo.Upper();
        field->Cast<wxCustomStatusBarFieldText>()->SetText(ws);
        field->SetTooltip(ws);
    }

    {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
        CHECK_PTR_RET(field);

        wxString ws = eolMode.Upper();
        field->Cast<wxCustomStatusBarFieldText>()->SetText(ws);
        field->SetTooltip(ws);
    }
}

void clStatusBar::SetSourceControlBitmap(const wxBitmap& bmp, const wxString& label, const wxString& outputTabName,
                                         const wxString& tooltip)
{
    m_sourceControlTabName = outputTabName;
    m_bmpSourceControl = bmp;

    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_SCM_IDX);
    CHECK_PTR_RET(field);

    field->Cast<wxCustomStatusBarBitmapField>()->SetBitmap(m_bmpSourceControl);
    field->Cast<wxCustomStatusBarBitmapField>()->SetTooltip(tooltip);
    field->Cast<wxCustomStatusBarBitmapField>()->SetLabel(label);
}

void clStatusBar::ClearWhitespaceInfo()
{
    {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
        CHECK_PTR_RET(field);

        field->Cast<wxCustomStatusBarFieldText>()->SetText(wxEmptyString);
        field->SetTooltip(wxEmptyString);
    }

    {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
        CHECK_PTR_RET(field);

        field->Cast<wxCustomStatusBarFieldText>()->SetText(wxEmptyString);
        field->SetTooltip(wxEmptyString);
    }
}

void clStatusBar::SetEncoding(const wxString& enc)
{
    // Col 2
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ENCODING_COL_IDX);
    CHECK_PTR_RET(field);

    wxString ucEnc = enc.Upper();
    field->Cast<wxCustomStatusBarFieldText>()->SetText(ucEnc);
    field->SetTooltip(ucEnc);
}

void clStatusBar::OnEditorSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateView();
}

void clStatusBar::DoUpdateView()
{
    DoUpdateColour();

    // Update the language
    wxString language = "TEXT";
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetFileName().GetFullPath());
        if(lexer) {
            language = lexer->GetName().Upper();
        }
    }

    SetLanguage(language);

    wxString encodingName = wxFontMapper::GetEncodingName(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
    SetEncoding(encodingName);
    SetWhitespaceInfo();
}

void clStatusBar::DoFieldClicked(int fieldIndex)
{
    if(fieldIndex == STATUSBAR_SCM_IDX) {
        if(m_sourceControlTabName.IsEmpty())
            return;
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_SCM_IDX);
        CHECK_PTR_RET(field);
        // Open the output view only if the bitmap is valid
        if(field->Cast<wxCustomStatusBarBitmapField>()->GetBitmap().IsOk()) {
            m_mgr->ToggleOutputPane(m_sourceControlTabName);
        }
    } else if(fieldIndex == STATUSBAR_ICON_COL_IDX) {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ICON_COL_IDX);
        CHECK_PTR_RET(field);
        // Open the output view only if the bitmap is valid
        if(field->Cast<wxCustomStatusBarBitmapField>()->GetBitmap().IsOk()) {
            m_mgr->ToggleOutputPane("Build");
        }
    } else if(fieldIndex == STATUSBAR_ANIMATION_COL_IDX) {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
        CHECK_PTR_RET(field);
        // Open the output view only if the bitmap is valid
        if(field->Cast<wxCustomStatusBarAnimationField>()->IsRunning()) {
            m_mgr->ToggleOutputPane("Build");
        }
    } else if(fieldIndex == STATUSBAR_ENCODING_COL_IDX) {
        // Show encoding menu
        wxMenu menu;
        wxFontEncoding fontEnc;
        OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
        size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
        std::map<int, wxString> encodingMenuItems;
        for(size_t i = 0; i < iEncCnt; i++) {
            fontEnc = wxFontMapper::GetEncoding(i);
            if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                continue;
            }

            wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
            wxMenuItem* itemEnc = menu.Append(wxID_ANY, encodingName, "", wxITEM_CHECK);
            itemEnc->Check(fontEnc == options->GetFileFontEncoding());
            encodingMenuItems.insert(std::make_pair(itemEnc->GetId(), encodingName));
        }
        int selectedId = GetPopupMenuSelectionFromUser(menu);
        if(encodingMenuItems.count(selectedId) == 0)
            return;

        // Change the encoding
        wxString selectedEncodingName = encodingMenuItems.find(selectedId)->second;
        ::clSetEditorFontEncoding(selectedEncodingName);
        // Update the status bar
        SetEncoding(selectedEncodingName);

    } else if(fieldIndex == STATUSBAR_LANG_COL_IDX) {
        if(m_mgr->GetActiveEditor()) {
            wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_LANG_COL_IDX);
            CHECK_PTR_RET(field);

            IEditor* editor = m_mgr->GetActiveEditor();
            LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetFileName().GetFullPath());
            if(lexer) {
                wxMenu menu;
                wxMenu* themesMenu = new wxMenu();
                wxMenu* langMenu = new wxMenu();

                // Create the themes menu
                std::map<int, wxString> themesMap;
                std::map<int, wxString> langsMap;
                wxArrayString themesArr = ColoursAndFontsManager::Get().GetAvailableThemesForLexer(lexer->GetName());
                for(size_t i = 0; i < themesArr.size(); ++i) {
                    wxMenuItem* itemTheme = themesMenu->Append(wxID_ANY, themesArr.Item(i), "", wxITEM_CHECK);
                    itemTheme->Check(themesArr.Item(i) == lexer->GetThemeName());
                    themesMap.insert(std::make_pair(itemTheme->GetId(), themesArr.Item(i)));
                }

                // Create the language menu
                wxArrayString langs = ColoursAndFontsManager::Get().GetAllLexersNames();
                for(size_t i = 0; i < langs.size(); ++i) {
                    wxMenuItem* itemLang = langMenu->Append(wxID_ANY, langs.Item(i), "", wxITEM_CHECK);
                    itemLang->Check(langs.Item(i) == lexer->GetName());
                    langsMap.insert(std::make_pair(itemLang->GetId(), langs.Item(i)));
                }
                menu.Append(wxID_ANY, _("Language"), langMenu);
                menu.Append(wxID_ANY, _("Colour Themes"), themesMenu);
                int selectedId = GetPopupMenuSelectionFromUser(menu);
                if(themesMap.count(selectedId)) {
                    // change the colour theme
                    wxBusyCursor bc;
                    ColoursAndFontsManager::Get().SetTheme(themesMap.find(selectedId)->second);
                    ColoursAndFontsManager::Get().Save();

                } else if(langsMap.count(selectedId)) {
                    // change the syntax highlight for the file
                    wxBusyCursor bc;
                    editor->SetSyntaxHighlight(langsMap.find(selectedId)->second);
                    SetLanguage(langsMap.find(selectedId)->second.Upper());
                }
            }
        }
    } else if(fieldIndex == STATUSBAR_WHITESPACE_INFO_IDX) {
        if(m_mgr->GetActiveEditor()) {
            // show a popup menu when clicking on the TABS/SPACES
            // field
            wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
            CHECK_PTR_RET(field);
            wxStyledTextCtrl* stc = m_mgr->GetActiveEditor()->GetCtrl();
            wxMenu menu;
            wxMenuItem* idConvertToTabs = menu.Append(wxID_ANY, _("Convert Indentations to Tabs"));
            wxMenuItem* idConvertToSpaces = menu.Append(wxID_ANY, _("Convert Indentations to Spaces"));
            menu.AppendSeparator();
            wxMenuItem* idUseTabs = menu.Append(wxID_ANY, _("Use Tabs"), "", wxITEM_CHECK);
            wxMenuItem* idUseSpaces = menu.Append(wxID_ANY, _("Use Spaces"), "", wxITEM_CHECK);

            // Check the proper tabs vs spaces option
            menu.Check(idUseSpaces->GetId(), !stc->GetUseTabs());
            menu.Check(idUseTabs->GetId(), stc->GetUseTabs());
            int selectedId = GetPopupMenuSelectionFromUser(menu);
            if(selectedId == wxID_NONE)
                return;

            if(selectedId == idConvertToTabs->GetId()) {
                wxCommandEvent evt(wxEVT_MENU, XRCID("convert_indent_to_tabs"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
            } else if(selectedId == idConvertToSpaces->GetId()) {
                wxCommandEvent evt(wxEVT_MENU, XRCID("convert_indent_to_spaces"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
            } else if(selectedId == idUseSpaces->GetId()) {
                stc->SetUseTabs(false);

                // Update the configuration
                OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
                options->SetIndentUsesTabs(false);
                EditorConfigST::Get()->SetOptions(options);

            } else if(selectedId == idUseTabs->GetId()) {
                stc->SetUseTabs(true);

                // Update the configuration
                OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
                options->SetIndentUsesTabs(true);
                EditorConfigST::Get()->SetOptions(options);
            }
            SetWhitespaceInfo();
        }
    } else if(fieldIndex == STATUSBAR_EOL_COL_IDX) {
        if(m_mgr->GetActiveEditor()) {
            // show a popup menu
            wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
            CHECK_PTR_RET(field);

            wxStyledTextCtrl* stc = m_mgr->GetActiveEditor()->GetCtrl();
            wxMenu menu;

            wxMenuItem* idDisplayEOL = menu.Append(wxID_ANY, _("Display EOL"), "", wxITEM_CHECK);
            menu.AppendSeparator();

            wxMenuItem* idUseLf = menu.Append(wxID_ANY, _("Use Linux Format (LF)"), "", wxITEM_CHECK);
            wxMenuItem* idUseCrLf = menu.Append(wxID_ANY, _("Use Windows Format (CRLF)"), "", wxITEM_CHECK);
            menu.AppendSeparator();

            wxMenuItem* idConvertToCrLF = menu.Append(wxID_ANY, _("Convert to Windows Format"));
            wxMenuItem* idConvertToLf = menu.Append(wxID_ANY, _("Convert to Linux Format"));

            // Check the proper tabs vs spaces option
            menu.Check(idUseLf->GetId(), stc->GetEOLMode() == wxSTC_EOL_LF);
            menu.Check(idUseCrLf->GetId(), stc->GetEOLMode() == wxSTC_EOL_CRLF);
            menu.Check(idDisplayEOL->GetId(), stc->GetViewEOL());

            int selectedId = GetPopupMenuSelectionFromUser(menu);
            if(selectedId == wxID_NONE)
                return;

            if(selectedId == idConvertToCrLF->GetId()) {
                // This will also change the EOL mode to CRLF
                wxCommandEvent evt(wxEVT_MENU, XRCID("convert_eol_win"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
            } else if(selectedId == idConvertToLf->GetId()) {
                // This will also change the EOL mode to LF
                wxCommandEvent evt(wxEVT_MENU, XRCID("convert_eol_unix"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
            } else if(selectedId == idUseLf->GetId()) {
                stc->SetEOLMode(wxSTC_EOL_LF);
            } else if(selectedId == idUseCrLf->GetId()) {
                stc->SetEOLMode(wxSTC_EOL_CRLF);
            } else if(selectedId == idDisplayEOL->GetId()) {
                wxCommandEvent evt(wxEVT_MENU, XRCID("display_eol"));
                evt.SetInt(stc->GetViewEOL() ? 0 : 1);
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
            }

            // Update the whitespace info
            CallAfter(&clStatusBar::SetWhitespaceInfo);
        }
    }
}

void clStatusBar::OnGotoAnythingShowing(clGotoEvent& e)
{
    e.Skip();
    const wxBitmap& defaultBitmap = clGetManager()->GetStdIcons()->LoadBitmap("colour-pallette");
    m_gotoAnythingTableSyntax.clear();
    m_gotoAnythingTableThemes.clear();
    // Get list of languages
    wxArrayString langs = ColoursAndFontsManager::Get().GetAllLexersNames();
    for(size_t i = 0; i < langs.size(); ++i) {
        wxString text;
        text << _("Set Syntax: ") << langs.Item(i);
        m_gotoAnythingTableSyntax[text] = langs.Item(i);
        clGotoEntry entry(text, "", wxID_ANY);
        e.GetEntries().push_back(entry);
    }

    wxArrayString themes = ColoursAndFontsManager::Get().GetAllThemes();
    for(size_t i = 0; i < themes.size(); ++i) {
        wxString text;
        text << _("Select Theme: ") << themes.Item(i);
        m_gotoAnythingTableThemes[text] = themes.Item(i);
        clGotoEntry entry(text, "", wxID_ANY);
        entry.SetBitmap(defaultBitmap);
        e.GetEntries().push_back(entry);
    }
}

void clStatusBar::OnActionSelected(clGotoEvent& e)
{
    e.Skip();
    const clGotoEntry& entry = e.GetEntry();
    const wxString& desc = entry.GetDesc();
    if(m_gotoAnythingTableSyntax.count(desc)) {
        IEditor* editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);
        wxBusyCursor bc;
        const wxString& lang = m_gotoAnythingTableSyntax[desc];

        editor->SetSyntaxHighlight(lang);
        SetLanguage(lang.Upper());
    } else if(m_gotoAnythingTableThemes.count(desc)) {
        ColoursAndFontsManager::Get().SetTheme(m_gotoAnythingTableThemes[desc]);
        ColoursAndFontsManager::Get().Save();
    }
}

int clStatusBar::GetTextWidth(const wxString& text) const
{
    const int SPACER = 10;
    wxClientDC dc(const_cast<clStatusBar*>(this));
    dc.SetFont(DrawingUtils::GetDefaultGuiFont());
    int textWidth = dc.GetTextExtent(text).GetWidth();
    textWidth += 2 * SPACER;
    return textWidth;
}

clToolBarGeneric* clStatusBar::CreatePaneButtonsToolbar()
{
    clToolBarGeneric* toolbar = new clToolBarGeneric(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER);
    toolbar->ShowOverflowButton(false);

    clBitmapList* images = toolbar->GetBitmapsCreateIfNeeded();
    toolbar->AddTool(XRCID("sidebar-button"), _("Show sidebar"), images->Add("sidebar"), wxEmptyString, wxITEM_CHECK);
    toolbar->AddTool(XRCID("bottombar-button"), _("Show output pane"), images->Add("bottombar"), wxEmptyString,
                     wxITEM_CHECK);
    toolbar->AddTool(XRCID("secondary-sidebar-button"), _("Show secondary sidebar"), images->Add("secondary-sidebar"),
                     wxEmptyString, wxITEM_CHECK);
    toolbar->Realize();

    toolbar->Bind(wxEVT_TOOL, &clStatusBar::OnSidebar, this, XRCID("sidebar-button"));
    toolbar->Bind(wxEVT_UPDATE_UI, &clStatusBar::OnSidebarUI, this, XRCID("sidebar-button"));
    toolbar->Bind(wxEVT_TOOL, &clStatusBar::OnBottombar, this, XRCID("bottombar-button"));
    toolbar->Bind(wxEVT_UPDATE_UI, &clStatusBar::OnBottombarUI, this, XRCID("bottombar-button"));
    toolbar->Bind(wxEVT_TOOL, &clStatusBar::OnSecondarySidebar, this, XRCID("secondary-sidebar-button"));
    toolbar->Bind(wxEVT_UPDATE_UI, &clStatusBar::OnSecondarySidebarUI, this, XRCID("secondary-sidebar-button"));
    return toolbar;
}

void clStatusBar::OnSecondarySidebarUI(wxUpdateUIEvent& event)
{
    wxAuiPaneInfo& info = clGetManager()->GetDockingManager()->GetPane(SECONDARY_SIDEBAR);
    if(info.IsOk()) {
        event.Check(info.IsShown());
    }
}

void clStatusBar::OnSecondarySidebar(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAuiPaneInfo& info = clGetManager()->GetDockingManager()->GetPane(SECONDARY_SIDEBAR);
    CHECK_COND_RET(info.IsOk());

    if(!info.IsShown()) {
        clGetManager()->ShowPane(SECONDARY_SIDEBAR, true);
    } else {
        clGetManager()->ShowPane(SECONDARY_SIDEBAR, false);
    }
}

void clStatusBar::OnSidebarUI(wxUpdateUIEvent& event)
{
    wxAuiPaneInfo& info = clGetManager()->GetDockingManager()->GetPane(wxT("Workspace View"));
    if(info.IsOk()) {
        event.Check(info.IsShown());
    }
}

void clStatusBar::OnSidebar(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clGetManager()->ToggleSidebarPane();
}

void clStatusBar::OnBottombarUI(wxUpdateUIEvent& event)
{
    wxAuiPaneInfo& info = clGetManager()->GetDockingManager()->GetPane(wxT("Output View"));
    if(info.IsOk()) {
        event.Check(info.IsShown());
    }
}

void clStatusBar::OnBottombar(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clGetManager()->ToggleOutputPane();
}
