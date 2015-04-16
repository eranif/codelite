#ifndef ECLIPSEJAVASCRIPTTHEMEIMPORTER_H
#define ECLIPSEJAVASCRIPTTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseJavaScriptThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseJavaScriptThemeImporter();
    virtual ~EclipseJavaScriptThemeImporter();
    
    /**
     * @brief import an eclipse XML colour theme
     * @param eclipseXmlFile
     * @param codeliteXml [output] the output file name
     */
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEJAVASCRIPTTHEMEIMPORTER_H
