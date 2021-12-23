#include "ColoursAndFontsManager.h"
#include "EclipseASMThemeImporter.h"
#include "EclipseBatchThemeImporter.h"
#include "EclipseCMakeThemeImporter.h"
#include "EclipseCSSThemeImporter.h"
#include "EclipseCXXThemeImporter.h"
#include "EclipseCobraAltThemeImporter.h"
#include "EclipseCobraThemeImporter.h"
#include "EclipseDiffThemeImporter.h"
#include "EclipseDockerfileThemeImporter.h"
#include "EclipseFortranThemeImporter.h"
#include "EclipseINIThemeImporter.h"
#include "EclipseInnoSetupThemeImporter.h"
#include "EclipseJavaScriptThemeImporter.h"
#include "EclipseJavaThemeImporter.h"
#include "EclipseLuaThemeImporter.h"
#include "EclipseMakefileThemeImporter.h"
#include "EclipsePHPThemeImporter.h"
#include "EclipsePythonThemeImporter.h"
#include "wx/versioninfo.h"
#if wxCHECK_VERSION(3, 1, 0)
#include "EclipseJsonThemeImporter.hpp"
#include "EclipseRustThemeImporter.h"
#endif
#include "EclipseRubyThemeImporter.hpp"
#include "EclipseSCSSThemeImporter.h"
#include "EclipseSQLThemeImporter.h"
#include "EclipseScriptThemeImporter.h"
#include "EclipseTextThemeImporter.h"
#include "EclipseThemeImporterManager.h"
#include "EclipseXMLThemeImporter.h"
#include "EclipseYAMLThemeImporter.h"

EclipseThemeImporterManager::EclipseThemeImporterManager()
{
    m_importers.push_back(new EclipseCXXThemeImporter());
    m_importers.push_back(new EclipseCMakeThemeImporter());
    m_importers.push_back(new EclipseTextThemeImporter());
    m_importers.push_back(new EclipseMakefileThemeImporter());
    m_importers.push_back(new EclipseDiffThemeImporter());
    m_importers.push_back(new EclipsePHPThemeImporter());
    m_importers.push_back(new EclipseCSSThemeImporter());
    m_importers.push_back(new EclipseXMLThemeImporter());
    m_importers.push_back(new EclipseJavaScriptThemeImporter());
    m_importers.push_back(new EclipseINIThemeImporter());
    m_importers.push_back(new EclipseASMThemeImporter());
    m_importers.push_back(new EclipseBatchThemeImporter());
    m_importers.push_back(new EclipsePythonThemeImporter());
    m_importers.push_back(new EclipseCobraThemeImporter());
    m_importers.push_back(new EclipseCobraAltThemeImporter());
    m_importers.push_back(new EclipseFortranThemeImporter());
    m_importers.push_back(new EclipseInnoSetupThemeImporter());
    m_importers.push_back(new EclipseJavaThemeImporter());
    m_importers.push_back(new EclipseLuaThemeImporter());
    m_importers.push_back(new EclipseScriptThemeImporter());
    m_importers.push_back(new EclipseSQLThemeImporter());
    m_importers.push_back(new EclipseSCSSThemeImporter());
    m_importers.push_back(new EclipseDockerfileThemeImporter());
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
