/* $Id$ */
/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of The Open Ragnarok Project
    Copyright 2007 - 2009 The Open Ragnarok Team
    For the latest information visit http://www.open-ragnarok.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
*/
#ifndef __RO_PYTHON_PYACT_H
#define __RO_PYTHON_PYACT_H

#include "../types/act.h"

namespace ro {
extern "C" {

ROINT_DLLAPI ACT* ACT_new();
ROINT_DLLAPI void ACT_del(ACT*);

ROINT_DLLAPI bool ACT_read(ACT*, const char*);

ROINT_DLLAPI void ACT_dump(const ACT*, const char*);
ROINT_DLLAPI unsigned int ACT_count(const ACT*);

} /* extern "C" */
} /* namespace ro */

#endif /* __RO_PYTHON_PYACT_H */
