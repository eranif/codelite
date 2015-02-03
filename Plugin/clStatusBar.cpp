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

#define STATUSBAR_MESSAGES_COL_IDX 0
#define STATUSBAR_ANIMATION_COL_IDX 1
#define STATUSBAR_LINECOL_COL_IDX 2
#define STATUSBAR_LANG_COL_IDX 3
#define STATUSBAR_ICON_COL_IDX 4

class WXDLLIMPEXP_SDK clStatusBarArtNormal : public wxCustomStatusBarArt
{
public:
    clStatusBarArtNormal()
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
    
    wxCustomStatusBarField::Ptr_t messages(new wxCustomStatusBarFieldText(300));
    AddField(messages);

    wxCustomStatusBarField::Ptr_t buildAnimation(new wxCustomStatusBarAnimationField(
        this, wxXmlResource::Get()->LoadBitmap("build-animation-sprite"), wxHORIZONTAL, wxSize(80, 7)));
    AddField(buildAnimation);
    
    wxCustomStatusBarField::Ptr_t lineCol(new wxCustomStatusBarFieldText(150));
    AddField(lineCol);

    wxCustomStatusBarField::Ptr_t language(new wxCustomStatusBarFieldText(80));
    AddField(language);

    wxCustomStatusBarField::Ptr_t build(new wxCustomStatusBarBitmapField(30));
    AddField(build);

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

void clStatusBar::SetMessage(const wxString& message)
{
    // Col 0
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_MESSAGES_COL_IDX);
    CHECK_PTR_RET(field);
    field->Cast<wxCustomStatusBarFieldText>()->SetText(message);
    field->SetTooltip(message);
    Refresh();
}

void clStatusBar::OnPageChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateColour();

    // Update the file name
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor) {
        SetFileName(editor->GetFileName().GetFullPath());
    } else {
        SetFileName("");
    }

    // update the language
    wxString language;
    if(editor) {
        int lexerId = editor->GetSTC()->GetLexer();
        switch(lexerId) {
        case wxSTC_LEX_CPP: {
            // C++ is used for 3 languages:
            // C++, JS and Jave
            if(FileExtManager::IsCxxFile(editor->GetFileName()))
                language = "C++";
            else if(FileExtManager::IsJavaFile(editor->GetFileName()))
                language = "Java";
            else if(FileExtManager::IsJavascriptFile(editor->GetFileName()))
                language = "Javascript";
            else
                language = "C++";
            break;
        }
        case wxSTC_LEX_NULL:
            language = "text";
            break;
        case wxSTC_LEX_ASM:
            language = "asm";
            break;
        case wxSTC_LEX_BATCH:
            language = "BATCH";
            break;
        case wxSTC_LEX_PROPERTIES:
            language = "INI";
            break;
        case wxSTC_LEX_XML:
            language = "XML";
            break;
        case wxSTC_LEX_HTML: {
            // wxSTC_LEX_HTML is used for 2 languages: HTML and PHP
            if(FileExtManager::IsPHPFile(editor->GetFileName()))
                language = "PHP";
            else
                language = "hypertext";
            break;
        }
        case wxSTC_LEX_CSS:
            language = "css";
            break;
        case wxSTC_LEX_CMAKE:
            language = "cmake";
            break;
        case wxSTC_LEX_PERL:
            language = "perl";
            break;
        case wxSTC_LEX_PYTHON:
            language = "python";
            break;
        case wxSTC_LEX_LUA:
            language = "lua";
            break;
        case wxSTC_LEX_INNOSETUP:
            language = "innosetup";
            break;
        case wxSTC_LEX_MAKEFILE:
            language = "makefile";
            break;
        case wxSTC_LEX_BASH:
            language = "bash";
            break;
        case wxSTC_LEX_FORTRAN:
            language = "fortran";
            break;
        case wxSTC_LEX_SQL:
            language = "sql";
            break;
        default:
            language = "";
            break;
        }
        language.MakeUpper();
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

void clStatusBar::SetFileName(const wxString& filename) { SetText(filename); }

void clStatusBar::SetLanguage(const wxString& lang)
{
    // Col 2
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_LANG_COL_IDX);
    CHECK_PTR_RET(field);

    wxString ucLang = lang.Upper();
    field->Cast<wxCustomStatusBarFieldText>()->SetText(ucLang);
    field->SetTooltip(lang);
    Refresh();
}

void clStatusBar::SetLinePosColumn(const wxString& lineCol) { CallAfter(&clStatusBar::DoSetLinePosColumn, lineCol); }

void clStatusBar::DoSetLinePosColumn(const wxString& message)
{
    // Col 1
    wxCustomStatusBarField::Ptr_t field = GetField(STATUSBAR_LINECOL_COL_IDX);
    CHECK_PTR_RET(field);
    field->Cast<wxCustomStatusBarFieldText>()->SetText(message);
    field->SetTooltip(message);
    Refresh();
}

void clStatusBar::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

void clStatusBar::Clear()
{
    SetFileName("");
    SetLinePosColumn("");
    SetMessage("");
    SetText("");
    SetBuildBitmap(wxNullBitmap, "");
    StopAnimation();
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
    }
}
