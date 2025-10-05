#ifndef THEME_IMPORTER_TCL_HPP
#define THEME_IMPORTER_TCL_HPP

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
#endif // THEME_IMPORTER_TCL_HPP
