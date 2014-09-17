#ifndef ECLIPSETEXTTHEMEIMPORTER_H
#define ECLIPSETEXTTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseTextThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseTextThemeImporter();
    virtual ~EclipseTextThemeImporter();

public:
    virtual bool Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSETEXTTHEMEIMPORTER_H
