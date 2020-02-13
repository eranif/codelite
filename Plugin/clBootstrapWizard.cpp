#include "ColoursAndFontsManager.h"
#include "CompilersDetectorManager.h"
#include "build_settings_config.h"
#include "clBootstrapWizard.h"
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

static std::vector<wxString> GetCxxPlugins()
{
    static std::vector<wxString> cxxPlugins;
    if(cxxPlugins.empty()) {
        cxxPlugins.push_back("CMakePlugin");
        cxxPlugins.push_back("CScope");
        cxxPlugins.push_back("CallGraph");
        cxxPlugins.push_back("ContinuousBuild");
        cxxPlugins.push_back("CppChecker");
        cxxPlugins.push_back("LLDBDebuggerPlugin");
#ifdef __WXGTK__
        cxxPlugins.push_back("MemCheck");
#endif
        cxxPlugins.push_back("QMakePlugin");
        cxxPlugins.push_back("UnitTestPP");
        cxxPlugins.push_back("Wizards");
        cxxPlugins.push_back("wxFormBuilder");
        cxxPlugins.push_back("wxcrafter");
        cxxPlugins.push_back("EOSWiki");
    }
    return cxxPlugins;
}

static std::vector<wxString> GetCommonPlugins()
{
    static std::vector<wxString> cxxPlugins;
    if(cxxPlugins.empty()) {
        cxxPlugins.push_back("AutoSave");
        cxxPlugins.push_back("Source Code Formatter");
        cxxPlugins.push_back("CodeLite Vim");
        cxxPlugins.push_back("Diff Plugin");
        cxxPlugins.push_back("LanguageServerPlugin");
        cxxPlugins.push_back("Outline");
        cxxPlugins.push_back("SFTP");
        cxxPlugins.push_back("Git");
        cxxPlugins.push_back("ExternalTools");
    }
    return cxxPlugins;
}

static std::vector<wxString> GetAllPlugins()
{
    static std::vector<wxString> allPlugins;
    static std::unordered_set<wxString> commonPlugins;
    if(commonPlugins.empty()) {
        GetCommonPlugins();
    }
    if(allPlugins.empty()) {

        clConfig conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        const PluginInfo::PluginMap_t& pluginsInfo = plugins.GetPlugins();
        std::for_each(pluginsInfo.begin(), pluginsInfo.end(), [&](const std::pair<wxString, PluginInfo>& item) {
            if(commonPlugins.count(item.second.GetName()) == 0) {
                allPlugins.push_back(item.first);
            }
        });
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

#define DARK_THEME "Retta light"
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

clBootstrapWizard::clBootstrapWizard(wxWindow* parent)
    : clBoostrapWizardBase(parent)
    , m_developmentProfile(0)
{
    m_selectedTheme = LIGHT_THEME;
    if(DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))) {
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

    m_stc24->SetText(sampleText);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", m_selectedTheme);
    if(lexer) {
        lexer->Apply(m_stc24, true);
    }
    m_stc24->SetKeyWords(1, "Demo std string");
    m_stc24->SetKeyWords(3, "other number");

    ::clRecalculateSTCHScrollBar(m_stc24);
    m_stc24->SetEditable(false);

    m_developmentProfile = clConfig::Get().Read("DevelopmentProfile", m_developmentProfile);
    m_radioBoxProfile->SetSelection(m_developmentProfile);

#if PHP_BUILD
    m_radioBoxProfile->SetSelection(3); // PHP
    m_radioBoxProfile->Enable(false);
#endif
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
    if(lexer) {
        lexer->Apply(m_stc24, true);
    }
}

void clBootstrapWizard::OnThemeSelected(wxCommandEvent& event)
{
    m_globalThemeChanged = true;
    m_stc24->SetEditable(true);
    int themeID = m_themePicker->GetSelection();
    switch(themeID) {
    case 0: // System Default
    {
        auto lexer = ColoursAndFontsManager::Get().GetLexer("c++", m_selectedTheme);
        m_selectedTheme = LIGHT_THEME;
        if(DrawingUtils::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))) {
            m_selectedTheme = DARK_THEME;
        }
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

    m_stc24->SetKeyWords(1, "Demo std string");
    m_stc24->SetKeyWords(3, "other");
    ::clRecalculateSTCHScrollBar(m_stc24);
    m_stc24->SetEditable(false);
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

bool clBootstrapWizard::GetUnSelectedPlugins(wxArrayString& plugins)
{
    int profile = m_radioBoxProfile->GetSelection();
    if(profile == 0) {
        // Default, dont change anything
        return false;
    } else if(profile == 1) {
        // Enable all
        plugins.Clear();
        return true;
    } else if(profile == 2) {
        // C/C++ developer
        std::vector<wxString> cxxPlugins = GetCxxPlugins();
        std::vector<wxString> allPlugins = GetAllPlugins();
        std::vector<wxString> commonPlugins = GetCommonPlugins();

        // Add the common plugins to the CXX ones
        cxxPlugins.insert(cxxPlugins.end(), commonPlugins.begin(), commonPlugins.end());

        std::vector<wxString> webPlugins;
        std::sort(cxxPlugins.begin(), cxxPlugins.end());
        std::sort(allPlugins.begin(), allPlugins.end());
        std::set_difference(allPlugins.begin(), allPlugins.end(), cxxPlugins.begin(), cxxPlugins.end(),
                            std::back_inserter(webPlugins));
        plugins.clear();
        for(const wxString& plugin : webPlugins) {
            plugins.Add(plugin);
        }
        return true;
    } else if(profile == 3) {
        // C/C++ developer for blockchain
        std::vector<wxString> cxxPlugins = GetCxxPlugins();
        std::vector<wxString> allPlugins = GetAllPlugins();
        std::vector<wxString> commonPlugins = GetCommonPlugins();

        // Add the common plugins to the CXX ones
        cxxPlugins.insert(cxxPlugins.end(), commonPlugins.begin(), commonPlugins.end());

        std::vector<wxString> webPlugins;
        std::sort(cxxPlugins.begin(), cxxPlugins.end());
        std::sort(allPlugins.begin(), allPlugins.end());
        std::set_difference(allPlugins.begin(), allPlugins.end(), cxxPlugins.begin(), cxxPlugins.end(),
                            std::back_inserter(webPlugins));
        plugins.Clear();
        std::for_each(webPlugins.begin(), webPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
        plugins.Add("wxcrafter");     // we don't want wxC enabled for this profile
        plugins.Add("wxFormBuilder"); // we don't want wxFB enabled for this profile
        return true;
    } else {
        // Web developer
        const std::vector<wxString>& cxxPlugins = GetCxxPlugins();
        std::for_each(cxxPlugins.begin(), cxxPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
        return true;
    }
}

bool clBootstrapWizard::IsRestartRequired()
{
    return (m_developmentProfile != m_radioBoxProfile->GetSelection()) || m_globalThemeChanged;
}

void clBootstrapWizard::OnFinish(wxWizardEvent& event)
{
    event.Skip();
    wxArrayString pluginsToDisable;
    if(IsRestartRequired() && GetUnSelectedPlugins(pluginsToDisable)) {
        // user changed plugins
        clConfig conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        plugins.DisablePugins(pluginsToDisable);
        conf.WriteItem(&plugins);
    }
}

void clBootstrapWizard::OnCancelWizard(wxCommandEvent& event)
{
    ::wxMessageBox(_("You can always run this setup wizard from the menu:\nHelp -> Run the Setup Wizard"), "CodeLite",
                   wxOK | wxCENTER | wxICON_INFORMATION, this);
    CallAfter(&clBootstrapWizard::EndModal, wxID_CANCEL);
}
