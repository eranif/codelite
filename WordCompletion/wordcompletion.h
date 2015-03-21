#ifndef __WordCompletion__
#define __WordCompletion__

#include "plugin.h"
#include "WordCompletionRequestReply.h"
#include "UI.h"
#include "cl_command_event.h"
#include "macros.h"

class WordCompletionDictionary;
class WordCompletionPlugin : public IPlugin
{
    WordCompletionImages m_images;
    WordCompletionDictionary* m_dictionary;
    
public:
    void OnWordComplete(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
        
public:
    WordCompletionPlugin(IManager* manager);
    ~WordCompletionPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // WordCompletion
