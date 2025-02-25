// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @author Zibo Chen


#ifndef INCLUDED_protocols_cryst_TwoDLattice_hh
#define INCLUDED_protocols_cryst_TwoDLattice_hh

#include <protocols/cryst/wallpaper.hh>

#include <core/types.hh>

#include <core/pose/Pose.fwd.hh>
#include <core/chemical/ChemicalManager.hh>
#include <core/chemical/ResidueTypeSet.hh>
#include <core/conformation/Residue.hh>
#include <core/conformation/ResidueFactory.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>


#include <protocols/moves/Mover.fwd.hh>



#include <utility/vector1.hh>
#include <numeric/xyzVector.hh>

#include <protocols/moves/Mover.hh> // AUTO IWYU For Mover





namespace protocols {
namespace cryst {


class MakeLayerMover : public protocols::moves::Mover {
private:
	WallpaperGroup wg_;
	core::Real contact_dist_;
	bool moving_lattice_;

public:
	MakeLayerMover() : contact_dist_(24.0), moving_lattice_(true) {}

	void
	set_moving_lattice( bool val ) { moving_lattice_ = val; }

	core::Size
	place_near_origin (
		core::pose::Pose & pose
	);

	void
	add_monomers_to_layer(
		core::pose::Pose const & monomer_pose,
		core::pose::Pose & pose,
		utility::vector1<core::Size> const & monomer_anchors,
		utility::vector1<core::Size> & monomer_jumps,
		core::Size rootres
	);

	void
	detect_connecting_subunits(
		core::pose::Pose const & monomer_pose,
		core::pose::Pose const & pose,
		utility::vector1<core::Size> & monomer_anchors,
		core::Size &basesubunit
	);

	void
	build_layer_of_virtuals(
		core::pose::Pose & pose,
		utility::vector1<core::Size> &Ajumps,
		utility::vector1<core::Size> &Bjumps,
		utility::vector1<core::Size> &subunit_anchors,
		core::Size &basesubunit
	);

	void
	setup_xtal_symminfo(
		core::pose::Pose const & pose,
		core::Size const num_monomers,
		core::Size const num_virtuals,
		core::Size const base_monomer,
		core::Size const nres_monomer,
		utility::vector1<core::Size> const &Ajumps,
		utility::vector1<core::Size> const &Bjumps,
		utility::vector1<core::Size> const &monomer_jumps,
		core::conformation::symmetry::SymmetryInfo & symminfo
	);


	void apply( core::pose::Pose & pose ) override;


	protocols::moves::MoverOP clone() const override { return utility::pointer::make_shared< MakeLayerMover >(*this); }
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< MakeLayerMover >(); }

	void
	wallpaper_group( WallpaperGroup const & wg_in ) {
		wg_ = wg_in;
	}

	void
	contact_dist( core::Real const setting ) {
		contact_dist_ = setting;
	}

	void parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:

	core::conformation::ResidueOP make_vrt( core::Vector O, core::Vector X, core::Vector Y ) {
		core::conformation::ResidueOP vrtrsd
			( core::conformation::ResidueFactory::create_residue(
			core::chemical::ChemicalManager::get_instance()->residue_type_set( core::chemical::FA_STANDARD )->name_map( "VRT" ) ) );
		vrtrsd->set_xyz("ORIG",O);
		vrtrsd->set_xyz("X",O+X);
		vrtrsd->set_xyz("Y",O+Y);
		return vrtrsd;
	}
};

}
}

#endif
