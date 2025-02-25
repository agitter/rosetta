// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/chemical/rotamers/StoredRotamerLibrarySpecificationCreator.hh
/// @brief  Class for instantiating a particular StoredRotamerLibrarySpecification
/// @author Rocco Moretti (rmorettiase@gmail.com)

#ifndef INCLUDED_core_chemical_rotamers_StoredRotamerLibrarySpecificationCreator_HH
#define INCLUDED_core_chemical_rotamers_StoredRotamerLibrarySpecificationCreator_HH

// Package headers
#include <core/chemical/rotamers/RotamerLibrarySpecificationCreator.hh>


// Program header

// Utility headers

// C++ headers
#include <string>
#include <iosfwd>

namespace core {
namespace chemical {
namespace rotamers {

class StoredRotamerLibrarySpecificationCreator : public RotamerLibrarySpecificationCreator {
public:
	RotamerLibrarySpecificationOP
	create() const override;

	RotamerLibrarySpecificationOP
	create( std::istream & ) const override;

	std::string keyname() const override;

};


} //namespace rotamers
} //namespace chemical
} //namespace core


#endif
