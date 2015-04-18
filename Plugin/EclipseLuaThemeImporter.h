#ifndef ECLIPSELUATHEMEIMPORTER_H
#define ECLIPSELUATHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseLuaThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseLuaThemeImporter();
    virtual ~EclipseLuaThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSELUATHEMEIMPORTER_H
