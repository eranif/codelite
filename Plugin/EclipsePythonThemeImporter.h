#ifndef ECLIPSEPYTHONTHEMEIMPORTER_H
#define ECLIPSEPYTHONTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipsePythonThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipsePythonThemeImporter();
    ~EclipsePythonThemeImporter();
};

#endif // ECLIPSEPYTHONTHEMEIMPORTER_H
