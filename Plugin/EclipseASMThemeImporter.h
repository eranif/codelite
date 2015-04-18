#ifndef ECLIPSEASMTHEMEIMPORTER_H
#define ECLIPSEASMTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseASMThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseASMThemeImporter();
    ~EclipseASMThemeImporter();
};

#endif // ECLIPSEASMTHEMEIMPORTER_H
