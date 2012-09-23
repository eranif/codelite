#ifndef OUTLINESETTINGS_H
#define OUTLINESETTINGS_H

#include <wx/string.h>
#include "json_node.h"

class OutlineSettings
{
    
public:
    OutlineSettings();
    virtual ~OutlineSettings();

    
    void Load();
    void Save();
};

#endif // OUTLINESETTINGS_H
