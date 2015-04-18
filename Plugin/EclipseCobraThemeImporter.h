#ifndef ECLIPSECOBRATHEMEIMPORTER_H
#define ECLIPSECOBRATHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseCobraThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseCobraThemeImporter();
    ~EclipseCobraThemeImporter();
};

#endif // ECLIPSECOBRATHEMEIMPORTER_H
