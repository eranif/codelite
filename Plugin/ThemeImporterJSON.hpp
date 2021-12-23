#ifndef ECLIPSEJSONTHEMEIMPORTER_HPP
#define ECLIPSEJSONTHEMEIMPORTER_HPP

#include "ThemeImporterBase.hpp" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"

#if wxCHECK_VERSION(3, 1, 0)
class WXDLLIMPEXP_SDK EclipseJsonThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipseJsonThemeImporter();
    virtual ~EclipseJsonThemeImporter();

public:
    LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile) override;
};
#endif
#endif // ECLIPSEJSONTHEMEIMPORTER_HPP
