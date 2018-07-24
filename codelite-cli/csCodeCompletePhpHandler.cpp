#include "csCodeCompletePhpHandler.h"

csCodeCompletePhpHandler::csCodeCompletePhpHandler(wxEvtHandler* sink)
    : csCommandHandlerBase(sink)
{
}

csCodeCompletePhpHandler::~csCodeCompletePhpHandler() {}

void csCodeCompletePhpHandler::DoProcessCommand(const JSONElement& options) 
{
    CHECK_STR_PARAM("path", m_path);
    CHECK_INT_PARAM("position", m_position);
}
