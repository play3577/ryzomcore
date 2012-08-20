/**
 * \file scene_class_registry.cpp
 * \brief CSceneClassRegistry
 * \date 2012-08-20 09:57GMT
 * \author Jan Boon (Kaetemi)
 * CSceneClassRegistry
 */

/*
 * Copyright (C) 2012  by authors
 *
 * This file is part of RYZOM CORE PIPELINE.
 * RYZOM CORE PIPELINE is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * RYZOM CORE PIPELINE is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with RYZOM CORE PIPELINE.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <nel/misc/types_nl.h>
#include "scene_class_registry.h"

// STL includes

// NeL includes
// #include <nel/misc/debug.h>

// Project includes

using namespace std;
// using namespace NLMISC;

namespace PIPELINE {
namespace MAX {

CSceneClassRegistry::CSceneClassRegistry()
{

}

CSceneClassRegistry::~CSceneClassRegistry()
{

}

//void CSceneClassRegistry::add(const NLMISC::CClassId, const ISceneClassDesc *desc);
//void CSceneClassRegistry::remove(const NLMISC::CClassId);

CSceneClass *CSceneClassRegistry::create(const NLMISC::CClassId classid) const
{
	return NULL; // TODO
}

//void CSceneClassRegistry::destroy(CSceneClass *sceneClass) const;
//const CSceneClassRegistry::ISceneClassDesc *describe(const NLMISC::CClassId classid) const;

} /* namespace MAX */
} /* namespace PIPELINE */

/* end of file */
