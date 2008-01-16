#ifndef COMMENT_CREATOT_H
#define COMMENT_CREATOT_H

#include "wx/string.h"

/**
 * \ingroup CodeLite
 * an interface class to the comment creator. 
 *
 * \version 1.0
 * first version
 *
 * \date 07-20-2007
 *
 * \author Eran
 *
 */
class CommentCreator
{
public:
	CommentCreator(){}
	virtual ~CommentCreator(){}
	virtual wxString CreateComment() = 0;
};
#endif //COMMENT_CREATOT_H
