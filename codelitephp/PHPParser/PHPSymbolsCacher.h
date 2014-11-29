#ifndef PHPSYMBOLSCACHER_H
#define PHPSYMBOLSCACHER_H

#include "job.h" // Base class: Job
#include <wx/string.h>
#include <wx/event.h>

class PHPCodeCompletion;
class PHPSymbolsCacher : public Job
{
    PHPCodeCompletion* m_owner;
    wxString m_filename;
    
public:
    PHPSymbolsCacher(PHPCodeCompletion* owner, const wxString& dbfile);
    virtual ~PHPSymbolsCacher();

public:
    virtual void Process(wxThread* thread);
};

#endif // PHPSYMBOLSCACHER_H
