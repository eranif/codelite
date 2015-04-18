#ifndef ECLIPSEJAVATHEMEIMPORTER_H
#define ECLIPSEJAVATHEMEIMPORTER_H

#include "codelite_exports.h"
#include "EclipseThemeImporterBase.h"

class WXDLLIMPEXP_SDK EclipseJavaThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseJavaThemeImporter();
    virtual ~EclipseJavaThemeImporter();
};

#endif // ECLIPSEJAVATHEMEIMPORTER_H
