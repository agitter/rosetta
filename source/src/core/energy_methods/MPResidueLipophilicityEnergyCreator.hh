// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file  core/scoring/membrane/MPResidueLipophilicityEnergyCreator.hh
///
/// @brief  Fullatom Smoothed Membrane Environment Energy
/// @details residue speicific enrgy by membrane depth, according to the Elazar
/// hydrophobicity scale
///    @FlesihmanLab.
///    Last Modified: 4/4/16
///
/// @author  Jonathan Weinstein (jonathan.weinstein@weizmann.ac.il)
/// @author  Assaf Elazar
/// @author Sarel Fleishman

#ifndef INCLUDED_core_scoring_membrane_MPResidueLipophilicityEnergyCreator_hh
#define INCLUDED_core_scoring_membrane_MPResidueLipophilicityEnergyCreator_hh

// Unit Headers
#include <core/scoring/methods/EnergyMethodCreator.hh>

// Package Headers
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/scoring/methods/EnergyMethod.fwd.hh>

// Utility Headers

// C++ Headers

namespace core {
namespace energy_methods {


/// @brief Membrane Termini Penalty Creator Class
class MPResidueLipophilicityEnergyCreator : public core::scoring::methods::EnergyMethodCreator
{
public:

	/// @brief Instantiate a new MPResidueLipophilicityEnergy
	core::scoring::methods::EnergyMethodOP
	create_energy_method(
		core::scoring::methods::EnergyMethodOptions const &
	) const override;

	/// @brief Return the set of score types claimed by the EnergyMethod
	/// this EnergyMethodCreator creates in its create_energy_method() function
	core::scoring::ScoreTypes
	score_types_for_method() const override;

};

} // scoring
} // core

#endif // INCLUDED_core_scoring_membrane_MPResidueLipophilicityEnergyCreator_hh
