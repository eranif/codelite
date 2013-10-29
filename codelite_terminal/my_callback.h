#ifndef MYCALLBACK_H
#define MYCALLBACK_H

#include "asyncprocess.h" // Base class: IProcessCallback
#include <wx/stc/stc.h>

class MainFrame;
class MyCallback : public IProcessCallback
{
    MainFrame* m_frame;
    
public:
    virtual void OnProcessOutput(const wxString& str);
    virtual void OnProcessTerminated();
    
    MyCallback(MainFrame* frame);
    virtual ~MyCallback();

};

#endif // MYCALLBACK_H
