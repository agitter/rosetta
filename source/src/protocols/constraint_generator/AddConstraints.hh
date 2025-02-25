// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/constraint_generator/AddConstraints.hh
/// @brief Adds constraints generated by ConstraintGenerators to a pose
/// @author Tom Linsky (tlinsky@uw.edu)

#ifndef INCLUDED_protocols_constraint_generator_AddConstraints_hh
#define INCLUDED_protocols_constraint_generator_AddConstraints_hh

// Unit headers
#include <protocols/constraint_generator/AddConstraints.fwd.hh>
#include <protocols/moves/Mover.hh>

// Protocol headers
#include <protocols/constraint_generator/ConstraintGenerator.fwd.hh>

// Core headers
#include <core/pose/Pose.fwd.hh>

// Basic/Utility headers
#include <basic/datacache/DataMap.fwd.hh>

namespace protocols {
namespace constraint_generator {

///@brief Adds constraints generated by ConstraintGenerators to a pose
class AddConstraints : public protocols::moves::Mover {

public:
	AddConstraints();
	AddConstraints( ConstraintGeneratorCOPs const & generators );

	// destructor (important for properly forward-declaring smart-pointer members)
	~AddConstraints() override;


	void
	apply( core::pose::Pose & pose ) override;

public:

	/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
	void
	parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	fresh_instance() const override;

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	clone() const override;

public:
	void
	add_generator( ConstraintGeneratorCOP cst_generator );

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

	/// @brief Provide citations to the passed CitationCollectionList.
	/// This allows this mover to provide citations for itself
	/// and for any modules that it invokes.
	/// @details This override cites Tom Linsky, who created the constraint generator framework.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void provide_citation_info(basic::citation_manager::CitationCollectionList & citations) const override;

private:
	ConstraintGeneratorCOPs generators_;

};

} //protocols
} //constraint_generator

#endif //protocols/constraint_generator_AddConstraints_hh
