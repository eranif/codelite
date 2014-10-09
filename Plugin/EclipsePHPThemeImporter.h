#ifndef ECLIPSEPHPTHEMEIMPORTER_H
#define ECLIPSEPHPTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase

class EclipsePHPThemeImporter : public EclipseThemeImporterBase
{
public:
    EclipsePHPThemeImporter();
    virtual ~EclipsePHPThemeImporter();

public:
    virtual bool Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSEPHPTHEMEIMPORTER_H
