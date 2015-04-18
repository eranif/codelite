#ifndef ECLIPSEFORTRANTHEMEIMPORTER_H
#define ECLIPSEFORTRANTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseFortranThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseFortranThemeImporter();
    virtual ~EclipseFortranThemeImporter();

};

#endif // ECLIPSEFORTRANTHEMEIMPORTER_H
