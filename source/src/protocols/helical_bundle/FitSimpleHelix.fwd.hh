// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/helical_bundle/FitSimpleHelix.fwd.hh
/// @brief  Defines owning pointers for FitSimpleHelix mover class.
/// @author Vikram K. Mulligan (vmullig@uw.edu)

#ifndef INCLUDED_protocols_helical_bundle_FitSimpleHelix_fwd_hh
#define INCLUDED_protocols_helical_bundle_FitSimpleHelix_fwd_hh

#include <utility/pointer/owning_ptr.hh>
#include <utility/vector1.fwd.hh>

namespace protocols {
namespace helical_bundle {

class FitSimpleHelix; // fwd declaration
typedef utility::pointer::shared_ptr< FitSimpleHelix > FitSimpleHelixOP;
typedef utility::pointer::shared_ptr< FitSimpleHelix const > FitSimpleHelixCOP;
typedef utility::vector1<FitSimpleHelixOP> FitSimpleHelixOPs;
typedef utility::vector1<FitSimpleHelixCOP> FitSimpleHelixCOPs;

} // namespace helical_bundle
} // namespace protocols

#endif // INCLUDED_protocols_helical_bundle_FitSimpleHelix_fwd_hh
