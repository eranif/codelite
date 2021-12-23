#ifndef ECLIPSEYAMLTHEMEIMPORTER_H
#define ECLIPSEYAMLTHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"

class WXDLLIMPEXP_SDK EclipseYAMLThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseYAMLThemeImporter();
    virtual ~EclipseYAMLThemeImporter();

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEYAMLTHEMEIMPORTER_H
