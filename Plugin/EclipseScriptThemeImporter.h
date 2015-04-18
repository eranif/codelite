#ifndef ECLIPSESCRIPTTHEMEIMPORTER_H
#define ECLIPSESCRIPTTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase

class WXDLLIMPEXP_SDK EclipseScriptThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseScriptThemeImporter();
    virtual ~EclipseScriptThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSESCRIPTTHEMEIMPORTER_H
