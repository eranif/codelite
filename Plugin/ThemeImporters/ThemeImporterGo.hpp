#pragma once

#include "ThemeImporterBase.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class ThemeImporterGo : public ThemeImporterBase
{
public:
    ThemeImporterGo();
    virtual ~ThemeImporterGo() = default;
    virtual LexerConf::Ptr_t Import(const wxFileName& theme_file);
};
