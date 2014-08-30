#ifndef PARSERTHREADUIPROGRESS_H
#define PARSERTHREADUIPROGRESS_H

#include "php_parser_thread.h" // Base class: PHPParserThreadProgressCB

class PHPWorkspaceView;
class ParserThreadUIProgress : public PHPParserThreadProgressCB
{
    PHPWorkspaceView* m_view;
    
public:
    ParserThreadUIProgress(PHPWorkspaceView* view);
    virtual ~ParserThreadUIProgress();

public:
    virtual void OnProgress(size_t currentIndex, size_t total);
    virtual void OnCompleted();
};

#endif // PARSERTHREADUIPROGRESS_H
