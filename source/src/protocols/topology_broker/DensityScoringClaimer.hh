// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file TopologyBroker
/// @brief  top-class (Organizer) of the TopologyBroker mechanism
/// @details responsibilities:
/// @author Oliver Lange


#ifndef INCLUDED_protocols_topology_broker_DensityScoringClaimer_hh
#define INCLUDED_protocols_topology_broker_DensityScoringClaimer_hh


// Unit Headers
#include <protocols/topology_broker/DensityScoringClaimer.fwd.hh>

// Package Headers
#include <protocols/topology_broker/TopologyClaimer.hh>
#include <protocols/topology_broker/claims/DofClaim.fwd.hh>


// Project Headers
#include <core/pose/Pose.fwd.hh>
//#include <core/fragment/FragSet.hh>


// ObjexxFCL Headers

// Utility headers
//#include <utility/io/izstream.hh>
//#include <utility/io/ozstream.hh>
//#include <utility/io/util.hh>

//// C++ headers
//#include <iosfwd>
#include <string>



namespace protocols {
namespace topology_broker {

class DensityScoringClaimer : public TopologyClaimer {
public:
	DensityScoringClaimer(); //for factory

	DensityScoringClaimerOP shared_from_this() { return utility::pointer::dynamic_pointer_cast<DensityScoringClaimer>( TopologyClaimer::shared_from_this() ); }

	TopologyClaimerOP clone() const override;

	///mainly calls parent function... but is also used to figure out what residue number we are jumping to.
	//virtual void initialize_residues( core::pose::Pose&, claims::SequenceClaimOP init_claim, claims::DofClaims& failed_to_init );

	void generate_sequence_claims( claims::DofClaims& ) override;

	void generate_claims( protocols::topology_broker::claims::DofClaims& dc) override;

	/// @brief type() is specifying the output name of the TopologyClaimer
	std::string type() const override {
		return _static_type_name();
	}

	static std::string _static_type_name() {
		return "DensityScoringClaimer";
	}

	void add_constraints( core::pose::Pose& /*pose*/ ) const override;

	bool accept_declined_claim( claims::DofClaim const& was_declined ) override;
protected:

	void set_defaults() override;
	bool read_tag( std::string tag, std::istream& ) override;
	void init_after_reading() override;

private:
	core::Size  anchor_residue_;  // where does the VRT jump to
	std::string anchor_chain_;    // a SequenceLabel  << fpd ... is this used??
}; //class MetalloClaimer

}
}

#endif
