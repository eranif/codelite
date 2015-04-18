#ifndef ECLIPSESQLTHEMEIMPORTER_H
#define ECLIPSESQLTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseSQLThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseSQLThemeImporter();
    virtual ~EclipseSQLThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSESQLTHEMEIMPORTER_H
