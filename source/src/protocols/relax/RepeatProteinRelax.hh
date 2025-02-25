// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file RepeatProteinRelax.cc
/// @brief This code is to be used instead of relax/minimize for repeat proteins. It maintains perfect symmetry while running relax
/// @author TJ Brunette

// Unit headers

#ifndef INCLUDED_protocols_simple_moves_RepeatProteinRelax_hh
#define INCLUDED_protocols_simple_moves_RepeatProteinRelax_hh

#include <protocols/relax/RepeatProteinRelax.fwd.hh>

#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>
#include <core/pose/Pose.fwd.hh>

// Utility Headers
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/types.hh>

// C++ Headers
#include <string>
#include <utility/vector1.hh>

//unit header
#include <protocols/relax/RelaxProtocolBase.hh>
#include <protocols/moves/Mover.fwd.hh>

#include <map> // AUTO IWYU For map


namespace protocols {
namespace relax {

class RepeatProteinRelax : public RelaxProtocolBase {
public:
	RepeatProteinRelax();
	core::Size get_midpoint_longest_loop(core::pose::Pose const & pose,core::Size const repeat_length);
	void setup_repeat_symminfo(core::Size const repeatlen, core::Size const nrepeat, core::Size const base_repeat, bool const with_jumping, core::conformation::symmetry::SymmetryInfo & symminfo);
	void setup_repeat_pose(core::pose::Pose & pose);
	void setup_repeat_pose_jumping(core::pose::Pose & pose);
	void minimize_pose(core::pose::Pose & pose);
	void relax_pose(core::pose::Pose & pose);
	void setup_movemap(core::pose::Pose & pose);
	void seal_jumps(core::pose::Pose & pose);
	void add_residue_labels_back(core::pose::Pose & pose, std::map<core::Size, utility::vector1<std::string> > res_label_map,int symmetry_resid_offset);
	void apply(core::pose::Pose & pose ) override;
	moves::MoverOP clone() const override { return utility::pointer::make_shared< RepeatProteinRelax >( *this ); }
	void parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & datamap ) override;
	std::string get_name() const override;
	static std::string mover_name();
	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
private:
	core::Size numb_repeats_;
	bool minimize_;
	bool loop_cutpoint_mode_;
	core::Size relax_iterations_;
	bool cartesian_;
	bool ramp_down_constraints_;
	bool remove_symm_;
	bool modify_symmetry_and_exit_;
	std::string min_type_;
	core::scoring::ScoreFunctionOP sfxn_;

};
} //protocols
} //moves
#endif
