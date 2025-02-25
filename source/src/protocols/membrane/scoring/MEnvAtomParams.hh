// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/membrane/scoring/MEnvAtomParams.hh
/// @brief A container for storing memrbane environemnt parameters and derivatives
/// @author Rebecca Alford (rfalford12@gmail.com)

#ifndef INCLUDED_protocols_membrane_scoring_MEnvAtomParams_hh
#define INCLUDED_protocols_membrane_scoring_MEnvAtomParams_hh

#include <protocols/membrane/scoring/MEnvAtomParams.fwd.hh>

// Basic headers
#include <core/types.hh>
#include <numeric/xyzVector.hh>

// Utility headers
#include <utility/VirtualBase.hh>

namespace protocols {
namespace membrane {
namespace scoring {

/// @brief A container for storing memrbane environemnt parameters and derivatives
class MEnvAtomParams : public utility::VirtualBase {

public:

	MEnvAtomParams();

	MEnvAtomParams(
		std::string const & atom_type_name,
		core::Real const dGfreeW,
		core::Real const dGfreeB,
		core::Real const hyd,
		core::Vector const f1,
		core::Vector const f2
	);

	MEnvAtomParams(MEnvAtomParams const & src);

	~MEnvAtomParams() override;

	MEnvAtomParamsOP
	clone() const;

	void set_hydration( core::Real hyd ) { hyd_ = hyd; }

	std::string atom_type_name() const { return atom_type_name_; }
	core::Real dGfreeW() const { return dGfreeW_; }
	core::Real dGfreeB() const { return dGfreeB_; }
	core::Real hydration() const { return hyd_; }
	core::Vector f1() const { return f1_; }
	core::Vector f2() const { return f2_; }

private:

	std::string atom_type_name_;
	core::Real dGfreeW_;
	core::Real dGfreeB_;
	core::Real hyd_;
	numeric::xyzVector< core::Real > f1_;
	numeric::xyzVector< core::Real > f2_;

};

} //protocols
} //membrane
} //scoring

#endif //INCLUDED_protocols_membrane_scoring_MEnvAtomParams_hh

