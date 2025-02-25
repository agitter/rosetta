// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file Monica Berrondo October 22 2007
/// @brief


// libRosetta headers

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <numeric/random/random.hh>

#include <protocols/jobdist/standard_mains.hh>

#include <protocols/moves/Mover.hh>


#include <devel/init.hh>

// C++ headers
//#include <cstdlib>
#include <iostream>
#include <string>

#include <basic/Tracer.hh>

//Auto Headers
#include <protocols/relax/ClassicRelax.hh>

using basic::Error;
using basic::Warning;

using namespace core;

int
main( int argc, char * argv [] )
{
	try {
		using namespace protocols;
		using namespace protocols::jobdist;
		using namespace protocols::moves;

		// initialize core
		devel::init(argc, argv);

		core::scoring::ScoreFunctionOP scorefxn;
		scorefxn = core::scoring::get_score_function();

		MoverOP relax = new relax::ClassicRelax( scorefxn );

		protocols::jobdist::main_plain_mover( *relax);
		std::cout << "Done! -------------------------------\n";
	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
}

