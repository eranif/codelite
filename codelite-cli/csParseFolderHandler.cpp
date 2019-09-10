#include "csParseFolderHandler.h"
#include "csParsePHPFolderHandler.h"
#include <file_logger.h>

csParseFolderHandler::csParseFolderHandler(csManager* manager)
    : csCommandHandlerBase(manager)
{
    m_parseHandlers.Register("parse-php-folder", csParsePHPFolderHandler::Ptr_t(new csParsePHPFolderHandler(manager)));
}

csParseFolderHandler::~csParseFolderHandler() {}

void csParseFolderHandler::DoProcessCommand(const JSONItem& options)
{
    CHECK_STR_PARAM("lang", m_language);
    CHECK_STR_PARAM("path", m_path);

    bool isDir = wxDirExists(m_path);
    wxString handlerName;
    handlerName << "parse-" << m_language << "-" << (isDir ? "folder" : "file");
    csCommandHandlerBase::Ptr_t handler = m_parseHandlers.FindHandler(handlerName);
    if(!handler) {
        clERROR() << "I have no handler for:" << handlerName;
        return;
    }
    clDEBUG() << "Using handler:" << handlerName;
    handler->DoProcessCommand(options);
}
