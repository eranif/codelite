#ifndef CLPERSISTENCEMANAGER_H
#define CLPERSISTENCEMANAGER_H

#include <wx/persist.h> // Base class: wxPersistenceManager
#include "clIniFile.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clPersistenceManager : public wxPersistenceManager
{
    clIniFile* m_iniFile;

public:
    clPersistenceManager();
    virtual ~clPersistenceManager();

    /**
     * @brief return the configuration object to use
     */
    virtual wxConfigBase* GetConfig() const;
};

#endif // CLPERSISTENCEMANAGER_H
