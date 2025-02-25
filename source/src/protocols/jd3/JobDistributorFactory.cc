// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/jd3/JobDistributorFactory.cc
/// @brief  JobDistributorFactory class; reads the command line to create a JobDistributor
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Unit headers
#include <protocols/jd3/JobDistributorFactory.hh>

// Package headers
#include <protocols/jd3/JobDistributor.fwd.hh>
//#include <protocols/jd3/job_distributors/MPIWorkPartitionJobDistributor.hh>
#include <protocols/jd3/job_distributors/VanillaJobDistributor.hh>

#include <basic/Tracer.hh>

#ifdef MULTI_THREADED
#include <protocols/jd3/job_distributors/MultiThreadedJobDistributor.hh>
#endif

#ifdef USEMPI
#include <protocols/jd3/job_distributors/MPIWorkPoolJobDistributor.hh>
#endif

namespace protocols {
namespace jd3 {

static basic::Tracer TR( "protocols.jd3.JobDistributorFactory" );

JobDistributorOP
JobDistributorFactory::create_job_distributor()
{
	// TEMP!
#ifdef USEMPI
#ifdef SERIALIZATION
	// In future: some command line option?
	// if ( basic::options::option[ basic::options::OptionKeys::jd3::mpi_work_partition_job_distributor ].value() ) {
	// 	return utility::pointer::make_shared< job_distributors::MPIWorkPartitionJobDistributor >();
	// } else {
	TR << "Creating MPIWorkPoolJobDistributor" << std::endl;
	return utility::pointer::make_shared< job_distributors::MPIWorkPoolJobDistributor >();
	//}
#else
	TR.Error << "JD3 cannot be used with MPI but without serialization; you must add \"extras=mpi,serlization\" to your ./scons.py build command" << std::endl;
	return nullptr;
#endif // SERIALIZATION
#endif // USEMPI

#ifdef MULTI_THREADED
	TR << "Creating MultiThreadedJobDistributor" << std::endl;
	return utility::pointer::make_shared< job_distributors::MultiThreadedJobDistributor >();
#else
	TR << "Creating VanillaJobDistributor" << std::endl;
	return utility::pointer::make_shared< job_distributors::VanillaJobDistributor >();
#endif


}


} // namespace jd3
} // namespace protocols

