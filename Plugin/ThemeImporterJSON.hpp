#ifndef ECLIPSEJSONTHEMEIMPORTER_HPP
#define ECLIPSEJSONTHEMEIMPORTER_HPP

#include "ThemeImporterBase.hpp" // Base class: ThemeImporterBase
#include "codelite_exports.h"

#if wxCHECK_VERSION(3, 1, 0)
class WXDLLIMPEXP_SDK ThemeImporterJson : public ThemeImporterBase
{
public:
    ThemeImporterJson();
    virtual ~ThemeImporterJson();

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};
#endif
#endif // ECLIPSEJSONTHEMEIMPORTER_HPP
