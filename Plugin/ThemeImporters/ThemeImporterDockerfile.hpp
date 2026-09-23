#ifndef ECLIPSEDOCKERFILETHEMEIMPORTER_H
#define ECLIPSEDOCKERFILETHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"

class WXDLLIMPEXP_SDK ThemeImporterDockerfile : public ThemeImporterBase
{
public:
    ThemeImporterDockerfile();
    ~ThemeImporterDockerfile() override = default;

public:
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};

#endif // ECLIPSEDOCKERFILETHEMEIMPORTER_H
