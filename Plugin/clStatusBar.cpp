#include "clStatusBar.h"
#include "macros.h"
#include "event_notifier.h"
#include "ieditor.h"
#include "imanager.h"
#include "drawingutils.h"
#include <wx/settings.h>
#include "codelite_events.h"
#include <wx/stc/stc.h>
#include "fileextmanager.h"
#include <wx/xrc/xmlres.h>
#include "editor_config.h"
#include "ColoursAndFontsManager.h"
#include <algorithm>
#include "globals.h"
#include <wx/menu.h>

#define STATUSBAR_LINE_COL_IDX 0
#define STATUSBAR_ANIMATION_COL_IDX 1
#define STATUSBAR_WHITESPACE_INFO_IDX 2
#define STATUSBAR_LANG_COL_IDX 3
#define STATUSBAR_ICON_COL_IDX 4

class WXDLLIMPEXP_SDK clStatusBarArtNormal : public wxCustomStatusBarArt
{
public:
    clStatusBarArtNormal()
        : wxCustomStatusBarArt("Light")
    {
        m_textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_textShadowColour = DrawingUtils::GetPanelBgColour();
        m_penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        m_bgColour = DrawingUtils::GetPanelBgColour();
        m_separatorColour = *wxWHITE;
    }
    virtual ~clStatusBarArtNormal() {}
};

clStatusBar::clStatusBar(wxWindow* parent, IManager* mgr)
    : wxCustomStatusBar(parent)
    , m_mgr(mgr)
{
    SetArt(wxCustomStatusBarArt::Ptr_t(new clStatusBarArtNormal));

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clStatusBar::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clStatusBar::OnPageChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clStatusBar::OnAllEditorsClosed, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_STARTED, &clStatusBar::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &clStatusBar::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clStatusBar::OnWorkspaceClosed, this);
    Bind(wxEVT_STATUSBAR_CLICKED, &clStatusBar::OnFieldClicked, this);

    wxCustomStatusBarField::Ptr_t lineCol(new wxCustomStatusBarFieldText(this, 250));
    AddField(lineCol);

    wxCustomStatusBarField::Ptr_t buildAnimation(new wxCustomStatusBarAnimationField(
        this, wxXmlResource::Get()->LoadBitmap("build-animation-sprite"), wxHORIZONTAL, wxSize(80, 7)));
    AddField(buildAnimation);

    wxCustomStatusBarField::Ptr_t whitespace(new wxCustomStatusBarFieldText(this, 80));
    AddField(whitespace);

    wxCustomStatusBarField::Ptr_t language(new wxCustomStatusBarFieldText(this, 100));
    AddField(language);

    wxCustomStatusBarField::Ptr_t buildStatus(new wxCustomStatusBarBitmapField(this, 30));
    AddField(buildStatus);

    m_bmpBuildError = wxXmlResource::Get()->LoadBitmap("build-error");
    m_bmpBuildWarnings = wxXmlResource::Get()->LoadBitmap("build-warning");
    m_bmpBuild = wxXmlResource::Get()->LoadBitmap("build-building");
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
    Unbind(wxEVT_STATUSBAR_CLICKED, &clStatusBar::OnFieldClicked, this);
}

void clStatusBar::SetMessage(const wxString& message, int secondsToLive) { SetText(message, secondsToLive); }

void clStatusBar::OnPageChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateColour();

    // Update the file name
    IEditor* editor = m_mgr->GetActiveEditor();
    // update the language
    wxString language = "TEXT";
    if(editor) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetFileName().GetFullPath());
        if(lexer) {
            language = lexer->GetName().Upper();
        }
        // Set the "TABS/SPACES" field
        SetWhitespaceInfo(editor->GetCtrl()->GetUseTabs() ? "tabs" : "spaces");
    }
    SetLanguage(language);
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
            art.reset(new wxCustomStatusBarArt("Dark"));
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

    field->Cast<wxCustomStatusBarFieldText>()->SetText(message);
    field->SetTooltip(message);
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
    SetWhitespaceInfo("");
}

void clStatusBar::OnBuildEnded(clBuildEvent& event)
{
    event.Skip();
    StopAnimation();
    if(event.GetErrorCount()) {
        SetBuildBitmap(m_bmpBuildError, _("Build ended with errors. Click to view"));
    } else if(event.GetWarningCount()) {
        SetBuildBitmap(m_bmpBuildWarnings, _("Build ended with warnings. Click to view"));
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

void clStatusBar::OnWorkspaceClosed(wxCommandEvent& event)
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

void clStatusBar::OnFieldClicked(clCommandEvent& event)
{
    if(event.GetInt() == STATUSBAR_ICON_COL_IDX) {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ICON_COL_IDX);
        CHECK_PTR_RET(field);
        // Open the output view only if the bitmap is valid
        if(field->Cast<wxCustomStatusBarBitmapField>()->GetBitmap().IsOk()) {
            m_mgr->ToggleOutputPane("Build");
        }
    } else if(event.GetInt() == STATUSBAR_ANIMATION_COL_IDX) {
        wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
        CHECK_PTR_RET(field);
        // Open the output view only if the bitmap is valid
        if(field->Cast<wxCustomStatusBarAnimationField>()->IsRunning()) {
            m_mgr->ToggleOutputPane("Build");
        }
    } else if(event.GetInt() == STATUSBAR_LANG_COL_IDX) {
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
                    ColoursAndFontsManager::Get().SetActiveTheme(lexer->GetName(), themesMap.find(selectedId)->second);
                    ColoursAndFontsManager::Get().Save();

                    // Update the colours
                    IEditor::List_t editors;
                    clGetManager()->GetAllEditors(editors);
                    std::for_each(editors.begin(), editors.end(), [&](IEditor* e) {
                        // get the lexer associated with this editor 
                        // and re-apply the syntax highlight
                        LexerConf::Ptr_t editorLexer =
                            ColoursAndFontsManager::Get().GetLexerForFile(e->GetFileName().GetFullPath());
                        if(editorLexer) {
                            e->SetSyntaxHighlight(editorLexer->GetName());
                        }
                    });

                    // We need to force an update to ensure that the colours also affects
                    // the tab drawing area
                    clGetManager()->GetDockingManager()->Update();

                } else if(langsMap.count(selectedId)) {
                    // change the syntax highlight for the file
                    wxBusyCursor bc;
                    editor->SetSyntaxHighlight(langsMap.find(selectedId)->second);
                    SetLanguage(langsMap.find(selectedId)->second.Upper());
                }
            }
        }
    } else if(event.GetInt() == STATUSBAR_WHITESPACE_INFO_IDX) {
        if(m_mgr->GetActiveEditor()) {
            // show a popup menu when clicking on the TABS/SPACES
            // field
            wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
            CHECK_PTR_RET(field);
            wxStyledTextCtrl* stc = m_mgr->GetActiveEditor()->GetCtrl();
            wxMenu menu;
            wxMenuItem* idConvertToTabs = menu.Append(wxID_ANY, "Convert Indentations to Tabs");
            wxMenuItem* idConvertToSpaces = menu.Append(wxID_ANY, "Convert Indentations to Spaces");
            menu.AppendSeparator();
            wxMenuItem* idUseTabs = menu.Append(wxID_ANY, "Use Tabs", "", wxITEM_CHECK);
            wxMenuItem* idUseSpaces = menu.Append(wxID_ANY, "Use Spaces", "", wxITEM_CHECK);

            // Check the proper tabs vs spaces option
            menu.Check(idUseSpaces->GetId(), !stc->GetUseTabs());
            menu.Check(idUseTabs->GetId(), stc->GetUseTabs());

            // wxPoint pt = field->GetRect().GetTopLeft();
            // int menuHeight = 20;
            // menuHeight *= 5;
            //
            // pt.y -= menuHeight;

            int selectedId = GetPopupMenuSelectionFromUser(menu);
            if(selectedId == wxID_NONE) return;

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
            SetWhitespaceInfo(stc->GetUseTabs() ? "tabs" : "spaces");
        }
    }
}

void clStatusBar::SetWhitespaceInfo(const wxString& whitespaceInfo)
{
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
    CHECK_PTR_RET(field);

    wxString ws = whitespaceInfo.Upper();
    field->Cast<wxCustomStatusBarFieldText>()->SetText(ws);
    field->SetTooltip(ws);
}
