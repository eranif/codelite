#ifndef __WordCompletion__
#define __WordCompletion__

#include "plugin.h"
#include "WordCompletionRequestReply.h"
#include "UI.h"
#include "cl_command_event.h"

class WordCompletionThread;
class WordCompletionPlugin : public IPlugin
{
    WordCompletionThread* m_thread;
    WordCompletionImages m_images;
    
public:
    /**
     * @brief this function is called by the word completion thread when parsing phase is done
     * @param reply
     */
    void OnSuggestThread(const WordCompletionThreadReply& reply);
    void OnWordComplete(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnEditorHandler(clCommandEvent &event);
public:
    WordCompletionPlugin(IManager* manager);
    ~WordCompletionPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // WordCompletion
