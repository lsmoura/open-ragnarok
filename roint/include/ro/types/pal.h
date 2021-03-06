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
#ifndef __RO_TYPES_PAL_H
#define __RO_TYPES_PAL_H

#include "../ro_object.h"

namespace ro {

/**
 * Palette class.
 * Contains an array of 256 palette entries.
 * The first index is sometimes considered invisible.
 *
 * \ingroup ROInterface
 */
class ROINT_DLLAPI PAL : public Object {
public:
#pragma pack(push,1)
	/**
	 * Palette entry.
	 * Represents a color with red, green and blue components.
	 */
	struct Color {
		unsigned char r, g, b, reserved;
		inline operator unsigned char* () { return(&r); }
		inline operator const unsigned char* () const { return(&r); }
	};
#pragma pack(pop)

protected:
	void reset();

	Color m_pal[256];

public:
	PAL();
	virtual ~PAL();

	virtual bool readStream(std::istream&);

	/** Returns the palette entry */
	const Color& getColor(unsigned char idx) const;
};

} /* namespace ro */

#endif /* __RO_TYPES_PAL_H */

