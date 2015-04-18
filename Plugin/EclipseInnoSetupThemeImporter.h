#ifndef ECLIPSEINNOSETUPTHEMEIMPORTER_H
#define ECLIPSEINNOSETUPTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseInnoSetupThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseInnoSetupThemeImporter();
    virtual ~EclipseInnoSetupThemeImporter();
};

#endif // ECLIPSEINNOSETUPTHEMEIMPORTER_H
