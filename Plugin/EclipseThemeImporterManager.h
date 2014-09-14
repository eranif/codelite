#ifndef ECLIPSETHEMEIMPORTERMANAGER_H
#define ECLIPSETHEMEIMPORTERMANAGER_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "EclipseThemeImporterBase.h"

class WXDLLIMPEXP_SDK EclipseThemeImporterManager
{
    EclipseThemeImporterBase::List_t m_importers;
    
public:
    EclipseThemeImporterManager();
    virtual ~EclipseThemeImporterManager();
    
    bool Import(const wxString& eclipseXml);
};

#endif // ECLIPSETHEMEIMPORTERMANAGER_H
