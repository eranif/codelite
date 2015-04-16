#ifndef ECLIPSEXMLTHEMEIMPORTER_H
#define ECLIPSEXMLTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseXMLThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseXMLThemeImporter();
    virtual ~EclipseXMLThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEXMLTHEMEIMPORTER_H
