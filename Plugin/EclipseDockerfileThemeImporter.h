#ifndef ECLIPSEDOCKERFILETHEMEIMPORTER_H
#define ECLIPSEDOCKERFILETHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"

class WXDLLIMPEXP_SDK EclipseDockerfileThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseDockerfileThemeImporter();
    virtual ~EclipseDockerfileThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEDOCKERFILETHEMEIMPORTER_H
