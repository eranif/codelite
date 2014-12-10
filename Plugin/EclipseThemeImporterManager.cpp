#include "EclipseThemeImporterManager.h"
#include "EclipseCXXThemeImporter.h"
#include "EclipseCMakeThemeImporter.h"
#include "EclipseTextThemeImporter.h"
#include "EclipseMakefileThemeImporter.h"
#include "EclipseDiffThemeImporter.h"
#include "EclipsePHPThemeImporter.h"
#include "EclipseCSSThemeImporter.h"

EclipseThemeImporterManager::EclipseThemeImporterManager()
{
    m_importers.push_back(new EclipseCXXThemeImporter());
    m_importers.push_back(new EclipseCMakeThemeImporter());
    m_importers.push_back(new EclipseTextThemeImporter());
    m_importers.push_back(new EclipseMakefileThemeImporter());
    m_importers.push_back(new EclipseDiffThemeImporter());
    m_importers.push_back(new EclipsePHPThemeImporter());
    m_importers.push_back(new EclipseCSSThemeImporter());
}

EclipseThemeImporterManager::~EclipseThemeImporterManager() {}

bool EclipseThemeImporterManager::Import(const wxString& eclipseXml)
{
    EclipseThemeImporterBase::List_t::iterator iter = m_importers.begin();
    for(; iter != m_importers.end(); ++iter) {
        (*iter)->Import(eclipseXml);
    }
    return true;
}

bool EclipseThemeImporterManager::ImportCxxToAll()
{
    std::vector<wxFileName> files = EclipseCXXThemeImporter::ToEclipseXMLs();
    for(size_t i = 0; i < files.size(); ++i) {
        wxString eclipseXml = files.at(i).GetFullPath();
        EclipseThemeImporterBase::List_t::iterator iter = m_importers.begin();
        for(; iter != m_importers.end(); ++iter) {
            if((*iter)->GetLangName() != "c++") {
                (*iter)->Import(eclipseXml);
            }
        }
    }
    return true;
}
