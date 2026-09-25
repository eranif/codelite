#ifndef ECLIPSEYAMLTHEMEIMPORTER_H
#define ECLIPSEYAMLTHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"

class WXDLLIMPEXP_SDK ThemeImporterYAML : public ThemeImporterBase
{
public:
    ThemeImporterYAML();
    ~ThemeImporterYAML() override = default;

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};

#endif // ECLIPSEYAMLTHEMEIMPORTER_H
