#ifndef ECLIPSESCSSTHEMEIMPORTER_H
#define ECLIPSESCSSTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"

class WXDLLIMPEXP_SDK EclipseSCSSThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseSCSSThemeImporter();
    virtual ~EclipseSCSSThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSESCSSTHEMEIMPORTER_H
