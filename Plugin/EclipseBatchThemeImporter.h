#ifndef ECLIPSEBATCHTHEMEIMPORTER_H
#define ECLIPSEBATCHTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseBatchThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseBatchThemeImporter();
    ~EclipseBatchThemeImporter();
};

#endif // ECLIPSEBATCHTHEMEIMPORTER_H
