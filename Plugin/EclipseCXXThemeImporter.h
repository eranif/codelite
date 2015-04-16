#ifndef ECLIPSECXXTHEMEIMPORTER_H
#define ECLIPSECXXTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"
#include <vector>

class WXDLLIMPEXP_SDK EclipseCXXThemeImporter : public EclipseThemeImporterBase
{
private:
    /**
     * @brief convert a single XML file to eclipse XML format
     */
    static wxFileName ToEclipseXML(const wxFileName& codeliteXml, size_t id);
    
public:
    EclipseCXXThemeImporter();
    virtual ~EclipseCXXThemeImporter();
    
    /**
     * @brief convert _all_ C++ xml files into eclipse format
     */
    static std::vector<wxFileName> ToEclipseXMLs();
    
    /**
     * @brief import an eclipse XML colour theme
     * @param eclipseXmlFile
     * @param codeliteXml [output] the output file name
     */
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSECXXTHEMEIMPORTER_H
