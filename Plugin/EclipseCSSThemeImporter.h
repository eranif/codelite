#ifndef ECLIPSECSSTHEMEIMPORTER_H
#define ECLIPSECSSTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase

class WXDLLIMPEXP_SDK EclipseCSSThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseCSSThemeImporter();
    virtual ~EclipseCSSThemeImporter();

public:
    virtual bool Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSECSSTHEMEIMPORTER_H
