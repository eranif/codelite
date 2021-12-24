#include "clBootstrapWizard.h"

#include "ColoursAndFontsManager.h"
#include "CompilersDetectorManager.h"
#include "build_settings_config.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "globals.h"
#include "macros.h"
#include "plugindata.h"

#include <algorithm>
#include <vector>
#include <wx/dcmemory.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>
#include <wxStringHash.h>

static wxArrayString GetMiscPlugins()
{
    static wxArrayString miscPlugins;
    if(miscPlugins.empty()) {
        miscPlugins.push_back("AutoSave");
        miscPlugins.push_back("CodeLite Vim");
        miscPlugins.push_back("ExternalTools");
        miscPlugins.push_back("CMakePlugin");
        miscPlugins.push_back("CScope");
        miscPlugins.push_back("CppChecker");
        miscPlugins.push_back("QMakePlugin");
        miscPlugins.push_back("UnitTestPP");
        miscPlugins.push_back("EOSWiki");
#ifdef __WXGTK__
        miscPlugins.push_back("MemCheck");
#endif
    }
    return miscPlugins;
}

static const wxArrayString& GetBasePlugins()
{
    static wxArrayString basePlugins;
    if(basePlugins.empty()) {
        basePlugins.push_back("Source Code Formatter");
        basePlugins.push_back("EditorConfig");
        basePlugins.push_back("LanguageServerPlugin");
        basePlugins.push_back("Outline");
        basePlugins.push_back("SFTP");
        basePlugins.push_back("Git");
        basePlugins.push_back("Word Completion");
        basePlugins.push_back("Diff Plugin");
        basePlugins.push_back("SmartCompletion");
    }
    return basePlugins;
}

static const wxArrayString& GetCxxPlugins()
{
    static wxArrayString cxxPlugins;
    if(cxxPlugins.empty()) {
        cxxPlugins.insert(cxxPlugins.end(), GetBasePlugins().begin(), GetBasePlugins().end());
        cxxPlugins.push_back("LLDBDebuggerPlugin");
        cxxPlugins.push_back("Wizards");
        cxxPlugins.push_back("wxcrafter");
    }
    return cxxPlugins;
}

static const wxArrayString& GetEOSWikiPlugins()
{
    static wxArrayString eosPlugins;
    if(eosPlugins.empty()) {
        eosPlugins.insert(eosPlugins.end(), GetBasePlugins().begin(), GetBasePlugins().end());
        eosPlugins.push_back("LLDBDebuggerPlugin");
        eosPlugins.push_back("Wizards");
        eosPlugins.push_back("EOSWiki");
    }
    return eosPlugins;
}

static const wxArrayString& GetWebPlugins()
{
    static wxArrayString webPlugins;
    if(webPlugins.empty()) {
        webPlugins.insert(webPlugins.end(), GetBasePlugins().begin(), GetBasePlugins().end());
        webPlugins.push_back("WebTools");
        webPlugins.push_back("PHP");
        webPlugins.push_back("PHPLint");
        webPlugins.push_back("PHPRefactoring");
    }
    return webPlugins;
}

static const wxArrayString& GetAllPlugins()
{
    static wxArrayString allPlugins;
    if(allPlugins.empty()) {
        WX_APPEND_ARRAY(allPlugins, GetBasePlugins());
        WX_APPEND_ARRAY(allPlugins, GetCxxPlugins());
        WX_APPEND_ARRAY(allPlugins, GetWebPlugins());
        WX_APPEND_ARRAY(allPlugins, GetMiscPlugins());
        allPlugins.Sort();
    }
    return allPlugins;
}

class clBootstrapWizardPluginData : public wxClientData
{
public:
    bool checked;
    PluginInfo pluginInfo;

public:
    clBootstrapWizardPluginData(const PluginInfo& pi)
        : checked(true)
        , pluginInfo(pi)
    {
    }
    virtual ~clBootstrapWizardPluginData() {}

    wxString GetPluginSummary() const
    {
        wxString summary;
        summary << pluginInfo.GetName() << " " << pluginInfo.GetVersion() << "\n"
                << _("By: ") << pluginInfo.GetAuthor() << "\n\n"
                << pluginInfo.GetDescription();
        return summary;
    }
};

#define DARK_THEME "Atom One-Dark"
#define NO_SO_LIGHT_THEME "Roboticket"
#define LIGHT_THEME "Atom One Light"

const wxString sampleText = "class Demo {\n"
                            "private:\n"
                            "    std::string m_str;\n"
                            "    int m_integer;\n"
                            "    \n"
                            "public:\n"
                            "    /**\n"
                            "     * Creates a new demo.\n"
                            "     * @param o The object\n"
                            "     */\n"
                            "    Demo(const Demo &other) {\n"
                            "        // Initialise with Zero\n"
                            "        int number = 0;\n"
                            "        m_str = other.m_str;\n"
                            "        m_integer = other.m_integer;\n"
                            "    }\n"
                            "};";

clBootstrapWizard::clBootstrapWizard(wxWindow* parent, bool firstTime)
    : clBoostrapWizardBase(parent)
    , m_firstTime(firstTime)
    , m_developmentProfile(0)
{
    m_selectedTheme = LIGHT_THEME;
    if(DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW))) {
        m_selectedTheme = DARK_THEME;
    }

    m_themePicker->Clear();
    wxArrayString options;
    options.Add(_("System Default"));
    options.Add(_("Dark"));
    options.Add(_("Grey"));
    options.Add(_("Light"));
    m_themePicker->Append(options);
    m_themePicker->SetSelection(0);

    SetSelectedTheme(m_selectedTheme);

    m_developmentProfile = clConfig::Get().Read("DevelopmentProfile", m_developmentProfile);
    m_radioBoxProfile->SetSelection(m_developmentProfile);
}

clBootstrapWizard::~clBootstrapWizard() { clConfig::Get().Write("DevelopmentProfile", m_developmentProfile); }

void clBootstrapWizard::SetSelectedTheme(const wxString& themeName)
{
    m_selectedTheme = themeName;
    auto lexer = ColoursAndFontsManager::Get().GetLexer("c++", m_selectedTheme);
    wxColour bgColour = ColoursAndFontsManager::Get().GetBackgroundColourFromLexer(lexer);
    if(bgColour.IsOk()) {
        clConfig::Get().Write("UseCustomBaseColour", true);
        clConfig::Get().Write("BaseColour", bgColour);
    } else {
        clConfig::Get().Write("UseCustomBaseColour", false);
    }
    DoUpdatePreview(themeName);
}

void clBootstrapWizard::OnThemeSelected(wxCommandEvent& event)
{
    m_globalThemeChanged = true;
    int themeID = m_themePicker->GetSelection();
    switch(themeID) {
    case 0: { // System Default
        auto lexer = ColoursAndFontsManager::Get().GetLexer("c++", m_selectedTheme);
        m_selectedTheme = LIGHT_THEME;
        if(DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW))) {
            m_selectedTheme = DARK_THEME;
        }
        SetSelectedTheme(m_selectedTheme);
        clConfig::Get().Write("UseCustomBaseColour", false);
        if(lexer) {
            lexer->Apply(m_stc24, true);
        }
    }; break;
    case 1: // Dark
        SetSelectedTheme(DARK_THEME);
        break;
    case 2: // Grey
        SetSelectedTheme(NO_SO_LIGHT_THEME);
        break;
    default:
    case 3: // Light
        SetSelectedTheme(LIGHT_THEME);
        break;
    }
}

void clBootstrapWizard::OnScanForCompilers(wxCommandEvent& event)
{
    wxBusyCursor bc;
    m_compilers.clear();

#ifndef __WXGTK__
    wxWindowUpdateLocker locker(m_wizardPageCompilers);
#endif

    CompilersDetectorManager detector;
    if(detector.Locate()) {
        m_cmdLnkBtnScanForCompilers->Hide();
        m_cmdLnkBtnDownloadCompiler->Hide();
        m_dvListCtrlCompilers->Show();

        m_compilers = detector.GetCompilersFound();
        for(size_t i = 0; i < m_compilers.size(); ++i) {
            wxVector<wxVariant> cols;
            cols.push_back(m_compilers.at(i)->GetName());
            cols.push_back(m_compilers.at(i)->GetInstallationPath());
            m_dvListCtrlCompilers->AppendItem(cols);
        }

        if(!detector.FoundMinGWCompiler()) {
            CompilersDetectorManager::MSWSuggestToDownloadMinGW(true);
        }

    } else {
        // nothing found on this machine, offer to download
        CompilersDetectorManager::MSWSuggestToDownloadMinGW(true);
    }
    m_wizardPageCompilers->GetSizer()->Layout();
}

clBootstrapData clBootstrapWizard::GetData()
{
    clBootstrapData data;
    data.compilers = m_compilers;
    data.selectedTheme = m_selectedTheme;
    data.useTabs = (m_radioBoxSpacesVsTabs->GetSelection() == 1);
    data.whitespaceVisibility = m_radioBoxWhitespaceVisibility->GetSelection();
    return data;
}

void clBootstrapWizard::OnInstallCompiler(wxCommandEvent& event)
{
    CompilersDetectorManager::MSWSuggestToDownloadMinGW(false);
}

void clBootstrapWizard::OnInstallCompilerUI(wxUpdateUIEvent& event)
{
#ifdef __WXMSW__
    event.Enable(true);
#else
    event.Enable(false);
#endif
}

wxArrayString clBootstrapWizard::GetSelectedPlugins()
{
    int profile = m_radioBoxProfile->GetSelection();
    if(profile == 0) {
        // Default, so load the lot
        return GetAllPlugins();
    } else if(profile == 1) {
        // C++
        return GetCxxPlugins();
    } else if(profile == 2) {
        // web developer
        return GetWebPlugins();
    } else {
        return GetEOSWikiPlugins();
    }
}

bool clBootstrapWizard::IsRestartRequired()
{
    return m_firstTime || (m_developmentProfile != m_radioBoxProfile->GetSelection()) || m_globalThemeChanged;
}

void clBootstrapWizard::OnFinish(wxWizardEvent& event)
{
    event.Skip();
    if(IsRestartRequired() || m_firstTime) {
        // user changed plugins
        clConfig conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);
        plugins.EnablePlugins(GetSelectedPlugins());
        conf.WriteItem(&plugins);
    }
}

void clBootstrapWizard::OnCancelWizard(wxCommandEvent& event)
{
    ::wxMessageBox(_("You can always run this setup wizard from the menu:\nHelp -> Run the Setup Wizard"), "CodeLite",
                   wxOK | wxCENTER | wxICON_INFORMATION, this);
    CallAfter(&clBootstrapWizard::EndModal, wxID_CANCEL);
}

void clBootstrapWizard::DoUpdatePreview(const wxString& themeName)
{
    // Populate the preview
    LexerConf::Ptr_t previewLexer = ColoursAndFontsManager::Get().GetLexer("c++", themeName);

    int class_index = 1;
    int locals_index = 1;
    int methods_index = 1;
    int other_index = 1;

    if(previewLexer) {
        previewLexer->Apply(m_stc24, true);
        class_index = previewLexer->GetWordSetClassIndex();
        locals_index = previewLexer->GetWordSetLocalsIndex();
        methods_index = previewLexer->GetWordSetFunctionsIndex();
        other_index = previewLexer->GetWordSetOthersIndex();
    }

    m_stc24->SetKeyWords(class_index, "Demo std string");
    m_stc24->SetKeyWords(locals_index, "other");
    m_stc24->SetKeyWords(methods_index, "CallMethod");
    m_stc24->SetKeyWords(other_index, wxEmptyString);

    m_stc24->SetEditable(true);
    m_stc24->SetText(sampleText);
    m_stc24->HideSelection(true);
    m_stc24->SetEditable(false);
    ::clRecalculateSTCHScrollBar(m_stc24);
}
