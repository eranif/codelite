#ifndef ECLIPSEINITHEMEIMPORTER_H
#define ECLIPSEINITHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseINIThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    
    EclipseINIThemeImporter();
    ~EclipseINIThemeImporter();
};

#endif // ECLIPSEINITHEMEIMPORTER_H
