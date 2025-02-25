// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/func/SplineFunc.cc
/// @brief  Constraint function for looking up data in a histogram over which a spline is created
/// @details The HistogramFunc constraints function allows one to input a histogram (in most cases, a knowledge-based potential).
/// This class loads a lookup table (your histogram), provides an interface for accessing it, creates a spline, and returns a value based on the spline generated from the knowledge-based potential.
/// @author Stephanie Hirst (stephanie.j.hirst@vanderbilt.edu)

// Unit Headers
#include <core/scoring/func/SplineFunc.hh>

// Package Headers
#include <basic/options/keys/constraints.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <basic/database/open.hh>

// Project Headers
#include <basic/Tracer.hh>

// Utility and Numeric Headers
#include <utility/vector1.hh>
#include <numeric/interpolation/spline/SplineGenerator.hh>
#include <numeric/interpolation/spline/Interpolator.hh>
#include <numeric/interpolation/util.hh>

// C++ Headers
#include <iostream>
#include <string>
#include <cmath>

#include <utility/stream_util.hh> // AUTO IWYU For operator<<

static basic::Tracer TR( "core.scoring.constraints.SplineFunc" );

#ifdef SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>
#include <utility/vector1.srlz.hh>

// Cereal headers
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#endif // SERIALIZATION


namespace core {
namespace scoring {
namespace func {

SplineFunc::SplineFunc():
	exp_val_(0),
	filename_(""),
	KB_description_(""),
	weight_(0),
	bin_size_(0),
	lower_bound_x_(0),
	lower_bound_y_(0),
	upper_bound_x_(0),
	upper_bound_y_(0),
	lower_bound_dy_(0),
	upper_bound_dy_(0),
	bins_vect_(),
	bins_vect_size_(0),
	potential_vect_(),
	potential_vect_size_(0),
	interpolator_()
{}


SplineFunc::SplineFunc(
	std::string const & KB_description,
	core::Real const weight,
	core::Real const exp_val,
	core::Real const bin_size,
	utility::vector1<core::Real> const & bins_vect,
	utility::vector1<core::Real> const & potential_vect,
	utility::vector1<std::tuple<std::string, platform::Real, platform::Real, platform::Real>> const & boundary_functions):
	exp_val_(exp_val),
	KB_description_(KB_description),
	weight_(weight),
	bin_size_(bin_size),
	bins_vect_(bins_vect),
	bins_vect_size_(bins_vect.size()),
	potential_vect_(potential_vect),
	potential_vect_size_(potential_vect.size())
{
	numeric::interpolation::spline::SplineGenerator common_spline( numeric::interpolation::make_spline( bins_vect, potential_vect, bin_size, boundary_functions ) );

	interpolator_ = common_spline.get_interpolator();
	lower_bound_x_ = common_spline.get_lbx();
	upper_bound_x_ = common_spline.get_ubx();
	lower_bound_y_ = common_spline.get_lby();
	upper_bound_y_ = common_spline.get_uby();
	upper_bound_x_ = common_spline.get_ubx();
	lower_bound_dy_ = common_spline.get_lbdy();
	upper_bound_dy_ = common_spline.get_ubdy();
}



SplineFunc::~SplineFunc() = default;

bool SplineFunc::operator == ( Func const & other ) const
{
	if ( !       same_type_as_me( other ) ) return false;
	if ( ! other.same_type_as_me( *this ) ) return false;

	auto const & other_downcast( static_cast< SplineFunc const & > (other) );

	if ( exp_val_             != other_downcast.exp_val_             ) return false;
	if ( filename_            != other_downcast.filename_            ) return false;
	if ( KB_description_      != other_downcast.KB_description_      ) return false;
	if ( weight_              != other_downcast.weight_              ) return false;
	if ( bin_size_            != other_downcast.bin_size_            ) return false;
	if ( lower_bound_x_       != other_downcast.lower_bound_x_       ) return false;
	if ( lower_bound_y_       != other_downcast.lower_bound_y_       ) return false;
	if ( upper_bound_x_       != other_downcast.upper_bound_x_       ) return false;
	if ( upper_bound_y_       != other_downcast.upper_bound_y_       ) return false;
	if ( lower_bound_dy_      != other_downcast.lower_bound_dy_      ) return false;
	if ( upper_bound_dy_      != other_downcast.upper_bound_dy_      ) return false;
	if ( bins_vect_           != other_downcast.bins_vect_           ) return false;
	if ( bins_vect_size_      != other_downcast.bins_vect_size_      ) return false;
	if ( potential_vect_      != other_downcast.potential_vect_      ) return false;
	if ( potential_vect_size_ != other_downcast.potential_vect_size_ ) return false;

	return interpolator_ == other_downcast.interpolator_ ||
		( interpolator_ && other_downcast.interpolator_ && *interpolator_ == *other_downcast.interpolator_ );

}

bool SplineFunc::same_type_as_me( Func const & other ) const
{
	return dynamic_cast< SplineFunc const * > ( &other );
}



/// @brief Compare this SplineFunc to another one, and determine whether
/// the two are equal within some threshold.
/// @details The float threshold is used for comparing floating-point values.
bool
SplineFunc::is_approximately_equal(
	SplineFunc const & other,
	core::Real const float_threshold
) const {
	if ( this == &other ) return true; //If this is the same object, then we're done.
	if ( !       same_type_as_me( other ) ) return false;
	if ( ! other.same_type_as_me( *this ) ) return false;

	if ( !equal_within_thresh( exp_val_, other.exp_val_, float_threshold) ) return false;
	if ( !equal_within_thresh( weight_, other.weight_, float_threshold) ) return false;
	if ( !equal_within_thresh( bin_size_, other.bin_size_, float_threshold ) ) return false;
	if ( !equal_within_thresh( lower_bound_x_, other.lower_bound_x_, float_threshold ) ) return false;
	if ( !equal_within_thresh( lower_bound_y_, other.lower_bound_y_, float_threshold ) ) return false;
	if ( !equal_within_thresh( upper_bound_x_, other.upper_bound_x_, float_threshold ) ) return false;
	if ( !equal_within_thresh( upper_bound_y_, other.upper_bound_y_, float_threshold ) ) return false;
	if ( !equal_within_thresh( lower_bound_dy_, other.lower_bound_dy_, float_threshold ) ) return false;
	if ( !equal_within_thresh( upper_bound_dy_, other.upper_bound_dy_, float_threshold ) ) return false;
	if ( bins_vect_.size() != other.bins_vect_.size() ) return false;
	if ( potential_vect_.size() != other.potential_vect_.size() ) return false;
	for ( core::Size i(1), imax(bins_vect_.size()); i<=imax; ++i ) {
		if ( !equal_within_thresh( bins_vect_[i], other.bins_vect_[i], float_threshold ) ) return false;
	}
	for ( core::Size j(1), jmax(potential_vect_.size()); j<=jmax; ++j ) {
		if ( !equal_within_thresh(potential_vect_[j], other.potential_vect_[j], float_threshold) ) return false;
	}

	return true;
}


// get_ functions to obtain values of member variables (mostly for unit test)
core::Real SplineFunc::get_exp_val() const
{
	return exp_val_;
}

std::string const & SplineFunc::get_filename() const
{
	return filename_;
}

std::string const & SplineFunc::get_KB_description() const
{
	return KB_description_;
}

core::Real SplineFunc::get_weight() const
{
	return weight_;
}

core::Real SplineFunc::get_bin_size() const
{
	return bin_size_;
}

core::Size SplineFunc::get_bins_vect_size() const
{
	return bins_vect_size_;
}

core::Size SplineFunc::get_potential_vect_size() const
{
	return potential_vect_size_;
}

core::Real SplineFunc::get_lower_bound_x() const
{
	return lower_bound_x_;
}

core::Real SplineFunc::get_upper_bound_x() const
{
	return upper_bound_x_;
}

core::Real SplineFunc::get_lower_bound_y() const
{
	return lower_bound_y_;
}

core::Real SplineFunc::get_upper_bound_y() const
{
	return upper_bound_y_;
}

core::Real SplineFunc::get_lower_bound_dy() const
{
	return lower_bound_dy_;
}

core::Real SplineFunc::get_upper_bound_dy() const
{
	return upper_bound_dy_;
}

utility::vector1<core::Real> const & SplineFunc::get_bins_vect() const
{
	return bins_vect_;
}

utility::vector1<core::Real> const & SplineFunc::get_potential_vect() const
{
	return potential_vect_;
}

/// @brief Initialize this SplineFunc from the given izstream.
/// @details Triggers read from disk UNLESS the first entry (filename) is "NONE" (upper or lower case or any mixture).
/// In that case, the spline is read from the rest of the line.
void SplineFunc::read_data( std::istream &in )
{
	// If constraints::epr_distance specified, read in histogram from database
	if ( basic::options::option[ basic::options::OptionKeys::constraints::epr_distance]() ) {
		in >> KB_description_ >> exp_val_ >> weight_ >> bin_size_;
		filename_ = basic::database::full_name("scoring/constraints/epr_distance_potential.histogram");
	} else {
		// Else, read in potential specified in constraints file
		in >> KB_description_ >> filename_ >> exp_val_ >> weight_ >> bin_size_;
	}
	numeric::interpolation::spline::SplineGenerator common_spline(
		utility::upper(filename_) == "NONE" ?
		//If filename is "NONE" (or any case variant), then read the spline from the rest of the line.
		//The line must be: [Constraint setup] [Desc.] NONE [Exp. val.] [Weight] [Bin size] xaxis [x1] [x2] [x3] ... [xn] yaxis [y1] [y2] [y3] ... [yn]
		//Optionally, it may include lb_function [cutoff] [slope] [intercept] or ub_function [cutoff] [slope] [intercept].
		numeric::interpolation::spline_from_stream( in, bin_size_ )
		:
		//If filename is not "NONE" (or any case variant), then read the spline from a histogram on disk:
		numeric::interpolation::spline_from_file( filename_, bin_size_ )
	);

	interpolator_ = common_spline.get_interpolator();
	lower_bound_x_ = common_spline.get_lbx();
	//std::cout << "now in SplineFunc..." << std::endl;
	//std::cout << "histogram_lbx: " << lower_bound_x_ << std::endl;
	upper_bound_x_ = common_spline.get_ubx();
	//std::cout << "histogram_ubx: " << upper_bound_x_ << std::endl;
	lower_bound_y_ = common_spline.get_lby();
	//std::cout << "histogram_lby: " << lower_bound_y_ << std::endl;
	upper_bound_y_ = common_spline.get_uby();
	upper_bound_x_ = common_spline.get_ubx();
	//std::cout << "" << "histogram_uby: " << upper_bound_y_ << std::endl;
	lower_bound_dy_ = common_spline.get_lbdy();
	upper_bound_dy_ = common_spline.get_ubdy();
	bins_vect_size_ = common_spline.get_num_points();
	potential_vect_size_ = common_spline.get_num_points();

	utility::vector1<numeric::interpolation::spline::Point> const & points(common_spline.get_points());
	for ( auto const & point : points ) {
		bins_vect_.push_back(point.x);
		potential_vect_.push_back(point.y);
	}
} // read_data()

/// @brief Returns the value of this SplineFunc evaluated at distance x.
// Insert spline into a lookup table and get potential
core::Real SplineFunc::func( core::Real const x) const
{
	core::Real potential_energy;
	core::Real delta_potential_energy;
	core::Real weighted_potential_energy;

	// This will be added an EPR-specific class later
	if ( KB_description_ == "EPR_DISTANCE" ) {
		// calculate sl-cb
		core::Real diff = exp_val_ - x; // for AtomPair constraints, x is the distance between atoms/residues in the model.

		// The upper and lower bounds of the x-axis are hard coded.  This is a hack but will be fixed later!
		if ( diff >= -15.0 && diff <= 15.0 ) {
			interpolator_->interpolate( diff, potential_energy, delta_potential_energy);
			weighted_potential_energy = ( weight_*potential_energy);
			return weighted_potential_energy;
		} else {
			// Return potential = 0 for sl-cb values outside of lowest and highest values in histogram
			potential_energy = 0;
			return potential_energy;
		}
	} else { // for other cases that are not EPR Distance restraints, just return potential for x, not sl-cb
		if ( KB_description_ == "difference" ) {
			//std::cout << "using difference!" << std::endl;
			//calculate exp_distance - CB distance
			core::Real diff = exp_val_ - x; // for AtomPair constraints, x is the distance between atoms/residues in the model

			//std::cout << "x is:  " << x << " and difference is:  " << diff << " and weight is:  " << weight_ << std::endl;
			if ( diff >= lower_bound_x_ && diff <= upper_bound_x_ ) {
				interpolator_->interpolate( diff, potential_energy, delta_potential_energy);
				weighted_potential_energy = ( weight_*potential_energy);
				return weighted_potential_energy;
			} else {
				// Return potential = 0 for x values outside of lowest and highest values in histogram
				potential_energy = 0;
				return potential_energy;
			}
			//std::cout << "potential energy is:  " << potential_energy<< " and " << weighted_potential_energy << std::endl;
		} else {
			if ( x >= lower_bound_x_ && x <= upper_bound_x_ ) {
				interpolator_->interpolate( x, potential_energy, delta_potential_energy);
				weighted_potential_energy = ( weight_*potential_energy);
				return weighted_potential_energy;
			} else {
				// Return potential = 0 for x values outside of lowest and highest values in histogram
				potential_energy = 0;
				return potential_energy;
			}//else
		}//else
	}

} // SplineFunc::func()

/// @brief Returns the value of the first derivative of this SplineFunc at distance x.
core::Real SplineFunc::dfunc( core::Real const x) const
{
	core::Real potential_energy;
	core::Real delta_potential_energy;

	// This will be added an EPR-specific class later
	if ( KB_description_ == "EPR_DISTANCE" ) {
		// calculate sl-cb
		core::Real diff = exp_val_ - x; // for AtomPair constraints, x is the distance between atoms/residues in the model.
		if ( diff >= -15.0 && diff <= 16.0 ) {
			interpolator_->interpolate( diff, potential_energy, delta_potential_energy);
			return ( weight_*delta_potential_energy);
		}

		// Return delta_potential = 0 for sl-cb values outside of lowest and highest values in histogram
		return 0;
	} else { // for other cases that are not EPR Distance restraints, just return potential for x, not sl-cb
		if ( x >= lower_bound_x_ && x <= upper_bound_x_ ) {
			interpolator_->interpolate( x, potential_energy, delta_potential_energy);
			return ( weight_*delta_potential_energy);
		}

		// Return delta_potential_energy = 0 for x values outside of lowest and highest values in histogram
		return 0;
	}

} // SplineFunc::dfunc()

/// @brief show the definition of this SplineFunc to the specified output stream.
void SplineFunc::show_definition( std::ostream &out ) const
{
	out << "SPLINE" << "\t" << KB_description_ << "\t" << "None" << "\t" << exp_val_ << "\t" << weight_ << "\t" << bin_size_;
	out << "\tx_axis";
	for ( core::Real const & entry : bins_vect_ ) {
		out << "\t" << entry;
	}
	out << "\ty_axis";
	for ( core::Real const & entry : potential_vect_ ) {
		out << "\t" << entry;
	}

	if ( interpolator_ != nullptr ) {
		if ( interpolator_->has_lb_function() ) {
			out << "\t" << "lb_function" << "\t" << interpolator_->get_lb_function_cutoff();
			out << "\t" << interpolator_->get_lb_function_slope();
			out << "\t" << interpolator_->get_lb_function_intercept();
		}
		if ( interpolator_->has_ub_function() ) {
			out << "\t" << "ub_function" << "\t" << interpolator_->get_ub_function_cutoff();
			out << "\t" << interpolator_->get_ub_function_slope();
			out << "\t" << interpolator_->get_ub_function_intercept();
		}
	}

	out << std::endl;
}

/// @brief show some sort of stringified representation of the violations for this constraint.
core::Size SplineFunc::show_violations( std::ostream &out, core::Real x, core::Size verbose_level, core::Real threshold) const
{
	core::Real potential_energy;
	core::Real delta_potential_energy;
	core::Real weighted_potential_energy;

	if ( KB_description_ == "EPR_DISTANCE" ) {
		// calculate sl-cb
		core::Real diff = exp_val_ - x; // for AtomPair constraints, x is the distance between atoms/residues in the model.

		// calculate potential_energy again
		if ( diff >= -15.0 && diff <= 16.0 ) {
			interpolator_->interpolate( diff, potential_energy, delta_potential_energy);
			weighted_potential_energy = (weight_*potential_energy);
		} else {
			// Return potential = 0 for sl-cb values outside of lowest and highest values in histogram
			weighted_potential_energy = 0;
		}

		// output according to specified verbosity level
		if ( verbose_level > 100 ) {
			out << "SPLINEFUNC:" << "\t" << "Description:  " << KB_description_ << "\t"
				<< "exp_val:  " << exp_val_ << "\t" << "model_dist:  " << x << "\t" << "dsl-dcb:  " << diff << "\t"
				<< "weighted_potential:  " << weighted_potential_energy << "\t"
				<< "weight:" << "\t" << weight_ << std::endl;
		} else if ( verbose_level > 70 ) {
			out << "SPLINEFUNC:" << "\t" << "Description:  " << KB_description_ << "\t"
				<< "exp_val:  " << exp_val_ << "\t" << "model_dist:  " << x << "\t" << "dsl-dcb:  " << diff << std::endl;
		}
	} else { // for other cases that are not EPR Distance restraints, just return potential for x, not sl-cb
		if ( x >= lower_bound_x_ && x <= upper_bound_x_ ) {
			interpolator_->interpolate( x, potential_energy, delta_potential_energy);
			weighted_potential_energy = (weight_*potential_energy);
		} else {
			// Return potential = 0 for x values outside of lowest and highest values in histogram
			weighted_potential_energy = 0;
		}

		// output according to specified verbosity level
		if ( verbose_level > 100 ) {
			out << "SPLINEFUNC:" << "\t" << "Description:  " << KB_description_ << "\t" << "exp_val:  " << exp_val_ << "\t"
				<< "model_dist:  " << x << "\t" << "exp_val - x:  " << exp_val_ - x  << "\t" << "weighted_potential:  " << weighted_potential_energy
				<< "\t" << "weight:" << weight_ << std::endl;
		} else if ( verbose_level > 70 ) {
			out << "SPLINEFUNC:" << "\t" << "Description:  " << KB_description_ << "\t"
				<< "exp_val:  " << exp_val_ << "\t" << "model_dist:  " << x << std::endl;
		}
	}

	return Func::show_violations( out, x, verbose_level, threshold);
} // show_violations()

/// @brief Are two values equal within some threshold?
bool
SplineFunc::equal_within_thresh(
	core::Real const val1,
	core::Real const val2,
	core::Real const threshold
) const {
	return std::abs( val1 - val2 ) < threshold;
}

} // constraints
} // scoring
} // core

#ifdef    SERIALIZATION

/// @brief Automatically generated serialization method
template< class Archive >
void
core::scoring::func::SplineFunc::save( Archive & arc ) const {
	arc( cereal::base_class< Func >( this ) );
	arc( CEREAL_NVP( exp_val_ ) ); // core::Real
	arc( CEREAL_NVP( filename_ ) ); // std::string
	arc( CEREAL_NVP( KB_description_ ) ); // std::string
	arc( CEREAL_NVP( weight_ ) ); // core::Real
	arc( CEREAL_NVP( bin_size_ ) ); // core::Real
	arc( CEREAL_NVP( lower_bound_x_ ) ); // core::Real
	arc( CEREAL_NVP( lower_bound_y_ ) ); // core::Real
	arc( CEREAL_NVP( upper_bound_x_ ) ); // core::Real
	arc( CEREAL_NVP( upper_bound_y_ ) ); // core::Real
	arc( CEREAL_NVP( lower_bound_dy_ ) ); // core::Real
	arc( CEREAL_NVP( upper_bound_dy_ ) ); // core::Real
	arc( CEREAL_NVP( bins_vect_ ) ); // utility::vector1<core::Real>
	arc( CEREAL_NVP( bins_vect_size_ ) ); // utility::vector1<core::Real>::size_type
	arc( CEREAL_NVP( potential_vect_ ) ); // utility::vector1<core::Real>
	arc( CEREAL_NVP( potential_vect_size_ ) ); // utility::vector1<core::Real>::size_type
	arc( CEREAL_NVP( interpolator_ ) ); // numeric::interpolation::spline::InterpolatorOP
}

/// @brief Automatically generated deserialization method
template< class Archive >
void
core::scoring::func::SplineFunc::load( Archive & arc ) {
	arc( cereal::base_class< Func >( this ) );
	arc( exp_val_ ); // core::Real
	arc( filename_ ); // std::string
	arc( KB_description_ ); // std::string
	arc( weight_ ); // core::Real
	arc( bin_size_ ); // core::Real
	arc( lower_bound_x_ ); // core::Real
	arc( lower_bound_y_ ); // core::Real
	arc( upper_bound_x_ ); // core::Real
	arc( upper_bound_y_ ); // core::Real
	arc( lower_bound_dy_ ); // core::Real
	arc( upper_bound_dy_ ); // core::Real
	arc( bins_vect_ ); // utility::vector1<core::Real>
	arc( bins_vect_size_ ); // utility::vector1<core::Real>::size_type
	arc( potential_vect_ ); // utility::vector1<core::Real>
	arc( potential_vect_size_ ); // utility::vector1<core::Real>::size_type
	arc( interpolator_ ); // numeric::interpolation::spline::InterpolatorOP
}

SAVE_AND_LOAD_SERIALIZABLE( core::scoring::func::SplineFunc );
CEREAL_REGISTER_TYPE( core::scoring::func::SplineFunc )

CEREAL_REGISTER_DYNAMIC_INIT( core_scoring_func_SplineFunc )
#endif // SERIALIZATION
