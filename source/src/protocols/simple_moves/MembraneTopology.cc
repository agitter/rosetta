// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

// Unit headers
#include <protocols/simple_moves/MembraneTopology.hh>
#include <protocols/simple_moves/MembraneTopologyCreator.hh>
#include <core/scoring/MembraneTopology.hh>
#include <core/pose/datacache/CacheableDataType.hh>
#include <basic/datacache/BasicDataCache.hh>

// Project Headers
#include <core/pose/Pose.hh>
//parsing
#include <utility/tag/Tag.hh>
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

// Utility Headers

// Unit Headers

// C++ headers

namespace protocols {
namespace simple_moves {

using namespace std;


static basic::Tracer TR( "protocols.simple_moves.MembraneTopology" );




MembraneTopology::~MembraneTopology() = default;

/// @brief default ctor
MembraneTopology::MembraneTopology() :
	parent(),
	span_file_("")
{}

void MembraneTopology::apply( Pose & pose ) {
	core::scoring::MembraneTopologyOP membrane_topology( new core::scoring::MembraneTopology );
	membrane_topology->initialize( span_file_ );
	pose.data().set( core::pose::datacache::CacheableDataType::MEMBRANE_TOPOLOGY, membrane_topology );
	TR<<"Setting pose's membrane topology according to span file "<<span_file()<<std::endl;
}


void MembraneTopology::parse_my_tag( utility::tag::TagCOP tag,
	basic::datacache::DataMap &
)
{

	span_file( tag->getOption< std::string >( "span_file" ) );
	TR<<"Span file defined as "<<span_file()<<std::endl;
}

std::string MembraneTopology::get_name() const {
	return mover_name();
}

std::string MembraneTopology::mover_name() {
	return "MembraneTopology";
}

void MembraneTopology::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		+ XMLSchemaAttribute::required_attribute(
		"span_file", xs_string,
		"path to transmembrane topology prediction file" );
	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"Inserts membrane topology from a membrane span file into a pose",
		attlist );
}

std::string MembraneTopologyCreator::keyname() const {
	return MembraneTopology::mover_name();
}

protocols::moves::MoverOP
MembraneTopologyCreator::create_mover() const {
	return utility::pointer::make_shared< MembraneTopology >();
}

void MembraneTopologyCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	MembraneTopology::provide_xml_schema( xsd );
}



} // moves
} // protocols
