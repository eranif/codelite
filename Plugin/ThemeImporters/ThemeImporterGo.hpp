#pragma once

#include "ThemeImporterBase.hpp"
#include "codelite_exports.h"

#include <wx/filename.h>

class ThemeImporterGo : public ThemeImporterBase
{
public:
    ThemeImporterGo();
    ~ThemeImporterGo() override = default;
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};
