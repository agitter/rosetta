// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/import_pose/libraries/RNA_LibraryManager.fwd.hh
/// @brief
/// @details
/// @author Rhiju Das, rhiju@stanford.edu


#ifndef INCLUDED_core_import_pose_RNA_LibraryManager_FWD_HH
#define INCLUDED_core_import_pose_RNA_LibraryManager_FWD_HH

#include <utility/pointer/owning_ptr.hh>

namespace core {
namespace import_pose {
namespace libraries {

class RNA_LibraryManager;
typedef utility::pointer::shared_ptr< RNA_LibraryManager > RNA_LibraryManagerOP;
typedef utility::pointer::shared_ptr< RNA_LibraryManager const > RNA_LibraryManagerCOP;

} //libraries
} //rna
} //protocols

#endif
