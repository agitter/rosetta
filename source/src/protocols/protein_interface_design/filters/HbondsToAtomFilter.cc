// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/protein_interface_design/filters/HbondsToAtomFilter.cc
/// @brief
/// @author Lei Shi (shileiustc @gmail.com)
#include <protocols/protein_interface_design/filters/HbondsToAtomFilter.hh>
#include <protocols/protein_interface_design/filters/HbondsToAtomFilterCreator.hh>


// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <basic/Tracer.hh>
#include <core/conformation/Conformation.hh>
#include <core/scoring/ScoreFunction.hh>
#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/pose/selection.hh>
//#include <protocols/moves/ResidueMover.hh>
//#include <protocols/simple_pose_metric_calculators/BuriedUnsatisfiedPolarsCalculator.hh>



//Objectxxxx header

// Utility Headers

// Unit Headers
#include <protocols/protein_interface_design/design_utils.hh>

// C++ headers

#include <ObjexxFCL/format.hh>

//Auto Headers
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>


using namespace core;
using namespace core::scoring;
using namespace ObjexxFCL::format;


namespace protocols {
namespace protein_interface_design {
namespace filters {

static basic::Tracer TR( "protocols.protein_interface_design.filters.HbondsToAtomFilter" );
using core::pose::Pose;



bool
HbondsToAtomFilter::apply( Pose const & pose ) const {
	core::Size hbonded_res( compute( pose ) );
	TR<<"found "<<hbonded_res<< " hbond to target residue " << resnum_ << " and name " << atomdesg_;
	if ( hbonded_res >= partners_ ) {
		TR << ". passing." << std::endl;
		return( true );
	} else {
		TR << ". failing." << std::endl;
		return( false );
	}
}

void
HbondsToAtomFilter::parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & )
{
	partners_ = tag->getOption<core::Size>( "partners" );
	energy_cutoff_ = tag->getOption<core::Real>( "energy_cutoff", -0.5 );
	bb_bb_ = tag->getOption<bool>( "bb_bb", false );
	backbone_ = tag->getOption<bool>( "backbone", false );
	sidechain_ = tag->getOption<bool>( "sidechain", true );
	resnum_ = core::pose::get_resnum_string( tag );

	if ( tag->hasOption( "atomname" ) ) {
		atomdesg_ = tag->getOption< std::string >( "atomname" );
	} else {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Need to set atomname");
	}

	TR<<"Hbonds to atom filter for resnum "<<resnum_<<" and name " << atomdesg_ <<" with "<<partners_<<" hbonding partners"<<std::endl;
}

void
HbondsToAtomFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	core::Size hbonded_res( compute( pose ) );

	out<<"Number of residues hbonded to "<<resnum_ << " and name " << atomdesg_ << " is " << hbonded_res <<'\n';
}

core::Real
HbondsToAtomFilter::report_sm( core::pose::Pose const & pose ) const {
	core::Size hbonded_res( compute( pose ) );
	return( hbonded_res );
}

core::Size
HbondsToAtomFilter::compute( Pose const & pose ) const {
	using core::Size;

	core::pose::Pose temp_pose( pose );
	core::scoring::ScoreFunctionOP scorefxn(get_score_function() );
	(*scorefxn)(temp_pose);
	/// Now handled automatically.  scorefxn->accumulate_residue_total_energies( temp_pose );

	core::Size const chain2begin( temp_pose.conformation().chain_begin( 2 ) );
	core::Size partner_begin, partner_end;
	core::Size resnum( core::pose::parse_resnum( resnum_, pose ) );
	if ( resnum >= chain2begin ) {
		partner_begin = 1; partner_end = chain2begin-1;
	} else {
		partner_begin = chain2begin; partner_end = temp_pose.size();
	}
	std::set<core::Size> binders;
	for ( core::Size i=partner_begin; i<=partner_end; ++i ) binders.insert( i );

	std::list< core::Size> hbonded_res( hbonded_atom ( temp_pose, resnum, atomdesg_,  binders, backbone_, sidechain_, energy_cutoff_, bb_bb_) );

	return( hbonded_res.size() );
}

HbondsToAtomFilter::~HbondsToAtomFilter() = default;

std::string HbondsToAtomFilter::name() const {
	return class_name();
}

std::string HbondsToAtomFilter::class_name() {
	return "HbondsToAtom";
}

void HbondsToAtomFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute( "partners", xsct_non_negative_integer, "H-bonding partner expectation, below which counts as failure" )
		+ XMLSchemaAttribute::attribute_w_default( "energy_cutoff", xsct_real, "Energy below which a H-bond counts", "-0.5" )
		+ XMLSchemaAttribute::attribute_w_default( "bb_bb", xsct_rosetta_bool, "Count backbone-backbone H-bonds", "0" )
		+ XMLSchemaAttribute::attribute_w_default( "backbone", xsct_rosetta_bool, "Count backbone H-bonds", "0" )
		+ XMLSchemaAttribute::attribute_w_default( "sidechain", xsct_rosetta_bool, "Count sidechain H-bonds", "1" )
		+ XMLSchemaAttribute( "pdb_num", xsct_refpose_enabled_residue_number, "Particular residue of interest" )
		+ XMLSchemaAttribute::required_attribute( "atomname", xs_string, "Atom name to which to examine H-bonds" );

	core::pose::attributes_for_get_resnum_string( attlist ) ;
	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "This filter counts the number of residues that form sufficiently energetically favorable H-bonds to a selected atom", attlist );
}

std::string HbondsToAtomFilterCreator::keyname() const {
	return HbondsToAtomFilter::class_name();
}

protocols::filters::FilterOP
HbondsToAtomFilterCreator::create_filter() const {
	return utility::pointer::make_shared< HbondsToAtomFilter >();
}

void HbondsToAtomFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	HbondsToAtomFilter::provide_xml_schema( xsd );
}


}
} // protein_interface_design
} // devel
