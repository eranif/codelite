#ifndef ECLIPSEJSONTHEMEIMPORTER_HPP
#define ECLIPSEJSONTHEMEIMPORTER_HPP

#include "ThemeImporterBase.hpp" // Base class: ThemeImporterBase
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeImporterJson : public ThemeImporterBase
{
public:
    ThemeImporterJson();
    virtual ~ThemeImporterJson();

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};
#endif // ECLIPSEJSONTHEMEIMPORTER_HPP
