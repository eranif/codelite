#ifndef LANGUAGESERVERLOGVIEW_H
#define LANGUAGESERVERLOGVIEW_H
#include "UI.h"

class LanguageServerLogView : public LanguageServerLogViewBase
{
public:
    LanguageServerLogView(wxWindow* parent);
    virtual ~LanguageServerLogView();
};
#endif // LANGUAGESERVERLOGVIEW_H
