// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/pack/task/PackerTask.hh
/// @brief  Task class to describe packer's behavior header
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)
/// @author Steven Lewis
/// @modified Vikram K. Mulligan (vmulligan@flatironinstitute.org) -- Added support for multithreading.

#ifndef INCLUDED_core_pack_task_PackerTask_hh
#define INCLUDED_core_pack_task_PackerTask_hh

// Unit Headers
#include <core/pack/task/PackerTask.fwd.hh>

// Package Headers
#include <core/pack/rotamer_set/RotamerCouplings.fwd.hh>
#include <core/pack/rotamer_set/RotamerLinks.fwd.hh>
#include <core/pack/rotamer_set/RotamerSetOperation.fwd.hh>
#include <core/pack/task/IGEdgeReweightContainer.fwd.hh>

// Project Headers
#include <core/conformation/Residue.fwd.hh>
#include <core/chemical/AA.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/id/SequenceMapping.fwd.hh>

// Utility Headers
#include <utility/VirtualBase.hh>
#include <utility/options/keys/OptionKeyList.fwd.hh>

// STL Headers
#include <iosfwd>

#include <core/types.hh> // AUTO IWYU For Size, Real
#include <utility/options/OptionCollection.fwd.hh> // AUTO IWYU For OptionCollection

#ifdef PYROSETTA
	#include <core/id/SequenceMapping.hh>
#endif


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace core {
namespace pack {
namespace task {


/// @brief  Task class that gives instructions to the packer
class PackerTask : public utility::VirtualBase, public utility::pointer::enable_shared_from_this< PackerTask >
{
public:
	typedef chemical::AA AA;
	typedef rotamer_set::RotamerCouplingsCOP RotamerCouplingsCOP;
	typedef rotamer_set::RotamerLinksCOP RotamerLinksCOP;


public:

	virtual PackerTaskOP clone() const = 0;

	/// self pointers
	inline PackerTaskCOP get_self_ptr() const { return shared_from_this(); }
	inline PackerTaskOP get_self_ptr() { return shared_from_this(); }
	//inline PackerTaskCAP get_self_weak_ptr() const { return PackerTaskCAP( shared_from_this() ); }
	//inline PackerTaskAP get_self_weak_ptr() { return PackerTaskAP( shared_from_this() ); }

	virtual void clean_residue_task( conformation::Residue const & original_residue, Size const seqpos, core::pose::Pose const & pose ) = 0;

	virtual Size total_residue() const = 0;

	virtual void temporarily_fix_everything() = 0;
	virtual void temporarily_set_pack_residue( int resid, bool setting ) = 0;

	virtual bool pack_residue( int resid ) const = 0;
	virtual bool being_packed( Size resid ) const = 0;

	virtual Size num_to_be_packed() const = 0;

	virtual bool design_residue( int resid ) const = 0;
	virtual bool being_designed( Size resid ) const = 0;
	virtual bool design_any() const = 0;

	virtual void set_bump_check( bool setting ) = 0;
	virtual bool bump_check() const = 0;
	virtual void and_max_rotbump_energy( Real setting ) = 0;
	virtual Real max_rotbump_energy() const = 0;


	virtual void or_include_current( bool setting ) = 0;
	virtual void or_include_current( bool setting, Size resid ) = 0;
	virtual bool include_current( Size resid ) const = 0;

	virtual void add_behavior( std::string const & behavior ) = 0;
	virtual void add_behavior( std::string const & behavior, Size resid ) = 0;
	virtual bool has_behavior( std::string const & behavior, Size resid ) const = 0;
	virtual bool has_behavior( Size resid ) const = 0;

	virtual void or_adducts( bool setting ) = 0;
	virtual void or_adducts( bool setting, Size resid ) = 0;
	virtual bool adducts( Size resid ) const = 0;

	virtual void or_optimize_h_mode( bool setting ) = 0;

	virtual void or_preserve_c_beta( bool setting ) = 0;

	virtual void or_flip_HNQ( bool setting ) = 0;
	virtual void or_fix_his_tautomer( utility::vector1<int> const & positions, bool setting ) = 0;

	/// @brief Activate a LinearMemoryInteraction graph that uses 95% less memory in design runs
	/// and runs twice as fast.  (Not faster for fixed-sequence repackings).
	virtual void or_linmem_ig( bool setting ) = 0;
	virtual void and_linmem_ig( bool setting ) = 0;
	virtual bool linmem_ig() const = 0;

	virtual void decrease_linmem_ig_history_size( Size setting ) = 0;
	virtual Size linmem_ig_history_size() const = 0;

	/// @brief Explicitly request the O( N^2 ) precomputed graph
	/// @details linmem_ig() is checked first and will override this setting
	virtual void or_precompute_ig( bool setting ) = 0;
	//virtual void and_precompute_ig( bool setting ) = 0;
	virtual bool precompute_ig() const = 0;

	/// @brief Activate a LazyInteractionGraph that computes rotamer pair energies at most once
	virtual void or_lazy_ig( bool setting ) = 0;
	virtual bool lazy_ig() const = 0;

	/// @brief Activates the DoubleLazyInteractionGraph, which computes rotamer pair energies at most once, and delays allocating memory to hold them until needed.  Used for multistate design.
	virtual void or_double_lazy_ig( bool setting ) = 0;
	virtual bool double_lazy_ig() const = 0;
	virtual void decrease_double_lazy_ig_memlimit( Size nbytes_for_rpes ) = 0;
	virtual Size double_lazy_ig_memlimit() const = 0;

	virtual void or_multi_cool_annealer( bool setting ) = 0;
	virtual bool multi_cool_annealer() const = 0;

	virtual void increase_multi_cool_annealer_history_size( Size setting ) = 0;
	virtual Size multi_cool_annealer_history_size() const = 0;

	virtual bool smart_annealer() const = 0;
	virtual std::string smart_annealer_model() const = 0;
	virtual core::Real smart_annealer_cutoff() const = 0;
	virtual bool smart_annealer_pick_again() const = 0;
	virtual bool smart_annealer_disable_during_quench() const = 0;
	virtual void set_smart_annealer( bool setting ) = 0;
	virtual void set_smart_annealer_model( std::string const & setting ) = 0;
	virtual void set_smart_annealer_cutoff( core::Real setting ) = 0;
	virtual void set_smart_annealer_pick_again( bool setting ) = 0;
	virtual void set_smart_annealer_disable_during_quench( bool setting ) = 0;

	virtual void show( std::ostream & out ) const = 0;
	virtual void show() const = 0;
	virtual void show_residue_task( std::ostream & out, Size resid ) const = 0;
	virtual void show_residue_task( Size resid ) const = 0;
	virtual void show_all_residue_tasks( std::ostream & out ) const = 0;
	virtual void show_all_residue_tasks() const = 0;

	/// @brief Has this PackerTask been initialized with a PackerPalette?
	/// @details PackerTasks must be initialized with PackerPalettes before being modified with TaskOperations.  The TaskFactory
	/// will initialize the PackerTask with a DefaultPackerPalette if no custom PackerPalette is provided.
	/// @author Vikram K. Mulligan (vmullig@uw.edu).
	virtual
	bool is_initialized() const = 0;

	virtual
	PackerTask &
	initialize_from_command_line() = 0;

	virtual PackerTask &
	initialize_extra_rotamer_flags_from_command_line() = 0;

	virtual
	PackerTask &
	initialize_from_options( utility::options::OptionCollection const & options ) = 0;

	virtual PackerTask &
	initialize_extra_rotamer_flags_from_options( utility::options::OptionCollection const & options ) = 0;

	static
	void
	list_options_read( utility::options::OptionKeyList & read_options );

	virtual
	PackerTask &
	restrict_to_residues( utility::vector1< bool > const & residues_allowed_to_be_packed ) = 0;

	virtual
	PackerTask &
	restrict_to_repacking() = 0;

	virtual
	ResidueLevelTask const &
	residue_task( Size resid ) const = 0;

	virtual
	ResidueLevelTask &
	nonconst_residue_task( Size resid ) = 0;

	virtual
	utility::vector1< bool >
	repacking_residues() const = 0;

	virtual
	utility::vector1< bool >
	designing_residues() const = 0;

	virtual
	bool
	keep_sequence_symmetry() const = 0;

	virtual
	void
	keep_sequence_symmetry( bool const setting ) = 0;

	/* virtual
	std::string
	sequence_symmetric_rotamer_selection_logic() const = 0; */

	/* virtual
	void
	sequence_symmetric_rotamer_selection_logic( std::string const & selection_mode ) = 0; */

	virtual
	std::string
	sequence_symmetric_uid_prefix() const = 0;

	virtual
	void
	sequence_symmetric_uid_prefix( std::string const & prefix ) = 0;

	virtual
	bool
	rotamer_couplings_exist() const = 0;

	virtual
	RotamerCouplingsCOP
	rotamer_couplings() const = 0;

	virtual
	void
	rotamer_couplings( RotamerCouplingsCOP setting ) = 0;

	virtual
	bool
	rotamer_links_exist() const = 0;

	virtual
	RotamerLinksCOP
	rotamer_links() const = 0;

	virtual
	void
	rotamer_links( RotamerLinksCOP setting ) = 0;


	virtual
	IGEdgeReweightContainerCOP
	IGEdgeReweights() const = 0;

	virtual
	IGEdgeReweightContainerOP
	set_IGEdgeReweights() = 0;

	virtual
	void
	append_rotamer_operation(
		rotamer_set::RotamerOperationOP rotop
	) = 0;

	virtual
	void
	append_rotamerset_operation(
		rotamer_set::RotamerSetOperationOP rotsetop
	) = 0;

	virtual rotamer_set::RotSetsOperationListIterator
	rotamer_sets_operation_begin() const = 0;

	virtual rotamer_set::RotSetsOperationListIterator
	rotamer_sets_operation_end() const = 0;

	virtual
	void
	append_rotamersets_operation(
		rotamer_set::RotamerSetsOperationOP rotsetsop
	) = 0;

	virtual
	core::Real
	rotamer_prob_buried() const = 0;

	virtual
	core::Real
	rotamer_prob_nonburied() const = 0;

	virtual
	core::Real
	rotamer_prob_buried_semi() const = 0;

	virtual
	core::Real
	rotamer_prob_nonburied_semi() const = 0;

	virtual
	void
	rotamer_prob_buried( core::Real ) = 0;

	virtual
	void
	rotamer_prob_nonburied( core::Real ) = 0;

	virtual
	void
	rotamer_prob_buried_semi( core::Real ) = 0;

	virtual
	void
	rotamer_prob_nonburied_semi( core::Real ) = 0;


	//some get-set functions for annealer options
	virtual
	void
	low_temp( Real const & low_temp ) = 0;

	virtual
	void
	high_temp( Real const & high_temp ) = 0;

	virtual
	void
	disallow_quench( bool const & disallow_quench ) = 0;

	virtual
	Real
	low_temp() const = 0;

	virtual
	Real
	high_temp() const = 0;

	virtual
	bool
	disallow_quench() const = 0;

	virtual
	std::string
	task_string( pose::Pose const & pose ) const = 0;

	virtual
	void remap_residue_level_tasks(
		core::id::SequenceMappingCOP seqmap,
		core::pose::Pose const & pose
	) = 0;

	// stream (I)/O ////////////////////////////////////////////////////

	/// @brief output operator
	friend std::ostream & operator <<(std::ostream & os, PackerTask const & t);

	virtual
	void
	update_commutative(
		PackerTask const & to_copy
	) = 0;

	virtual
	void
	request_symmetrize_by_intersection() = 0;

	virtual
	void
	request_symmetrize_by_union() = 0;

	virtual
	bool
	symmetrize_by_union() const = 0;

	virtual
	bool
	symmetrize_by_intersection() const = 0;

	/// @brief How many threads should the packer request for interaction graph precomputation?
	/// @details Must be implemented by derived class.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	virtual core::Size ig_threads_to_request() const = 0;

	/// @brief Limit the interaction graph setup threads.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitue.org)
	virtual void limit_ig_setup_threads( core::Size const setting ) = 0;

#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

//NOTE: parse_resfile is now an independent function in ResfileReader.hh, not a member function of the PackerTask hierarchy


} //namespace task
} //namespace pack
} //namespace core

#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_pack_task_PackerTask )
#endif // SERIALIZATION


#endif
