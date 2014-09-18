#ifndef ECLIPSEMAKEFILETHEMEIMPORTER_H
#define ECLIPSEMAKEFILETHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseMakefileThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseMakefileThemeImporter();
    virtual ~EclipseMakefileThemeImporter();

public:
    virtual bool Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEMAKEFILETHEMEIMPORTER_H
