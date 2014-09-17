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
    
    /**
     * @brief scan, convert all C++ existing lexers into eclipse XML and then import them
     * to all other languages that we support. This is used internally and not exposed to 
     * the end user
     */
    bool ImportCxxToAll();
};

#endif // ECLIPSETHEMEIMPORTERMANAGER_H
