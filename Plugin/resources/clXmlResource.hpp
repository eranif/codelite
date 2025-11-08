#pragma once

#include "codelite_exports.h"

#include <wx/menu.h>

class WXDLLIMPEXP_SDK clXmlResource : public wxEvtHandler
{
public:
    /**
     * @brief Retrieves the singleton instance of {@code clXmlResource}.
     *
     * This function implements the singleton pattern by creating a static {@code clXmlResource}
     * object on the first call and returning a reference to it on subsequent calls.
     *
     * @return {@code clXmlResource&} Reference to the singleton {@code clXmlResource} instance.
     */
    static clXmlResource& Get();

    /**
     * @brief Initializes the XML resource system by registering all default handlers.
     *
     * This method invokes {@code wxXmlResource::Get()->InitAllHandlers()} to set up the
     * default XML resource handlers used by the application.
     */
    static void Init();

    /**
     * @brief Loads a menu bar resource identified by its name.
     *
     * @param name The name of the menu bar resource to load.
     *
     * @return A pointer to the loaded {@code wxMenuBar} object.
     *         The caller is responsible for freeing this pointer.
     */
    wxMenuBar* LoadMenuBar(const wxString& name);

    /**
     * @brief Loads a menu resource identified by the given name.
     *
     * This function retrieves the global wxXmlResource instance and loads a menu
     * resource with the specified name, returning a pointer to the newly created
     * wxMenu object.
     *
     * @param name The name of the menu resource to load.
     *
     * @return A pointer to the loaded wxMenu. The caller is responsible for freeing
     *         this pointer when it is no longer needed.
     */
    wxMenu* LoadMenu(const wxString& name);

private:
    clXmlResource();
    ~clXmlResource() override;

    void OnMenuShown(wxMenuEvent& event);
};
