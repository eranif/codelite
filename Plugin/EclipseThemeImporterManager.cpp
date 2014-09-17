#include "EclipseThemeImporterManager.h"
#include "EclipseCXXThemeImporter.h"
#include "EclipseCMakeThemeImporter.h"
#include "EclipseTextThemeImporter.h"

EclipseThemeImporterManager::EclipseThemeImporterManager()
{
    m_importers.push_back(new EclipseCXXThemeImporter());
    m_importers.push_back(new EclipseCMakeThemeImporter());
    m_importers.push_back(new EclipseTextThemeImporter());
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
    for(size_t i=0; i<files.size(); ++i) {
        Import(files.at(i).GetFullPath());
    }
    return true;
}
