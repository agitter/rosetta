// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/rosetta_scripts/RosettaScriptsParser.cc
/// @brief  August 2008 job distributor as planned at RosettaCon08 - Interface base class Parser
/// @author Sarel Fleishman sarelf@u.washington.edu
/// @author Luki Goldschmidt lugo@uw.edu
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com). JD3/PyRosetta compatability, removal of JD2 code, general updates and docs.

// Unit Headers
#include <protocols/rosetta_scripts/RosettaScriptsParser.hh>

// Package headers
#include <protocols/filters/Filter.fwd.hh>
#include <protocols/filters/FilterFactory.hh>
#include <protocols/filters/BasicFilters.hh>
#include <protocols/parser/DataLoader.hh>
#include <protocols/parser/DataLoaderFactory.hh>
#include <protocols/rosetta_scripts/util.hh>
#include <protocols/rosetta_scripts/ParsedProtocol.hh>
#include <protocols/rosetta_scripts/XmlObjects.hh>
#include <protocols/rosetta_scripts/RosettaScriptsSchemaValidator.hh>
#include <protocols/jd2/util.hh>

// Project headers
#include <core/pack/palette/PackerPalette.hh>
#include <core/pack/palette/PackerPaletteFactory.hh>
#include <core/pack/task/operation/TaskOperation.hh>
#include <core/pack/task/operation/TaskOperationFactory.hh>
#include <core/pose/Pose.hh>
#include <core/pose/metrics/CalculatorFactory.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/import_pose/import_pose.hh>
#include <core/types.hh>

// Basic headers
#include <basic/options/option.hh>
#include <basic/resource_manager/ResourceManager.hh>
#include <basic/datacache/DataMapObj.hh>

// Utility Headers
#include <utility/tag/Tag.hh>
#include <utility/tag/XMLSchemaGeneration.hh>
#include <utility/tag/XMLSchemaValidation.hh>
#include <utility/pointer/memory.hh>
#include <utility/VBWrapper.hh>

// ObjexxFCL Headers

// movers
#include <protocols/moves/Mover.fwd.hh>
#include <protocols/moves/MoverFactory.hh>
#include <protocols/moves/NullMover.hh>
#include <protocols/moves/mover_schemas.hh>

// Pose Metric Calculators for filters
#include <core/pose/metrics/simple_calculators/InterfaceSasaDefinitionCalculator.hh>
#include <core/pose/metrics/simple_calculators/InterfaceDeltaEnergeticsCalculator.hh>
#include <core/pose/metrics/simple_calculators/InterfaceNeighborDefinitionCalculator.hh>
#include <core/pose/metrics/simple_calculators/SasaCalculatorLegacy.hh>

// Utility headers
#include <basic/Tracer.hh>
#include <basic/options/keys/corrections.OptionKeys.gen.hh>
#include <basic/options/keys/mistakes.OptionKeys.gen.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <utility/options/keys/OptionKeyList.hh>
#include <utility/options/OptionCollection.hh>

#include <utility/io/izstream.hh>
#include <utility/vector0.hh>
#include <utility/vector1.hh>

// C++ headers
#include <sstream>
#include <fstream>
#include <map>
#include <set>

// option key includes
#include <basic/options/keys/parser.OptionKeys.gen.hh>
#include <basic/datacache/DataMap.hh>

namespace protocols {
namespace rosetta_scripts {

using namespace core;
using namespace core::pose;
using namespace basic::options;
using namespace scoring;
using namespace moves;

static basic::Tracer TR( "protocols.rosetta_scripts.RosettaScriptsParser" );

RosettaScriptsParser::RosettaScriptsParser()
{
	set_recursion_limit( basic::options::option );
	register_factory_prototypes();
}


RosettaScriptsParser::~RosettaScriptsParser()= default;

using utility::tag::TagOP;
using utility::tag::TagCOP;
using utility::tag::TagCAP;

using TagCOPs = utility::vector0<TagCOP>;

void
RosettaScriptsParser::list_options_read( utility::options::OptionKeyList & read_options ){

	read_options
		+ OptionKeys::parser::protocol
		+ OptionKeys::parser::script_vars
		+ OptionKeys::parser::inclusion_recursion_limit
		+ OptionKeys::corrections::restore_talaris_behavior
		+ OptionKeys::mistakes::restore_pre_talaris_2013_behavior
		+ OptionKeys::in::file::native;

	core::scoring::list_read_options_in_get_score_function( read_options );

}

void
RosettaScriptsParser::set_recursion_limit(const utility::options::OptionCollection &options) {
	recursion_limit_ = options[ OptionKeys::parser::inclusion_recursion_limit ].value();
}


ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	std::string const & xml_fname,
	std::string const & current_input_name,
	std::string const & current_output_name
){
	TagCOP tag = create_tag_from_xml( xml_fname, basic::options::option );
	ParsedProtocolOP in_mover = generate_mover_for_protocol( tag, basic::options::option, current_input_name, current_output_name );
	return in_mover;
}


ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	std::string const & xml_fname,
	utility::vector1< std::string > const & script_vars,
	std::string const & current_input_name,
	std::string const & current_output_name
){
	TagCOP tag = create_tag_from_xml( xml_fname, script_vars);
	ParsedProtocolOP in_mover = generate_mover_for_protocol( tag, basic::options::option, current_input_name, current_output_name );
	return in_mover;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	utility::options::OptionCollectionCOP options,
	std::string const & current_input_name,
	std::string const & current_output_name,
	basic::resource_manager::ResourceManagerOP resource_manager
){
	local_options_ = options;
	return generate_mover(*options,
		current_input_name, current_output_name, resource_manager);
}

ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	utility::options::OptionCollectionCOP options,
	std::string const & xml_fname,
	std::string const & current_input_name,
	std::string const & current_output_name,
	XmlObjectsOP xml_objects,
	basic::resource_manager::ResourceManagerOP resource_manager
) {
	local_options_ = options;
	return generate_mover(*options,
		xml_fname,current_input_name, current_output_name, xml_objects, resource_manager);
}

ParsedProtocolOP
RosettaScriptsParser::generate_mover_xml_string(
	utility::options::OptionCollectionCOP options,
	std::string const & xml_text,
	std::string const & current_input_name,
	std::string const & current_output_name,
	XmlObjectsOP xml_objects /* = nullptr */
) {
	local_options_ = options;
	return generate_mover_xml_string( *options, xml_text, current_input_name, current_output_name, xml_objects );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	utility::options::OptionCollection const & options,
	std::string const & current_input_name,
	std::string const & current_output_name,
	basic::resource_manager::ResourceManagerOP resource_manager
){
	debug_assert(options[ OptionKeys::parser::protocol ].user());

	return generate_mover( options, options[ OptionKeys::parser::protocol ](), current_input_name, current_output_name, nullptr, resource_manager );
}

ParsedProtocolOP
RosettaScriptsParser::generate_mover(
	utility::options::OptionCollection const & options,
	std::string const & xml_fname,
	std::string const & current_input_name,
	std::string const & current_output_name,
	XmlObjectsOP xml_objects,
	basic::resource_manager::ResourceManagerOP resource_manager
) {
	TagCOP tag = create_tag_from_xml( xml_fname, options);
	ParsedProtocolOP in_mover = generate_mover_for_protocol( tag, options, current_input_name, current_output_name, xml_objects, resource_manager );
	return in_mover;
}


ParsedProtocolOP
RosettaScriptsParser::generate_mover_xml_string(
	utility::options::OptionCollection const & options,
	std::string const & xml_text,
	std::string const & current_input_name,
	std::string const & current_output_name,
	XmlObjectsOP xml_objects /* = nullptr */
) {

	TagCOP tag = create_tag_from_xml_string( xml_text, options);
	ParsedProtocolOP in_mover = generate_mover_for_protocol( tag, options, current_input_name, current_output_name, xml_objects );
	return in_mover;
}

TagCOP
RosettaScriptsParser::create_tag_from_xml( std::string const & xml_fname, utility::vector1< std::string > const & script_vars ){

	// A list of all of the files that have been included, which is checked whenever a
	// new file is included to prevent circular dependencies.
	utility::vector1 < std::string > filenames_encountered;
	// The input string.  Input files are opened and closed by read_in_and_recursively_replace_includes().
	// After calling this function, fin will contain the XML with includes replaced by whatever XML they include.
	// No interpretation is done yet (except for recognizing xi:include).
	std::string substituted_contents;
	read_in_and_recursively_replace_includes( xml_fname, substituted_contents, filenames_encountered, 0 );
	return finish_creating_tag( xml_fname, script_vars, substituted_contents );
}

TagCOP
RosettaScriptsParser::create_tag_from_xml_string(
	std::string const & xml_text,
	utility::vector1< std::string > const & script_vars
)
{
	std::string xml_fname = "XmlString (not real file)";
	// A list of all of the files that have been included, which is checked whenever a
	// new file is included to prevent circular dependencies.
	utility::vector1 < std::string > filenames_encountered;
	// The input string.  Input files are opened and closed by read_in_and_recursively_replace_includes().
	// After calling this function, fin will contain the XML with includes replaced by whatever XML they include.
	// No interpretation is done yet (except for recognizing xi:include).
	std::string substituted_contents;
	read_in_and_recursively_replace_includes( xml_fname, substituted_contents, filenames_encountered, 0, false, xml_text );
	return finish_creating_tag( xml_fname, script_vars, substituted_contents );
}

// private method
/// @brief To be called after the full xml tree has been loaded into a string from create_tag_from_xml*
/// @details xml_fname only used for tracer output.
TagCOP
RosettaScriptsParser::finish_creating_tag(
	std::string const & xml_fname,
	utility::vector1< std::string > const & script_vars,
	std::string const & substituted_contents
) const {

	std::stringstream fin( substituted_contents );

	// Do substitution of the script_vars in the stream
	if ( script_vars.size() > 0 ) {
		std::stringstream fin_sub;
		substitute_variables_in_stream(fin, script_vars, fin_sub);
		fin.clear();
		fin.str( fin_sub.str() );
	}

	// Validate the input script against the XSD for RosettaScripts, if it hasn't yet been validated.
	validate_input_script_against_xsd( xml_fname, fin );

	TagCOP tag = utility::tag::Tag::create(fin);

	//fin.close();
	TR << "Parsed script:" << "\n";
	TR << tag;
	TR.flush();
	runtime_assert( tag->getName() == "dock_design" || tag->getName() == "ROSETTASCRIPTS" );

	if ( tag->getName() == "dock_design" ) {
		TR << "<dock_design> tag is deprecated; please use <ROSETTASCRIPTS> instead." << std::endl;
	}
	return tag;
}

///@brief Create a tag from an XML string.  Read from a local options collection.
TagCOP
RosettaScriptsParser::create_tag_from_xml_string( std::string const & xml_text, utility::options::OptionCollection const & options )
{
	// Do substitution of the script_vars in the stream
	if ( options[ OptionKeys::parser::script_vars ].user() ) {
		return create_tag_from_xml_string(xml_text, options[ OptionKeys::parser::script_vars ]());
	} else {
		utility::vector1< std::string > empty_vector;
		return create_tag_from_xml_string(xml_text, empty_vector);
	}
}

///@brief Create a tag from an XML file.  Read from a local options collection.
TagCOP
RosettaScriptsParser::create_tag_from_xml( std::string const & xml_fname, utility::options::OptionCollection const & options )
{
	// Do substitution of the script_vars in the stream
	if ( options[ OptionKeys::parser::script_vars ].user() ) {
		return create_tag_from_xml(xml_fname, options[ OptionKeys::parser::script_vars ]());
	} else {
		utility::vector1< std::string > empty_vector;
		return create_tag_from_xml(xml_fname, empty_vector);
	}
}

void
RosettaScriptsParser::initialize_data_map(
	basic::datacache::DataMap & data,
	utility::options::OptionCollection const & options
) {

	if ( options[ OptionKeys::in::file::native ].user() ) {
		load_native( options[ OptionKeys::in::file::native ].value() );
	}

	////////////////////////////////////////////
	///// Set up some defaults in the data maps

	// default scorefxns
	ScoreFunctionOP commandline_sfxn  = core::scoring::get_score_function(options);
	data.add( "scorefxns", "commandline", commandline_sfxn );
	// Only add the default scorefunctions if compatible options have been provided.
	if ( options[ basic::options::OptionKeys::corrections::restore_talaris_behavior ]() ) {
		data.add( "scorefxns", "talaris2014", ScoreFunctionFactory::create_score_function(TALARIS_2014) );
		data.add( "scorefxns", "talaris2013", ScoreFunctionFactory::create_score_function(TALARIS_2013) );
	} else if ( options[ basic::options::OptionKeys::mistakes::restore_pre_talaris_2013_behavior ]() ) {
		data.add( "scorefxns", "score12", ScoreFunctionFactory::create_score_function( PRE_TALARIS_2013_STANDARD_WTS, SCORE12_PATCH ) );
		data.add( "scorefxns", "score_docking", ScoreFunctionFactory::create_score_function( PRE_TALARIS_2013_STANDARD_WTS, DOCK_PATCH ) );
	} else {
		data.add( "scorefxns", "REF2015", ScoreFunctionFactory::create_score_function(REF_2015)  );
	}
	data.add( "scorefxns", "soft_rep", ScoreFunctionFactory::create_score_function( SOFT_REP_DESIGN_WTS ) );
	data.add( "scorefxns", "score_docking_low", ScoreFunctionFactory::create_score_function( "interchain_cen" ) );
	data.add( "scorefxns", "score4L", ScoreFunctionFactory::create_score_function( "cen_std", "score4L" ) );
	//default scorefxns end

	// default filters
	protocols::filters::FilterOP true_filter( utility::pointer::make_shared< protocols::filters::TrueFilter >() );
	protocols::filters::FilterOP false_filter( utility::pointer::make_shared< protocols::filters::FalseFilter >() );
	data.add( "filters", "true_filter", true_filter );
	data.add( "filters", "false_filter", false_filter );

	// default movers
	MoverOP null_mover( utility::pointer::make_shared< protocols::moves::NullMover >() );
	data.add( "movers", "null", null_mover );

	//If native is set, add it to the datamap as a resource instead of loading in each individual mover.  Works in JD3 as well to allow better benchmarking.
	if ( native_pose_ ) {
		data.add_resource("native_pose", native_pose_);
	}

	//Give a COP of the OptionCollection to the datamap to begin using local options with RosettaScripts.
	if ( local_options_ ) {
		data.add_resource("options", local_options_);
	}

	// Store the nstruct index for this job if we're using JD2
	if ( jd2::jd2_used() ) {
		Size nstruct_ind = jd2::current_nstruct_index();
		TR.Debug << "Setting nstruct_index JobLabel as " << nstruct_ind << std::endl;
		data.add(
			"JobLabels",
			"nstruct_index",
			std::make_shared<utility::VBWrapper<core::Size>>(nstruct_ind)
		);
	}
}


/// @details Uses the Tag interface to the xml reader library in boost to parse
/// an xml file that contains design protocol information. A sample protocol
/// file can be found in src/pilot/apps/sarel/dock_design.protocol.
///
/// SCOREFXNS provides a way to define scorefunctions as they are defined in the
/// rosetta database, using the weights/patch convenctions. Several default
/// scorefunctions are preset and can be used without defining them explicitly.
///
/// FILTERS defines a set of filters that can be used together with the
/// dockdesign movers to prune out poses that don't meet certain criteria
///
/// MOVERS defines the movers that will be used
///
/// PROTOCOLS is the only order-sensitive section where subsequent movers and
/// filters are expected to be defined. These movers and filters were defined
/// in the previous two sections. The order in which the protocols are specified
/// by the user will be maintained by the DockDesign mover.
///
/// APPLY_TO_POSE -- DEPRECATED/Removed. This section used to allow for applying certain
/// movers to the pose prior to protocol start. This functionality has been removed.
/// Anything other than an empty APPLY_TO_POSE section will raise an error.
///
/// OUTPUT is a section which allows the XML control of how things are output.
/// Notice that the order of the sections by which the protocol is written
/// doesn't matter, BUT the order of the mover-filter pairs in PROTOCOLS section
/// does matter.
///
/// xi:include statements can be placed anywhere to trigger read-in of another
/// RosettaScripts XML file.  Contents of the file replace the xi:include
/// statement.
void
RosettaScriptsParser::generate_mover(
	moves::MoverOP & in_mover,
	bool new_input,
	std::string const & xml_fname,
	std::string const & current_input_name,
	std::string const & current_output_name,
	bool guarantee_new_mover
)
{

	if ( !new_input && !guarantee_new_mover ) return;

	std::string const dock_design_filename( xml_fname == "" ? option[ OptionKeys::parser::protocol ] : xml_fname );
	TR << "dock_design_filename=" << dock_design_filename << std::endl;

	TagCOP tag = create_tag_from_xml( dock_design_filename, basic::options::option);

	in_mover =  generate_mover_for_protocol( tag, basic::options::option, current_input_name, current_output_name );

}

ParsedProtocolOP
RosettaScriptsParser::generate_mover_for_protocol(
	TagCOP tag,
	utility::options::OptionCollection const & options,
	std::string const & current_input_name /* = "" */,
	std::string const & current_output_name /* = "" */,
	XmlObjectsOP xml_objects,
	basic::resource_manager::ResourceManagerOP resource_manager
) {
	basic::datacache::DataMap data; // abstract objects, such as scorefunctions, to be used by filter and movers
	return generate_mover_for_protocol(tag, options, data, current_input_name,
		current_output_name, xml_objects, resource_manager);
}

ParsedProtocolOP
RosettaScriptsParser::generate_mover_for_protocol(
	TagCOP tag,
	utility::options::OptionCollection const & options,
	basic::datacache::DataMap & data,
	std::string const & current_input_name /* = "" */,
	std::string const & current_output_name /* = "" */,
	XmlObjectsOP xml_objects,
	basic::resource_manager::ResourceManagerOP resource_manager
) {
	initialize_data_map( data, options );

	// Set the names for this job -- the input names and the output names
	{
		using StringWrapper = basic::datacache::DataMapObj< std::string >;
		if ( current_input_name != "" ) {
			auto input_name = utility::pointer::make_shared< StringWrapper >();
			input_name->obj = current_input_name;
			data[ "strings" ][ "current_input_name" ] = input_name;
		}
		if ( current_output_name != "" ) {
			auto output_name = utility::pointer::make_shared< StringWrapper >();
			output_name->obj = current_output_name;
			data[ "strings" ][ "current_output_name" ] = output_name;
		}
	}

	return generate_mover_for_protocol(tag, data, options, xml_objects, resource_manager);
}

ParsedProtocolOP
RosettaScriptsParser::generate_mover_for_protocol(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & data,
	utility::options::OptionCollection const &,
	XmlObjectsOP xml_objects,
	basic::resource_manager::ResourceManagerOP resource_manager
) {

	if ( !tag->hasTag("PROTOCOLS") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "parser::protocol file must specify PROTOCOLS section");
	}

	///////////////////////////////////////////////////////////////////
	// Go through the subtags, creating/loading the associated objects
	// Most subtags should be handled by their respective Data loaders.
	// Some subtags are handled specially for technical/historical reasons.
	//
	// Data Loaders -- each data loader handles one of the top-level blocks of a
	// rosetta script.  These blocks are handled by the RosettaScriptsParser itself;
	// other data loaders may be registered with the DataLoaderFactory at load time
	// so that arbitrary data, living in any library, can be loaded into the DataMap
	// through the XML interface.
	//
	for ( TagCOP curr_tag : tag->getTags() ) {
		if ( curr_tag->getName() == "PROTOCOLS" || curr_tag->getName() == "OUTPUT" ) {
			continue; // We'll deal with the PROTOCOLS & OUTPUT section later.
		} else if ( curr_tag->getName() == "RESOURCES" ) {
			parse_resources_tag( curr_tag, data, resource_manager);
		} else if ( curr_tag->getName() == "FILTERS" ) {
			for ( TagCOP tag_ptr : curr_tag->getTags() ) {
				instantiate_filter(tag_ptr, data);
			}
		} else if ( curr_tag->getName() == "MOVERS" ) {
			for ( TagCOP tag_ptr : curr_tag->getTags() ) {
				instantiate_mover(tag_ptr, data);
			}
		} else if ( curr_tag->getName() == "APPLY_TO_POSE" ) {
			parse_apply_to_pose_tag( curr_tag, data );
		} else if ( curr_tag->getName() == "IMPORT" ) {
			parse_import_tag( curr_tag, data );
		} else {
			// All other tags are considered DataLoader tags.
			using namespace protocols::parser;
			DataLoaderOP loader = DataLoaderFactory::get_instance()->newDataLoader( curr_tag->getName() );
			loader->load_data( curr_tag, data );
		}
		TR.flush();
	}

	////// Setup the actual main protocol block.
	TagCOP const protocols_tag( tag->getTag("PROTOCOLS") );
	protocols::rosetta_scripts::ParsedProtocolOP protocol( utility::pointer::make_shared< protocols::rosetta_scripts::ParsedProtocol >() );
	protocol->parse_my_tag( protocols_tag, data );
	if ( native_pose_ != nullptr ) {
		protocol->set_native_pose( native_pose_ );
	}
	TR.flush();

	////// Set Output options
	if ( tag->hasTag("OUTPUT") ) {
		TagCOP const output_tag( tag->getTag("OUTPUT") );
		protocol->final_scorefxn( rosetta_scripts::parse_score_function( output_tag, data, "commandline" ) );
	}

	///// Finish up
	tag->die_for_unaccessed_options_recursively();

	if ( xml_objects ) {
		xml_objects->init_from_maps( data );
	}

	return protocol;
}

/// @brief Actually read in the XML file.  Called recursively to read in XML files that
/// this XML file includes.  At the end of this operation, substituted_contents contains the contents
/// of the XML file, with all xi:includes replaced with the contents of included XML
/// files.  Files are opened and closed here.
///
/// @details Note that filenames_encountered is passed by copy rather than by reference
/// DELIBERATELY.  This is so that it remains a list of parent files, so that only
/// circular dependencies (attempts to include one's own parent, grandparent, etc.) are
/// detected.
/// If xml_text_for_top_level is set the filename passed will not be read and instead
/// xml_text_for_top_level will be used as though it was the contents of the first file.
/// @author Vikram K. Mulligan (vmullig@uw.edu)
/// @author Rocco Moretti (rmorettiase@gmail.com)
void
RosettaScriptsParser::read_in_and_recursively_replace_includes(
	std::string const &filename,
	std::string & substituted_contents, // "output" variable
	utility::vector1 < std::string > filenames_encountered,
	core::Size const recursion_level,
	bool const do_not_recurse,
	std::string const & xml_text_for_top_level /* = "" */
) const {

	//Check whether we've already encountered this filename
	for ( std::string const & seen_fn : filenames_encountered ) {
		if ( seen_fn == filename ) {
			throw CREATE_EXCEPTION(utility::excn::BadInput,  "Error in protocols::rosetta_scipts::RosettaScriptsParser::read_in_and_recursively_replace_includes(): Circular inclusion pattern detected when reading \"" + filename + "\"." );
		}
	}
	if ( filenames_encountered.size() != 0 && TR.visible() ) TR << "Including RosettaScripts XML file " << filename << "." << std::endl;
	filenames_encountered.push_back(filename);

	std::string contents;
	if ( xml_text_for_top_level.length() != 0 ) {
		contents = xml_text_for_top_level;
	} else {
		//Actually read in the file
		utility::io::izstream inputstream( filename );
		if ( !inputstream.good() ) {
			utility_exit_with_message("Unable to open RosettaScripts XML file: \"" + filename + "\".");
		}
		utility::slurp( inputstream, contents );
		inputstream.close();
	}


	std::string::const_iterator copy_start( contents.begin() ); // Where to start the next copy from

	if ( ! do_not_recurse || recursion_level < recursion_limit_ ) {
		// Find xi:include tags:
		// Will be an innermost set of <> brackets.
		std::locale loc;
		std::string::const_iterator start_bracket( contents.begin() ); // The position of '<', or where to start the search from

		while ( (start_bracket = std::find( start_bracket, contents.cend(), '<') ) != contents.cend() ) {
			std::string::const_iterator end_bracket = start_bracket + 1; // Will be the postion of corresponding '>'
			// Advance to next non-whitespace portion
			while ( end_bracket != contents.cend() && std::isspace( *end_bracket, loc ) ) { ++end_bracket; }
			if ( end_bracket == contents.cend() || std::string( end_bracket, end_bracket+10 ) != "xi:include" ) {
				// This tag is not an "xi:include" tag
				start_bracket = end_bracket;
				continue;
			}
			end_bracket = std::find( end_bracket, contents.cend(), '>' );
			if ( end_bracket == contents.end() ) { break; } // End of file, with unmatched open

			TagCOP tag( utility::tag::Tag::create( std::string( start_bracket, end_bracket+1 ) ) ); //Parse the current inner tag.
			if ( tag->getName() != "xi:include" ) {
				// This tag is not an "xi:include" tag, for some reason
				++start_bracket; // We might have skipped an alternative '<' when searching for the '>', so restart closer to the last place.
				continue;
			}
			runtime_assert_string_msg( tag->hasOption("href"), "Error in protocols::rosetta_scipts::RosettaScriptsParser::read_in_and_recursively_replace_includes(): An \"xi:include\" tag must include an \"href=...\" statement." );
			runtime_assert_string_msg( !tag->hasOption("parse") || tag->getOption<std::string>("parse") == "XML", "Error in protocols::rosetta_scipts::RosettaScriptsParser::read_in_and_recursively_replace_includes(): An \"xi:include\" tag can ONLY be used to include XML for parsing.  Other parse types are unsupporrted in RosettaScripts." );
			std::string const & subfilename( tag->getOption<std::string>("href") );
			bool prevent_recursion_next_time( tag->getOption< bool >( "prevent_recursion", false ) );

			substituted_contents.append( copy_start, start_bracket );
			read_in_and_recursively_replace_includes( subfilename, substituted_contents, filenames_encountered, recursion_level + 1, prevent_recursion_next_time );  //Recursively call this function to read in included XML.
			copy_start = end_bracket + 1;
			start_bracket = end_bracket + 1; // Restart parsing after end of tag.
		}
	}

	// Copy over the remaining portion of the file contents.
	substituted_contents.append( copy_start, contents.cend() );
}


ParsedProtocolOP
RosettaScriptsParser::parse_protocol_tag( TagCOP protocol_tag, utility::options::OptionCollection const & options)
{
	return generate_mover_for_protocol( protocol_tag, options);
}

void
RosettaScriptsParser::parse_resources_tag(
	utility::tag::TagCOP resources_tag,
	basic::datacache::DataMap & data,
	basic::resource_manager::ResourceManagerOP resource_manager ) const
{
	std::ostringstream resource_error_oss;

	for ( auto const & subtag : resources_tag->getTags() ) {
		debug_assert( subtag->getName() == "Resource" );
		std::string resource_name = subtag->getOption< std::string >( "name" );
		if ( ! resource_manager ) {
			resource_error_oss << "The requested resource \"" << resource_name << "\" cannot be retrieved because";
			resource_error_oss << " no ResourceManager has been provided. Perhaps you are not using rosetta_scripts_jd3?\n";
		} else if ( ! resource_manager->has_resource( resource_name ) ) {
			resource_error_oss << "The requested resource \"" << resource_name << "\" was not found in the";
			resource_error_oss << " ResourceManager. The following resources are available from the ResourceManager\n";
			for ( auto const & resname : resource_manager->resources_that_have_been_declared() ) {
				resource_error_oss << "   " << resname << "\n";
			}
		} else {
			TR << "Loading resource \"" << resource_name << "\" into the DataMap" << std::endl;
			data.add_resource( resource_name, resource_manager->get_resource( resource_name ) );
		}
	}

	// scope -- process possible errors from the RESOURCES block.
	std::string errstring = resource_error_oss.str();
	if ( ! errstring.empty() ) {
		throw CREATE_EXCEPTION( utility::excn::Exception, errstring );
	}
}

void
RosettaScriptsParser::parse_apply_to_pose_tag(
	utility::tag::TagCOP apply_tag,
	basic::datacache::DataMap &
) const
{
	// An empty APPLY_TO_POSE tag is fine (and common) - it's only when there's content in it that we have concerns.
	for ( TagCOP apply_tag_ptr : apply_tag->getTags() ) {
		TR.Error << "==================================================================================" << std::endl;
		TR.Error << std::endl;
		TR.Error << "The APPLY_TO_POSE section is no longer valid. Please edit your XML to remove it." <<  std::endl;
		TR.Error << std::endl;
		TR.Error << "You should be able to get effectively the same results by moving" <<  std::endl;
		TR.Error << "the definitions in the APPLY_TO_POSE section to the MOVERS section, adding a name," << std::endl;
		TR.Error << "and then calling those movers at the very start of the PROTOCOLS section." << std::endl;
		TR.Error << std::endl;
		TR.Error << "==================================================================================" << std::endl;

		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "The APPLY_TO_POSE section is no longer valid. Please remove it from your XML.");

	} // done apply_tag_ptr
}

/// @brief Instantiate a new filter and add it to the list of defined filters for this ROSETTASCRIPTS block
void
RosettaScriptsParser::instantiate_filter(
	TagCOP const & tag_ptr,
	basic::datacache::DataMap & data
) const {
	std::string const type( tag_ptr->getName() );
	if ( ! tag_ptr->hasOption("name") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Can't define unnamed Filter of type " + type);
	}

	std::string const user_defined_name( tag_ptr->getOption<std::string>("name") );
	if ( data.has("filters", user_defined_name ) ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Filter of name \"" + user_defined_name + "\" (of type " + type + ") already exists.");
	}

	protocols::filters::FilterOP new_filter( protocols::filters::FilterFactory::get_instance()->newFilter( tag_ptr, data ) );
	runtime_assert( new_filter != nullptr );
	data.add("filters", user_defined_name, new_filter );
	TR << "Defined filter named \"" << user_defined_name << "\" of type " << type << std::endl;
}

/// @brief Instantiate a new mover and add it to the list of defined movers for this ROSETTASCRIPTS block
void
RosettaScriptsParser::instantiate_mover(
	TagCOP const & tag_ptr,
	basic::datacache::DataMap & data
) const {
	std::string const type( tag_ptr->getName() );
	if ( ! tag_ptr->hasOption("name") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Can't define unnamed Mover of type " + type);
	}

	std::string const user_defined_name( tag_ptr->getOption<std::string>("name") );
	if ( data.has("movers", user_defined_name ) ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Mover of name \"" + user_defined_name + "\" (of type " + type + ") already exists.");
	}

	MoverOP new_mover( MoverFactory::get_instance()->newMover( tag_ptr, data ) );
	runtime_assert( new_mover != nullptr );
	data.add("movers", user_defined_name, new_mover );
	TR << "Defined mover named \"" << user_defined_name << "\" of type " << type << std::endl;
}

/// @brief Instantiate a PackerPalette object.
/// @author Vikram K. Mulligan (vmullig@uw.edu).
void RosettaScriptsParser::instantiate_packer_palette(
	TagCOP const & tag_ptr,
	basic::datacache::DataMap & data
) const {
	using namespace core::pack::palette;

	std::string const type( tag_ptr->getName() );
	if ( ! tag_ptr->hasOption("name") ) {
		throw CREATE_EXCEPTION( utility::excn::RosettaScriptsOptionError, "Can't define unnamed PackerPalette of type " + type + "." );
	}

	std::string const user_defined_name( tag_ptr->getOption<std::string>("name") );
	if ( data.has( "packer_palette", user_defined_name ) ) {
		throw CREATE_EXCEPTION( utility::excn::RosettaScriptsOptionError, "PackerPalette with name \"" + user_defined_name + "\" (of type " + type + ") already exists." );
	}

	PackerPaletteOP new_pp( PackerPaletteFactory::get_instance()->newPackerPalette( type, data, tag_ptr ) );
	runtime_assert( new_pp != nullptr );
	data.add("packer_palette", user_defined_name, new_pp );
	TR << "Defined PackerPalette named \"" << user_defined_name << "\" of type " << type << "." << std::endl;
}


/// @brief Instantiate a new task operation (used in IMPORT tag)
void
RosettaScriptsParser::instantiate_taskoperation(
	TagCOP const & tag_ptr,
	basic::datacache::DataMap & data
) const {
	using namespace core::pack::task::operation;

	std::string const type( tag_ptr->getName() );
	if ( ! tag_ptr->hasOption("name") ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Can't define unnamed TaskOperation of type " + type);
	}

	std::string const user_defined_name( tag_ptr->getOption<std::string>("name") );
	if ( data.has( "task_operations", user_defined_name ) ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "TaskOperation with name \"" + user_defined_name + "\" (of type " + type + ") already exists.");
	}

	TaskOperationOP new_t_o( TaskOperationFactory::get_instance()->newTaskOperation( type, data, tag_ptr ) );
	runtime_assert( new_t_o != nullptr );
	data.add("task_operations", user_defined_name, new_t_o );
	TR << "Defined TaskOperation named \"" << user_defined_name << "\" of type " << type << std::endl;
}

/// @brief Recursively find a specific tag by option name and valie in any ROSETTASCRIPTS tag that's a child of rootTag
TagCOP
RosettaScriptsParser::find_rosettascript_tag(
	TagCOP tag_in,
	const std::string & section_name,
	const std::string & option_name,
	const std::string & option_value
) {

	TagCAP tag_ap(tag_in);

	// Find the next higher ROSETTASCRIPTS block
	do {
		TagCOP tag = tag_ap.lock();
		tag_ap = tag ? tag->getParent() : TagCAP();
	} while(!tag_ap.expired() && tag_ap.lock()->getName() != "ROSETTASCRIPTS");

	TagCOP tag( tag_ap.lock() );
	if ( !tag ) {
		return nullptr;
	}

	// Look for section_name (MOVERS, FITLERS, ...) directly below ROSETTASCRIPTS
	if ( tag->hasTag(section_name) ) {
		TagCOP section_tag( tag->getTag(section_name) );
		if ( !option_name.length() ) {
			return section_tag;
		}
		for ( TagCOP child_tag : section_tag->getTags() ) {
			if ( child_tag->getOption<std::string>(option_name) == option_value ) {
				return child_tag;
			}
		}
	}

	// No match, walk up another level
	TagCOP tagParent( tag->getParent().lock() );
	if ( tagParent ) {
		return find_rosettascript_tag(tagParent, section_name, option_name, option_value);
	}

	return nullptr;
}

void
RosettaScriptsParser::parse_import_tag(
	utility::tag::TagCOP import_tag,
	basic::datacache::DataMap & data
) const {
	std::set< ImportTagName > import_tag_names;

	if ( import_tag->hasOption("packer_palettes") ) {
		// Import task operations
		std::string pp_str( import_tag->getOption<std::string>("packer_palettes") );
		std::istringstream pp_ss(pp_str);
		std::string pp_name;
		while ( std::getline(pp_ss, pp_name, ',') ) {
			import_tag_names.insert(std::make_pair("PACKER_PALETTES", pp_name));
		}
	}//fi packer_palettes

	if ( import_tag->hasOption("taskoperations") ) {
		// Import task operations
		std::string taskoperations_str( import_tag->getOption<std::string>("taskoperations") );
		std::istringstream taskoperation_ss(taskoperations_str);
		std::string taskoperation_name;
		while ( std::getline(taskoperation_ss, taskoperation_name, ',') ) {
			import_tag_names.insert(std::make_pair("TASKOPERATIONS", taskoperation_name));
		}
	}//fi taskoperations

	if ( import_tag->hasOption("filters") ) {
		// Import filters
		std::string filters_str( import_tag->getOption<std::string>("filters") );
		std::istringstream filters_ss(filters_str);
		std::string filter_name;
		while ( std::getline(filters_ss, filter_name, ',') ) {
			import_tag_names.insert(std::make_pair("FILTERS", filter_name));
		}
	}//fi filters

	if ( import_tag->hasOption("movers") ) {
		// Import movers
		std::string movers_str( import_tag->getOption<std::string>("movers") );
		std::istringstream movers_ss(movers_str);
		std::string mover_name;
		while ( std::getline(movers_ss, mover_name, ',') ) {
			import_tag_names.insert(std::make_pair("MOVERS", mover_name));
		}
	}//fi movers

	// Attempt to find and import requested objects; throws exception on failure
	if ( import_tag_names.size() > 0 ) {
		import_tags(import_tag_names, import_tag->getParent().lock(), data);
	}
}

/// @brief Import filters, movers, ... specified in the IMPORT tag
/// in the order they were originally defined elsewhere in the script
void
RosettaScriptsParser::import_tags(
	std::set< ImportTagName > & import_tag_names,
	utility::tag::TagCOP my_tag,
	basic::datacache::DataMap & data
) const {
	// Process all parent ROSETTASCRIPTS tags, one at a time
	TagCAP curr_level_tag_ap(my_tag);

	while ( !curr_level_tag_ap.expired() ) {

		// Find direct parent ROSETTASCRIPTS tag
		do {
			TagCOP curr_level_tag = curr_level_tag_ap.lock();
			curr_level_tag_ap = curr_level_tag ? curr_level_tag->getParent() : TagCAP();
		} while(!curr_level_tag_ap.expired() && curr_level_tag_ap.lock()->getName() != "ROSETTASCRIPTS");

		TagCOP curr_level_tag( curr_level_tag_ap.lock() );
		if ( !curr_level_tag ) {
			break; // No ROSETTASCRIPTS tag to import from
		}

		TR.Debug << "Importing from tag: " << curr_level_tag << std::endl;

		// Check what we'd like to import from it
		for ( TagCOP tag : curr_level_tag->getTags() ) {

			if ( tag->getName() == "PACKER_PALETTES" ) {
				for ( TagCOP packer_palette_tag : tag->getTags() ) {
					std::string packer_palette_name( packer_palette_tag->getOption<std::string>("name") );
					ImportTagName key( std::make_pair( tag->getName(), packer_palette_name ) );
					bool const need_import( import_tag_names.find( key ) != import_tag_names.end() );
					if ( need_import ) {
						instantiate_packer_palette(packer_palette_tag, data);
						import_tag_names.erase(key);
					}
				}
			}
			if ( tag->getName() == "TASKOPERATIONS" ) {
				for ( TagCOP taskoperation_tag : tag->getTags() ) {
					std::string taskoperation_name( taskoperation_tag->getOption<std::string>("name") );
					ImportTagName key( std::make_pair( tag->getName(), taskoperation_name ) );
					bool const need_import( import_tag_names.find( key ) != import_tag_names.end() );
					if ( need_import ) {
						instantiate_taskoperation(taskoperation_tag, data);
						import_tag_names.erase(key);
					}
				}
			}
			if ( tag->getName() == "FILTERS" ) {
				for ( TagCOP filter_tag : tag->getTags() ) {
					std::string filter_name( filter_tag->getOption<std::string>("name") );
					ImportTagName key( std::make_pair( tag->getName(), filter_name ) );
					bool const need_import( import_tag_names.find( key ) != import_tag_names.end() );
					if ( need_import ) {
						instantiate_filter(filter_tag, data);
						import_tag_names.erase(key);
					}
				}
			}

			if ( tag->getName() == "MOVERS" ) {
				for ( TagCOP mover_tag : tag->getTags() ) {
					std::string mover_name( mover_tag->getOption<std::string>("name") );
					ImportTagName key( std::make_pair( tag->getName(), mover_name ) );
					bool const need_import( import_tag_names.find( key ) != import_tag_names.end() );
					if ( need_import ) {
						TagCAP parent = my_tag->getParent();
						if ( utility::pointer::equal(parent, mover_tag) ) {
							// Current mover_tag is our parent, i.e. same ROSETTASCRIPTS tag
							throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Cannot import mover " + mover_name + " into itself; recursion detected");
						}
						instantiate_mover(mover_tag, data);
						import_tag_names.erase(key);
					}
				}
			}

		} //for curr_level_tag->getTags()

		if ( import_tag_names.size() <= 0 ) {
			break; // All done
		}

		// Go up the tree
		curr_level_tag_ap = curr_level_tag->getParent();

	} // while

	// Check if there are any remaining imports that could not be satisfied
	for ( ImportTagName import_tag : import_tag_names ) {
		std::string msg("Failed to import " + import_tag.second + " from " + import_tag.first);
		TR << msg << std::endl;
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, msg);
	}
}


/// @brief Create a variable substituted version of the input stream, given a StringVectorOption formated list of variables
///to substitiute. Each item in script_vars should be in the form of "variable=value", where "value" is the string to substitiute
///into the input stream whereever the string "%%variable%%" is found in the input.
///
///Having the return value be passed through a parameter is to get around some copy-constructor limitations of C++ streams.
void
RosettaScriptsParser::substitute_variables_in_stream( std::istream & instream, utility::vector1< std::string > const& script_vars, std::stringstream & out){
	using namespace std;
	//Parse variable substitutions
	map<string,string> var_map;
	for ( std::string const & s : script_vars ) {
		core::Size eq_pos(s.find('='));
		if ( eq_pos != string::npos ) { //ignore ones without a '='
			var_map[ s.substr(0,eq_pos) ] = s.substr(eq_pos+1);
		}
	}
	//Print parsing for confirmation
	TR << "Variable substitution will occur with the following values: ";
	for ( auto const & elem : var_map ) {
		TR << "'%%" << elem.first << "%%'='" << elem.second << "';  ";
	}
	TR << std::endl;
	var_map[""] = "%%"; //for %%%%->%% substitutions
	//Perform actual variable substitution
	TR << "Substituted script:" << "\n";
	string line;
	while ( getline( instream, line ) ) {
		core::Size pos, end, last(0);
		string outline; // empty string
		while ( ( pos = line.find("%%", last) ) != string::npos ) {
			end = line.find("%%", pos+2);
			if ( end == string::npos ) break; //Unmatched %% on line - keep as is.
			outline += line.substr( last, pos-last );
			last = end+2;
			string var( line.substr( pos+2, end-(pos+2) ) );
			if ( var_map.find( var ) != var_map.end() ) {
				outline += var_map[var];
			} else {
				outline += "%%" + var + "%%"; // Silently ignore missing values - will probably cause error later.
			}
		}
		outline += line.substr( last ); // Add the rest of the line, starting at last
		TR << outline << "\n";
		out << outline << "\n";
	}
	TR.flush();
}

/// @brief Factories avoid requiring compiler dependencies for all possible constructible derived classes,
///by allowing runtime registration of derived class prototypes. However, this requires
///pre-registration of a derived type with the factory prior to asking the factory to return an
///instance of that type. This method registers those additional derived classes that are available for
///construction in the RosettaScriptsParser context.
/// TO-DO: replace this manual factory registration system with a load-time-initialized singleton scheme (see r32404 for example)
void
RosettaScriptsParser::register_factory_prototypes()
{
	// note: TaskOperations are now registered with a singleton factory at load time using apl's creator/registrator scheme

	// also register some constraint types with the ConstraintFactory (global singleton class)
	// this allows derived non-core constraints to be constructed from string definitions in constraints files
	//using namespace core::scoring::constraints;
	//ConstraintFactory & cstf( ConstraintIO::get_cst_factory() );
	//cstf.add_type( new core::scoring::constraints::SequenceProfileConstraint(
	// core::Size(), utility::vector1< id::AtomID >(), NULL ) );

	// register calculators
	core::Size const chain1( 1 ), chain2( 2 );
	using namespace core::pose::metrics;

	if ( !CalculatorFactory::Instance().check_calculator_exists( "sasa_interface" ) ) {
		PoseMetricCalculatorOP int_sasa_calculator( utility::pointer::make_shared< core::pose::metrics::simple_calculators::InterfaceSasaDefinitionCalculator >( chain1, chain2 ) );
		CalculatorFactory::Instance().register_calculator( "sasa_interface", int_sasa_calculator );
	}
	if ( !CalculatorFactory::Instance().check_calculator_exists( "sasa" ) ) {
		PoseMetricCalculatorOP sasa_calculator( utility::pointer::make_shared< core::pose::metrics::simple_calculators::SasaCalculatorLegacy >() );
		CalculatorFactory::Instance().register_calculator( "sasa", sasa_calculator );
	}
	if ( !CalculatorFactory::Instance().check_calculator_exists( "ligneigh" ) ) {
		PoseMetricCalculatorOP lig_neighbor_calc( utility::pointer::make_shared< core::pose::metrics::simple_calculators::InterfaceNeighborDefinitionCalculator >( chain1, chain2 ) );
		CalculatorFactory::Instance().register_calculator( "ligneigh", lig_neighbor_calc );
	}
	if ( !CalculatorFactory::Instance().check_calculator_exists( "liginterfE" ) ) {
		PoseMetricCalculatorOP lig_interf_E_calc( utility::pointer::make_shared< core::pose::metrics::simple_calculators::InterfaceDeltaEnergeticsCalculator >( "ligneigh" ) );
		CalculatorFactory::Instance().register_calculator( "liginterfE", lig_interf_E_calc );
	}
}

/// @brief Is the current tag one with a slash at the end?
/// @details Starting from endpos, crawl backward until we hit a
/// '/' or a non-whitespace character.  Return true if and only
/// if it's a slash, false otherwise.
/// @author Vikram K. Mulligan (vmullig@uw.edu)
bool
RosettaScriptsParser::has_slash_at_end(
	std::string const &str,
	core::Size const endpos
) const {
	debug_assert( str.length() >=endpos );
	debug_assert( endpos > 0 );
	core::Size i(endpos);

	do {
		--i;
		if ( str[i] == '/' ) return true;
		else if ( str[i] != ' ' && str[i] != '\t' && str[i] != '\n' ) return false;
	} while(i!=0);

	return false;
}

/// @brief Is the current tag one with something that will be replaced by variable replacement?
/// @details Starting from endpos, crawl backward until we hit a '<' or '%'.  Return 'false' if
/// the former and 'true' if the latter.
/// @author Vikram K. Mulligan (vmullig@uw.edu)
bool
RosettaScriptsParser::has_double_percent_signs(
	std::string const &str,
	core::Size const endpos
) const {
	debug_assert( str.length() >=endpos );
	debug_assert( endpos > 0 );
	core::Size i(endpos);

	do {
		--i;
		if ( str[i] == '<' ) return false;
		else if ( str[i] == '%' && i>0 && str[i-1] == '%' ) return true;
	} while(i!=0);

	return false;

}

void
RosettaScriptsParser::load_native( std::string const & native ){
	TR << "Loading native resource as native_pose to the datamap " << std::endl;
	native_pose_ = core::import_pose::pose_from_file( native , core::import_pose::PDB_file);
}

void
RosettaScriptsParser::validate_input_script_against_xsd( std::string const & fname, std::stringstream const & input_xml ) const
{
	using namespace utility::tag;


	TR << "Validating input script..." << std::endl;

	std::string xml_file = input_xml.str();

	std::ostringstream oss;
	oss << "Input rosetta scripts XML file \"" << fname << "\" failed to validate against"
		" the rosetta scripts schema. Use the option -parser::output_schema <output filename> to output"
		" the schema to a file to see all valid options.\n";

	std::string const long_error_string("Your XML has failed validation.  The error message below will tell you where in your XML file the error occurred.  Here's how to fix it:\n\n1) If the validation fails on something obvious, like an illegal attribute due to a spelling error (perhaps you used scorefnction instead of scorefunction), then you need to fix your XML file.\n2) If you haven't run the XML rewriter script and this might be pre-2017 Rosetta XML, run the rewriter script (tools/xsd_xrw/rewrite_rosetta_script.py) on your input XML first.  The attribute values not being in quotes (scorefunction=talaris2014 instead of scorefunction=\"talaris2014\") is a good indicator that this is your problem.\n3) If you are a developer and neither 1 nor 2 worked - email the developer's mailing list or try Slack.\n4) If you are an academic or commercial user - try the Rosetta Forums https://www.rosettacommons.org/forum\n");

	oss << long_error_string << "\n\n";

	if ( basic::options::option[ basic::options::OptionKeys::parser::output_schema ].user() ) {
		std::string schema;
		try {
			schema = RosettaScriptsSchemaValidator::xsd_for_rosetta_scripts();
		} catch ( utility::excn::Exception const & e ) {
			std::ostringstream oss;
			oss << "If you are seeing this message, the internally-generated XML Schema"
				" for rosetta_scripts could not be properly generated\nThis failure occurred"
				" before your XML script that provided was examined. The error has been"
				" compiled into Rosetta and will need to be fixed by a developer.";
			oss << "An error was encountered while the string of the schema was being"
				" generated; this is before the schema is analyzed for whether it is"
				" correct or not.\n";
			oss << e.msg() << "\n";
			throw CREATE_EXCEPTION(utility::excn::Exception,  oss.str() );
		}

		std::ofstream ofs( basic::options::option[ basic::options::OptionKeys::parser::output_schema ] );
		ofs << schema << std::endl;
		exit( 0 );
	}


	XMLValidationOutput validator_output;
	try {
		validator_output = RosettaScriptsSchemaValidator::get_instance()->
			validator()->validate_xml_against_schema( xml_file );
	} catch ( utility::excn::Exception const & e ) {
		oss << e.msg() << "\n";
		throw CREATE_EXCEPTION(utility::excn::Exception,  oss.str() );
	}

	if ( ! validator_output.valid() ) {
		//I don't think this part actually works - these messages are always empty and there is a matching "does this work?" comment in XMLSchemaValidation.cc XRW TODO
		oss << "Error messages were:\n" << validator_output.error_messages();;
		oss << "------------------------------------------------------------\n";
		oss << "Warning messages were:\n" << validator_output.warning_messages();
		oss << "------------------------------------------------------------\n";
		throw CREATE_EXCEPTION(utility::excn::Exception,  oss.str() );
	}

	if ( basic::options::option[ basic::options::OptionKeys::parser::validate_and_exit ] ) {
		TR << "Validation succeeded. Exiting" << std::endl;
		exit( 0 );
	}

	TR << "...done" << std::endl;
}

std::string
RosettaScriptsParser::xsd_for_rosetta_scripts()
{
	return RosettaScriptsSchemaValidator::xsd_for_rosetta_scripts();
}

void
RosettaScriptsParser::write_ROSETTASCRIPTS_complex_type( utility::tag::XMLSchemaDefinition & xsd )
{
	RosettaScriptsSchemaValidator::write_ROSETTASCRIPTS_complex_type( xsd );
}

bool
RosettaScriptsParser::protocol_option_set(utility::options::OptionCollection const & options) const {
	return options[ OptionKeys::parser::protocol ].user();
}

std::string
RosettaScriptsParser::rosetta_scripts_element_name()
{
	return RosettaScriptsSchemaValidator::rosetta_scripts_element_name();
}

std::string
RosettaScriptsParser::rosetta_scripts_complex_type_naming_func( std::string const & element_name )
{
	return RosettaScriptsSchemaValidator::rosetta_scripts_complex_type_naming_func( element_name );
}

} // namespace rosetta_scripts
} // namespace protocols
