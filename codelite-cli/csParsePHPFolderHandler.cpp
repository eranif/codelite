#include "PHPLookupTable.h"
#include "csParsePHPFolderHandler.h"
#include <wx/filename.h>

csParsePHPFolderHandler::csParsePHPFolderHandler(csManager* manager)
    : csCommandHandlerBase(manager)
{
}

csParsePHPFolderHandler::~csParsePHPFolderHandler() {}

void csParsePHPFolderHandler::DoProcessCommand(const JSONItem& options)
{
    CHECK_STR_PARAM("path", m_folder);
    CHECK_STR_PARAM("mask", m_mask);
    CHECK_STR_PARAM_OPTIONAL("symbols-path", m_dbpath);

    PHPLookupTable lookup;
    // Build the default symbols db path
    wxFileName dbpath(m_folder, "phpsymbols.db");
    dbpath.AppendDir(".codelite");

    // Allow the user to override
    if(!m_dbpath.IsEmpty()) {
        wxFileName userPath(m_dbpath);
        userPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        m_dbpath = userPath.GetFullPath();
        dbpath = m_dbpath;
    }
    
    clDEBUG() << "Using symbols db:" << dbpath;
    lookup.Open(dbpath);
    if(!lookup.IsOpened()) {
        clERROR() << "Could not open file:" << dbpath;
        return;
    }
    // Clear any content before we start the parsing
    lookup.ParseFolder(m_folder, m_mask, PHPLookupTable::kUpdateMode_Fast);
}
