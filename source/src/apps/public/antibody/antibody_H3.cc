// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/apps/pilot/jianqing/antibody_H3.cc
/// @brief Code to model and antibody CDR H3 loop, simultaneously optimizing surrounding CDR loops and VH/VL orientation
/// @author Jianqing Xu (xubest@gmail.com), Daisuke Kuroda, Brian Weitzer, Jeff Gray
/// 09/09/2011 - 2013


#include <protocols/antibody/AntibodyModelerProtocol.hh>
#include <protocols/jd2/JobDistributor.hh>
#include <protocols/jd2/internal_util.hh>

#include <devel/init.hh>

// Utility headers
#include <utility/excn/Exceptions.hh>
//#include <utility/tools/make_vector1.hh>

// option key includes
#include <basic/Tracer.hh>


static basic::Tracer TR( "protocols.antibody" );


int
main( int argc, char * argv [] )
{
	try {

		using namespace protocols::antibody;
		using namespace protocols::jd2;

		AntibodyModelerProtocol::register_options();
		protocols::jd2::register_options();
		// initialize core
		devel::init(argc, argv);


		AntibodyModelerProtocolOP ab_m_h3( new AntibodyModelerProtocol() );
		TR<<*ab_m_h3<<std::endl;
		//    exit(-1);

		JobDistributor::get_instance()->go(ab_m_h3);

	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
}


