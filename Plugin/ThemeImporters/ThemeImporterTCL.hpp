#pragma once

#include "ThemeImporterBase.hpp" // Base class: ThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeImporterTCL : public ThemeImporterBase
{
public:
    ThemeImporterTCL();
    ~ThemeImporterTCL() override = default;

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};
