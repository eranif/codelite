#ifndef ECLIPSECXXTHEMEIMPORTER_H
#define ECLIPSECXXTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseCXXThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseCXXThemeImporter();
    virtual ~EclipseCXXThemeImporter();
    
    /**
     * @brief import an eclipse XML colour theme
     * @param eclipseXmlFile
     * @param codeliteXml [output] the output file name
     */
    bool Import(const wxFileName& eclipseXmlFile, wxString &codeliteXml);
};

#endif // ECLIPSECXXTHEMEIMPORTER_H
