#ifndef ECLIPSEDOCKERFILETHEMEIMPORTER_H
#define ECLIPSEDOCKERFILETHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"

class WXDLLIMPEXP_SDK ThemeImporterDockerfile : public EclipseThemeImporterBase
{
public:
    ThemeImporterDockerfile();
    virtual ~ThemeImporterDockerfile();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEDOCKERFILETHEMEIMPORTER_H
