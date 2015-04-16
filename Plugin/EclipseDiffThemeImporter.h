#ifndef ECLIPSEDIFFTHEMEIMPORTER_H
#define ECLIPSEDIFFTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseDiffThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseDiffThemeImporter();
    virtual ~EclipseDiffThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEDIFFTHEMEIMPORTER_H
