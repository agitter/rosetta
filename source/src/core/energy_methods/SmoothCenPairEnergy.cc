// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/SmoothCenPairEnergy.cc
/// @brief  Smooth, differentiable, version of cenpair
/// @author Frank DiMaio


// Unit headers
#include <core/energy_methods/SmoothCenPairEnergy.hh>
#include <core/energy_methods/SmoothCenPairEnergyCreator.hh>

// Package headers
#include <core/scoring/SmoothEnvPairPotential.hh>
#include <core/scoring/ScoringManager.hh>
#include <core/scoring/DerivVectorPair.hh>

// Project headers
#include <core/pose/Pose.hh>
#include <core/conformation/Residue.hh>
#include <core/chemical/VariantType.hh>

#include <numeric/xyzVector.hh>

#include <core/scoring/EnergyMap.hh>
#include <utility/vector1.hh>

namespace core {
namespace energy_methods {


core::scoring::methods::EnergyMethodOP
SmoothCenPairEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const &
) const {
	return utility::pointer::make_shared< SmoothCenPairEnergy >();
}

/// @brief Return the set of score types claimed by the EnergyMethod
/// this EnergyMethodCreator creates in its create_energy_method() function
core::scoring::ScoreTypes
SmoothCenPairEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( cen_pair_smooth );
	sts.push_back( cenpack_smooth );
	return sts;
}


/// c-tor
SmoothCenPairEnergy::SmoothCenPairEnergy() :
	parent( utility::pointer::make_shared< SmoothCenPairEnergyCreator >() ),
	potential_( core::scoring::ScoringManager::get_instance()->get_SmoothEnvPairPotential() )
{}


/// clone
core::scoring::methods::EnergyMethodOP
SmoothCenPairEnergy::clone() const {
	return utility::pointer::make_shared< SmoothCenPairEnergy >();
}


/////////////////////////////////////////////////////////////////////////////
// scoring
/////////////////////////////////////////////////////////////////////////////


void
SmoothCenPairEnergy::setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & ) const
{
	// compute interpolated number of neighbors at various distance cutoffs
	potential_.compute_centroid_environment( pose );
	pose.update_residue_neighbors();
}


//////////////////////////////////////////////////////////////
//
//     CENTROID PAIR SCORE
//      and
//     "CENTROID PACK" SCORE (helps reproduce pairwise correlations
//                            between centroids, observed in PDB)
//
void
SmoothCenPairEnergy::residue_pair_energy(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const &,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap & emap
) const {
	// ignore scoring residues which have been marked as "REPLONLY" residues (only the repulsive energy will be calculated)
	if ( rsd1.has_variant_type( core::chemical::REPLONLY ) || rsd2.has_variant_type( core::chemical::REPLONLY ) ) return;
	if ( rsd1.aa() > core::chemical::num_canonical_aas || rsd2.aa() > core::chemical::num_canonical_aas ) return;

	/// assumes centroids are being used
	conformation::Atom const & cen1 ( rsd1.atom( rsd1.nbr_atom() ) ), cen2 (rsd2.atom( rsd2.nbr_atom() ) );
	Real const cendist = cen1.xyz().distance_squared( cen2.xyz() );

	/// accumulate total energies
	Real pair_score( 0.0 ), cenpack_score( 0.0 );
	potential_.evaluate_pair_and_cenpack_score( rsd1, rsd2, cendist, pair_score, cenpack_score );

	//fpd I'll just keep these magic constants here
	pair_score *= 2.019f;
	cenpack_score *= 2.0f;

	//fpd Get rid of secstruct weighing
	emap[ core::scoring::cen_pair_smooth ] += pair_score;
	emap[ core::scoring::cenpack_smooth ]  += cenpack_score;
}

void
SmoothCenPairEnergy::eval_residue_pair_derivatives(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	core::scoring::ResSingleMinimizationData const &,
	core::scoring::ResSingleMinimizationData const &,
	core::scoring::ResPairMinimizationData const &,
	pose::Pose const &,
	core::scoring::EnergyMap const & weights,
	utility::vector1< core::scoring::DerivVectorPair > & r1_atom_derivs,
	utility::vector1< core::scoring::DerivVectorPair > & r2_atom_derivs
) const {
	if ( rsd1.has_variant_type( core::chemical::REPLONLY ) || rsd2.has_variant_type( core::chemical::REPLONLY ) ) return;
	if ( rsd1.aa() > core::chemical::num_canonical_aas || rsd2.aa() > core::chemical::num_canonical_aas ) return;

	Real weight1 = weights[ core::scoring::cen_pair_smooth ];
	Real weight2 = weights[ core::scoring::cenpack_smooth ];

	/// assumes centroids are being used
	conformation::Atom const & cen1 ( rsd1.atom( rsd1.nbr_atom() ) ), cen2 (rsd2.atom( rsd2.nbr_atom() ) );
	Real const cendist = cen1.xyz().distance_squared( cen2.xyz() );
	Real dpair_dr, dcenpack_dr;
	potential_.evaluate_pair_and_cenpack_deriv( rsd1, rsd2, cendist, dpair_dr, dcenpack_dr );

	// again, multiply by magic constants
	dpair_dr *= 2.019f;
	dcenpack_dr *= 2.0f;

	Vector atom_x = rsd1.atom(rsd1.nbr_atom()).xyz();
	Vector atom_y = rsd2.atom(rsd2.nbr_atom()).xyz();

	Vector f1( atom_x.cross( atom_y ) );
	Vector f2( atom_x - atom_y );
	Real const dis( f2.length() );
	dpair_dr /= dis;
	dcenpack_dr /= dis;

	f1 *= (weight1 * dpair_dr + weight2 * dcenpack_dr);
	f2 *= (weight1 * dpair_dr + weight2 * dcenpack_dr);

	r1_atom_derivs[ rsd1.nbr_atom() ].f1() += f1;
	r1_atom_derivs[ rsd1.nbr_atom() ].f2() += f2;
	r2_atom_derivs[ rsd2.nbr_atom() ].f1() -= f1;
	r2_atom_derivs[ rsd2.nbr_atom() ].f2() -= f2;
}


void
SmoothCenPairEnergy::finalize_total_energy(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap &
) const {
	potential_.finalize( pose );
}

/// @brief SmoothCenPairEnergy distance cutoff
Distance
SmoothCenPairEnergy::atomic_interaction_cutoff() const {
	return 7.5;  // extra added to ensure full sigmoid is covered
}

core::Size
SmoothCenPairEnergy::version() const {
	return 1; // Initial versioning
}

}
}
