#ifndef __treebooknodebase__
#define __treebooknodebase__

#include "editor_config.h"

class TreeBookNodeBase
{
public:
	TreeBookNodeBase();
	virtual ~TreeBookNodeBase();

	virtual void Save(OptionsConfigPtr options)  = 0;
};

template<typename DerivedT>
class TreeBookNode : public TreeBookNodeBase{

public:
	TreeBookNode()
	: TreeBookNodeBase() {
	}

	virtual ~TreeBookNode(){
	}
};
#endif // __treebooknodebase__
