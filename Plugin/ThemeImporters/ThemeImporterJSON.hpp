#pragma once

#include "ThemeImporterBase.hpp" // Base class: ThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeImporterJson : public ThemeImporterBase
{
public:
    ThemeImporterJson();
    ~ThemeImporterJson() override = default;

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};
