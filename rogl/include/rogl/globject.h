/* $Id$ */
#ifndef __GLOBJECT_H
#define __GLOBJECT_H

#include "rogl/rogl_import.h"

#ifdef WIN32
#	include <windows.h>
#endif // WIN32

namespace rogl {

class ROGL_DLLAPI GLObject {
public:
	virtual void Draw() const = 0;
};

}

#endif /* __GLOBJECT_H */
