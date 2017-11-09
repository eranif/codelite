#include "clBootstrapWizard.h"
#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "CompilersDetectorManager.h"
#include "build_settings_config.h"
#include <wx/wupdlock.h>
#include <wx/dcmemory.h>
#include "plugindata.h"
#include <algorithm>
#include <vector>
#include "macros.h"
#include <wx/msgdlg.h>
#include "file_logger.h"

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
    }
    return cxxPlugins;
}

static std::vector<wxString> GetAllPlugins()
{
    static std::vector<wxString> allPlugins;
    if(allPlugins.empty()) {

        clConfig conf("plugins.conf");
        PluginInfoArray plugins;
        conf.ReadItem(&plugins);

        const PluginInfo::PluginMap_t& pluginsInfo = plugins.GetPlugins();
        std::for_each(pluginsInfo.begin(), pluginsInfo.end(),
                      [&](const std::pair<wxString, PluginInfo>& item) { allPlugins.push_back(item.first); });
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
        summary << pluginInfo.GetName() << " " << pluginInfo.GetVersion() << "\n" << _("By: ") << pluginInfo.GetAuthor()
                << "\n\n" << pluginInfo.GetDescription();
        return summary;
    }
};

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
    wxArrayString themes = ColoursAndFontsManager::Get().GetAllThemes();
    m_choiceTheme->Append(themes);
    m_choiceTheme->SetSelection(m_choiceTheme->FindString("One Dark Like"));
    m_stc24->SetText(sampleText);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", "One Dark Like");
    if(lexer) {
        lexer->Apply(m_stc24, true);
    }
    m_stc24->SetKeyWords(1, "Demo std string");
    m_stc24->SetKeyWords(3, "other number");

    ::clRecalculateSTCHScrollBar(m_stc24);
    m_stc24->SetEditable(false);

    m_developmentProfile = clConfig::Get().Read("DevelopmentProfile", m_developmentProfile);
    m_radioBoxProfile->SetSelection(m_developmentProfile);
}

clBootstrapWizard::~clBootstrapWizard() { clConfig::Get().Write("DevelopmentProfile", m_developmentProfile); }

void clBootstrapWizard::OnThemeSelected(wxCommandEvent& event)
{
    m_stc24->SetEditable(true);
    wxString selection = m_choiceTheme->GetStringSelection();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++", selection);
    if(lexer) {
        lexer->Apply(m_stc24, true);
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

wxBitmap clBootstrapWizard::GenerateBitmap(size_t labelIndex)
{
    wxArrayString labels;
    labels.Add("Welcome");
    labels.Add("Plugins");
    labels.Add("Compilers");
    labels.Add("Colours");
    labels.Add("Whitespace");

    wxBitmap bmp(150, 500);
    wxMemoryDC memDC(bmp);
    memDC.SetPen(wxColour("rgb(64, 64, 64)"));
    memDC.SetBrush(wxColour("rgb(64, 64, 64)"));
    memDC.DrawRectangle(wxRect(bmp.GetSize()));
    memDC.SetPen(*wxBLACK_PEN);
    memDC.DrawLine(149, 0, 149, 500);

    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont boldFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    boldFont.SetWeight(wxFONTWEIGHT_BOLD);

    wxBitmap arrowRight = wxXmlResource::Get()->LoadBitmap("arrow-right-24");
    for(size_t i = 0; i < labels.size(); ++i) {
        wxSize textSize = memDC.GetTextExtent("Tp");
        int rectHeight = textSize.y + 20; // 10 pixels margin x 2
        wxRect rect(0, i * rectHeight, bmp.GetWidth(), rectHeight);

        // Draw the text (align to the right)
        wxCoord textX, textY, bmpX, bmpY;
        memDC.SetFont(font);
        memDC.SetTextForeground(i == labelIndex ? *wxWHITE : wxColour("rgb(200, 200, 200)"));
        memDC.SetFont(i == labelIndex ? boldFont : font);
        textSize = memDC.GetTextExtent(labels.Item(i));
        textX = /*bmp.GetWidth() - textSize.GetWidth() - 16*/ 16;
        textY = ((rect.GetHeight() - textSize.GetHeight()) / 2) + rect.GetY();
        memDC.DrawText(labels.Item(i), textX, textY);

        if(i == labelIndex) {
            bmpX = rect.GetWidth() - arrowRight.GetWidth();
            bmpY = ((rect.GetHeight() - arrowRight.GetHeight()) / 2) + rect.GetY();
            memDC.DrawBitmap(arrowRight, bmpX, bmpY);
        }
    }
    memDC.SelectObject(wxNullBitmap);
    return bmp;
}

clBootstrapData clBootstrapWizard::GetData()
{
    clBootstrapData data;
    data.compilers = m_compilers;
    data.selectedTheme = m_choiceTheme->GetStringSelection();
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
    if(m_radioBoxProfile->GetSelection() == 0) {
        // Default, dont change anything
        return false;
    } else if(m_radioBoxProfile->GetSelection() == 1) {
        // Enable all
        plugins.Clear();
        return true;
    } else if(m_radioBoxProfile->GetSelection() == 2) {
        // C/C++ developer
        std::vector<wxString> cxxPlugins = GetCxxPlugins();
        std::vector<wxString> allPlugins = GetAllPlugins();
        std::vector<wxString> webPlugins;
        std::sort(cxxPlugins.begin(), cxxPlugins.end());
        std::sort(allPlugins.begin(), allPlugins.end());
        std::set_difference(allPlugins.begin(), allPlugins.end(), cxxPlugins.begin(), cxxPlugins.end(),
                            std::back_inserter(webPlugins));
        plugins.Clear();
        std::for_each(webPlugins.begin(), webPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
        return true;
    } else {
        // Web developer
        const std::vector<wxString>& cxxPlugins = GetCxxPlugins();
        std::for_each(cxxPlugins.begin(), cxxPlugins.end(), [&](const wxString& plugin) { plugins.push_back(plugin); });
        return true;
    }
}

bool clBootstrapWizard::IsRestartRequired() { return m_developmentProfile != m_radioBoxProfile->GetSelection(); }

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
