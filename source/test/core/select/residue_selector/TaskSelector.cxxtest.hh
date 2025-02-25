// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/select/residue_selector/TaskSelector.cxxtest.hh
/// @brief test suite for core::select::residue_selector::TaskSelector
/// @author Tom Linsky (tlinsky@gmail.com)

// Test headers
#include <cxxtest/TestSuite.h>
#include <test/util/pose_funcs.hh>

// Package headers
#include <protocols/task_operations/DesignAroundOperation.hh>
#include <protocols/residue_selectors/TaskSelector.hh>

// Project headers
#include <core/pose/Pose.hh>

// Utility headers
#include <utility/tag/Tag.hh>

// Basic headers
#include <basic/datacache/DataMap.hh>

// C++ headers

#include <core/init_util.hh> // AUTO IWYU For core_init

using namespace core::select::residue_selector;
using namespace protocols::residue_selectors;

static basic::Tracer TR( "test.core.select.TaskSelectorTests" );

class TaskSelectorTests : public CxxTest::TestSuite {

public:

	void setUp() {
		core_init();
	}

	void test_TaskSelector_parse_my_tag() {
		std::stringstream ss;
		ss << "<Task name=\"allloops\" />";
		utility::tag::TagOP tag( new utility::tag::Tag() );
		tag->read( ss );
		basic::datacache::DataMap dm;

		TaskSelectorOP rs( new TaskSelector );
		TS_ASSERT_THROWS( rs->parse_my_tag( tag, dm ), utility::excn::RosettaScriptsOptionError & );

		std::stringstream ssgood;
		ssgood << "<Task name=\"good_tos\" task_operations=\"test_op,test_op2\" packable=\"1\" />";
		tag->read( ssgood );
		TS_ASSERT_THROWS( rs->parse_my_tag( tag, dm ), utility::excn::RosettaScriptsOptionError & );

		protocols::task_operations::DesignAroundOperationOP des_around( new protocols::task_operations::DesignAroundOperation );
		des_around->include_residue( 2 );
		dm.add( "task_operations", "test_op", des_around );
		dm.add( "task_operations", "test_op2", des_around );
		TS_ASSERT_THROWS_NOTHING( rs->parse_my_tag( tag, dm ) );

		core::pose::Pose trpcage = create_trpcage_ideal_pose();
		core::select::residue_selector::ResidueSubset subset = rs->apply( trpcage );
		trpcage.dump_pdb( "trpcage.pdb" );
		std::set< core::Size > const residues = { 1, 2, 3, 4, 5, 6, 19 };
		for ( core::Size resid=1; resid<=trpcage.size(); ++resid ) {
			TS_ASSERT_EQUALS( residues.find( resid ) != residues.end(), subset[resid] );
			TR << resid << " " << subset[resid] << std::endl;
		}

		// select frozen
		rs->set_select_designable( false );
		rs->set_select_packable( false );
		//rs->set_select_frozen( true );
		subset = rs->apply( trpcage );
		for ( core::Size resid=1; resid<=trpcage.size(); ++resid ) {
			TS_ASSERT_EQUALS( residues.find( resid ) == residues.end(), subset[resid] );
			TR << resid << " " << subset[resid] << std::endl;
		}

	}

};
