#ifndef ECLIPSECMAKETHEMEIMPORTER_H
#define ECLIPSECMAKETHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseCMakeThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseCMakeThemeImporter();
    virtual ~EclipseCMakeThemeImporter();
    
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSECMAKETHEMEIMPORTER_H
