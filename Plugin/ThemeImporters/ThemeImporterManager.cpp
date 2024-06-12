#include "ThemeImporterManager.hpp"

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
#include "ThemeImporterGo.hpp"
#include "ThemeImporterINI.hpp"
#include "ThemeImporterInnoSetup.hpp"
#include "ThemeImporterJSON.hpp"
#include "ThemeImporterJava.hpp"
#include "ThemeImporterJavaScript.hpp"
#include "ThemeImporterLUA.hpp"
#include "ThemeImporterMakefile.hpp"
#include "ThemeImporterMarkdown.hpp"
#include "ThemeImporterPHP.hpp"
#include "ThemeImporterPython.hpp"
#include "ThemeImporterRuby.hpp"
#include "ThemeImporterRust.hpp"
#include "ThemeImporterSCSS.hpp"
#include "ThemeImporterSQL.hpp"
#include "ThemeImporterScript.hpp"
#include "ThemeImporterTCL.hpp"
#include "ThemeImporterText.hpp"
#include "ThemeImporterXML.hpp"
#include "ThemeImporterYAML.hpp"

#include <wx/versioninfo.h>

ThemeImporterManager::ThemeImporterManager()
{
    m_importers.push_back(std::make_unique<ThemeImporterCXX>());
    m_importers.push_back(std::make_unique<ThemeImporterCMake>());
    m_importers.push_back(std::make_unique<ThemeImporterText>());
    m_importers.push_back(std::make_unique<ThemeImporterMakefile>());
    m_importers.push_back(std::make_unique<ThemeImporterDiff>());
    m_importers.push_back(std::make_unique<ThemeImporterPHP>());
    m_importers.push_back(std::make_unique<ThemeImporterCSS>());
    m_importers.push_back(std::make_unique<ThemeImporterTCL>());
    m_importers.push_back(std::make_unique<ThemeImporterXML>());
    m_importers.push_back(std::make_unique<ThemeImporterJavaScript>());
    m_importers.push_back(std::make_unique<ThemeImporterINI>());
    m_importers.push_back(std::make_unique<ThemeImporterASM>());
    m_importers.push_back(std::make_unique<ThemeImporterBatch>());
    m_importers.push_back(std::make_unique<ThemeImporterPython>());
    m_importers.push_back(std::make_unique<ThemeImporterCobra>());
    m_importers.push_back(std::make_unique<ThemeImporterCobraAlt>());
    m_importers.push_back(std::make_unique<ThemeImporterFortran>());
    m_importers.push_back(std::make_unique<ThemeImporterInnoSetup>());
    m_importers.push_back(std::make_unique<ThemeImporterJava>());
    m_importers.push_back(std::make_unique<ThemeImporterLua>());
    m_importers.push_back(std::make_unique<ThemeImporterScript>());
    m_importers.push_back(std::make_unique<ThemeImporterSQL>());
    m_importers.push_back(std::make_unique<ThemeImporterSCSS>());
    m_importers.push_back(std::make_unique<ThemeImporterDockerfile>());
    m_importers.push_back(std::make_unique<ThemeImporterYAML>());
    m_importers.push_back(std::make_unique<ThemeImporterRuby>());
    m_importers.push_back(std::make_unique<ThemeImporterMarkdown>());
    m_importers.push_back(std::make_unique<ThemeImporterRust>());
    m_importers.push_back(std::make_unique<ThemeImporterJson>());
    m_importers.push_back(std::make_unique<ThemeImporterGo>());
}

ThemeImporterManager::~ThemeImporterManager() {}

wxString ThemeImporterManager::Import(const wxString& theme_file)
{
    // we add all or nothing
    wxString name;
    std::vector<LexerConf::Ptr_t> lexers;
    lexers.reserve(m_importers.size());
    for (auto& importer : m_importers) {
        auto lexer = importer->Import(theme_file);
        if (!lexer) {
            return wxEmptyString;
        }

        if (name.empty()) {
            name = lexer->GetThemeName();
        }
        lexers.emplace_back(lexer);
    }

    // add the lexers
    for (auto lexer : lexers) {
        ColoursAndFontsManager::Get().AddLexer(lexer);
    }
    return name;
}
