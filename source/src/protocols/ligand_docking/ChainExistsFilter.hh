// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/ligand_docking/ChainExistsFilter.hh
/// @brief Find packing defects at an interface using packstat score terms
/// @author Jacob Corn (jecorn@u.washington.edu)

#ifndef INCLUDED_protocols_ligand_docking_ChainExistsFilter_hh
#define INCLUDED_protocols_ligand_docking_ChainExistsFilter_hh


#include <protocols/filters/Filter.hh>

#include <utility/tag/Tag.fwd.hh>



namespace protocols {
namespace ligand_docking {

class ChainExistsFilter : public protocols::filters::Filter
{
public:
	ChainExistsFilter() :
		//utility::VirtualBase(),
		protocols::filters::Filter( "ChainExists" )
	{}

	ChainExistsFilter(std::string const & chain ) :
		//utility::VirtualBase(),
		protocols::filters::Filter( "ChainExists" ),
		chain_(chain)
	{}

	ChainExistsFilter( ChainExistsFilter const & init ) :
		//utility::VirtualBase(),
		protocols::filters::Filter( init ),
		chain_(init.chain_)
	{};

	~ChainExistsFilter() override= default;

	bool apply( core::pose::Pose const & pose ) const override;
	protocols::filters::FilterOP clone() const override {
		return utility::pointer::make_shared< ChainExistsFilter >( *this );
	}

	protocols::filters::FilterOP fresh_instance() const override{
		return utility::pointer::make_shared< ChainExistsFilter >();
	}

	void parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & ) override;

	std::string
	name() const override;

	static
	std::string
	class_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:
	std::string chain_;
};
} // ligand_docking
} // protocols

#endif //INCLUDED_protocols_ProteinInterfaceDesign_ligand_docking_ChainExistsFilter_HH_
