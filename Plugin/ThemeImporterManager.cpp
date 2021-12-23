#include "ColoursAndFontsManager.h"
#include "ThemeImporterASM.hpp"
#include "ThemeImporterBatch.hpp"
#include "ThemeImporterCMake.hpp"
#include "ThemeImporterCSS.hpp"
#include "ThemeImporterCXX.hpp"
#include "ThemeImporterCobra.hpp"
#include "ThemeImporterCobraAlt.hpp"
#include "ThemeImporterDiff.hpp"
#include "ThemeImporterDockerfile.hpp"
#include "ThemeImporterFortran.hpp"
#include "ThemeImporterINI.hpp"
#include "ThemeImporterInnoSetup.hpp"
#include "ThemeImporterJava.hpp"
#include "ThemeImporterJavaScript.hpp"
#include "ThemeImporterLUA.hpp"
#include "ThemeImporterMakefile.hpp"
#include "ThemeImporterPHP.hpp"
#include "ThemeImporterPython.hpp"
#include "wx/versioninfo.h"
#if wxCHECK_VERSION(3, 1, 0)
#include "ThemeImporterJSON.hpp"
#include "ThemeImporterRust.hpp"
#endif
#include "ThemeImporterManager.hpp"
#include "ThemeImporterRuby.hpp"
#include "ThemeImporterSCSS.hpp"
#include "ThemeImporterSQL.hpp"
#include "ThemeImporterScript.hpp"
#include "ThemeImporterText.hpp"
#include "ThemeImporterXML.hpp"
#include "ThemeImporterYAML.hpp"

EclipseThemeImporterManager::EclipseThemeImporterManager()
{
    m_importers.push_back(new ThemeImporterCXX());
    m_importers.push_back(new ThemeImporterCMake());
    m_importers.push_back(new EclipseTextThemeImporter());
    m_importers.push_back(new EclipseMakefileThemeImporter());
    m_importers.push_back(new ThemeImporterDiff());
    m_importers.push_back(new EclipsePHPThemeImporter());
    m_importers.push_back(new ThemeImporterCSS());
    m_importers.push_back(new ThemeImporterXML());
    m_importers.push_back(new EclipseJavaScriptThemeImporter());
    m_importers.push_back(new EclipseINIThemeImporter());
    m_importers.push_back(new ThemeImporterASM());
    m_importers.push_back(new ThemeImporterBatch());
    m_importers.push_back(new EclipsePythonThemeImporter());
    m_importers.push_back(new ThemeImporterCobra());
    m_importers.push_back(new ThemeImporterCobraAlt());
    m_importers.push_back(new EclipseFortranThemeImporter());
    m_importers.push_back(new EclipseInnoSetupThemeImporter());
    m_importers.push_back(new EclipseJavaThemeImporter());
    m_importers.push_back(new EclipseLuaThemeImporter());
    m_importers.push_back(new EclipseScriptThemeImporter());
    m_importers.push_back(new EclipseSQLThemeImporter());
    m_importers.push_back(new EclipseSCSSThemeImporter());
    m_importers.push_back(new ThemeImporterDockerfile());
    m_importers.push_back(new EclipseYAMLThemeImporter());
    m_importers.push_back(new EclipseRubyThemeImporter());
#if wxCHECK_VERSION(3, 1, 0)
    m_importers.push_back(new EclipseRustThemeImporter());
    m_importers.push_back(new EclipseJsonThemeImporter());
#endif
}

EclipseThemeImporterManager::~EclipseThemeImporterManager() {}

wxString EclipseThemeImporterManager::Import(const wxString& theme_file)
{
    wxString name;
    EclipseThemeImporterBase::List_t::iterator iter = m_importers.begin();
    for(; iter != m_importers.end(); ++iter) {
        auto lexer = (*iter)->Import(theme_file);
        if(name.empty()) {
            name = lexer->GetThemeName();
        }
        ColoursAndFontsManager::Get().AddLexer(lexer);
    }
    return name;
}
