#ifndef INCLUDED_ObjexxFCL_FArray6P_hh
#define INCLUDED_ObjexxFCL_FArray6P_hh


// FArray6P: Fortran-Compatible 5D Proxy Array
//
// Project: Objexx Fortran Compatibility Library (ObjexxFCL)
//
// Version: 3.0.0
//
// Language: C++
//
// Copyright (c) 2000-2009 Objexx Engineering, Inc. All Rights Reserved.
// Use of this source code or any derivative of it is restricted by license.
// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com  Objexx@objexx.com


// ObjexxFCL Headers
#include <ObjexxFCL/FArray6P.fwd.hh>
#include <ObjexxFCL/ObserverMulti.hh>
#include <ObjexxFCL/DynamicIndexRange.hh>

#include <ObjexxFCL/FArray6.hh> // AUTO IWYU For FArray6
#include <ObjexxFCL/Star.hh> // AUTO IWYU For star


namespace ObjexxFCL {


/// @brief FArray6P: Fortran-Compatible 5D Proxy Array
template< typename T >
class FArray6P :
	public FArray6< T >,
	public ObserverMulti
{


private: // Types


	typedef  FArray6< T >  Super;
	typedef  typename Super::real_FArray  real_FArray;
	typedef  typename Super::proxy_FArray  proxy_FArray;
	typedef  internal::ProxySentinel  ProxySentinel;


private: // Friend


	friend class FArray6A< T >;


public: // Types


	typedef  typename Super::Base  Base;
	typedef  typename Base::Section  Section;
	typedef  typename Super::IR  SIR;
	typedef  DynamicIndexRange  IR;

	// STL Style
	typedef  typename Base::value_type  value_type;
	typedef  typename Base::reference  reference;
	typedef  typename Base::const_reference  const_reference;
	typedef  typename Base::pointer  pointer;
	typedef  typename Base::const_pointer  const_pointer;
	typedef  typename Base::size_type  size_type;
	typedef  typename Base::difference_type  difference_type;

	// C++ Style
	typedef  typename Base::Value  Value;
	typedef  typename Base::Reference  Reference;
	typedef  typename Base::ConstReference  ConstReference;
	typedef  typename Base::Pointer  Pointer;
	typedef  typename Base::ConstPointer  ConstPointer;
	typedef  typename Base::Size  Size;
	typedef  typename Base::Difference  Difference;

	using Super::array_;
	using Super::array_size_;
#ifdef OBJEXXFCL_PROXY_CONST_CHECKS
	using Super::const_proxy;
	using Super::not_const_proxy;
#endif // OBJEXXFCL_PROXY_CONST_CHECKS
	using Super::npos;
	using Super::sarray_;
	using Super::shift_;
	using Super::shift_set;
	using Super::size_of;
	using Super::size_set;
	using Super::s1_;
	using Super::s2_;
	using Super::s3_;
	using Super::s4_;
	using Super::s5_;


public: // Creation


	/// @brief Default Constructor
	inline
	FArray6P() :
		Super( ProxySentinel() ),
		source_( nullptr )
	{}


	/// @brief Copy Constructor
	inline
	FArray6P( FArray6P const & a ) :
		Super( a, ProxySentinel() ),
		ObserverMulti(),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ ),
		I4_( a.I4_ ),
		I5_( a.I5_ ),
		I6_( a.I6_ ),
		source_( &a )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Non-Const Copy Constructor
	inline
	FArray6P( FArray6P & a ) :
		Super( a, ProxySentinel() ),
		ObserverMulti(),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ ),
		I4_( a.I4_ ),
		I5_( a.I5_ ),
		I6_( a.I6_ ),
		source_( &a )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Real Constructor
	inline
	FArray6P( real_FArray const & a ) :
		Super( a, ProxySentinel() ),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ ),
		I4_( a.I4_ ),
		I5_( a.I5_ ),
		I6_( a.I6_ ),
		source_( &a )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Non-Const Real Constructor
	inline
	FArray6P( real_FArray & a ) :
		Super( a, ProxySentinel() ),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ ),
		I4_( a.I4_ ),
		I5_( a.I5_ ),
		I6_( a.I6_ ),
		source_( &a )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Super Constructor
	inline
	FArray6P( Super const & a ) :
		Super( a, ProxySentinel() ),
		I1_( a.I1() ),
		I2_( a.I2() ),
		I3_( a.I3() ),
		I4_( a.I4() ),
		I5_( a.I5() ),
		I6_( a.I6() ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Non-Const Super Constructor
	inline
	FArray6P( Super & a ) :
		Super( a, ProxySentinel() ),
		I1_( a.I1() ),
		I2_( a.I2() ),
		I3_( a.I3() ),
		I4_( a.I4() ),
		I5_( a.I5() ),
		I6_( a.I6() ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		shift_set( a.shift_ );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		insert_as_observer();
	}


	/// @brief Base Constructor
	inline
	FArray6P( Base const & a ) :
		Super( a, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( a.size() ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Non-Const Base Constructor
	inline
	FArray6P( Base & a ) :
		Super( a, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( a.size() ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Section Constructor
	inline
	FArray6P( Section const & s ) :
		Super( s, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( s.size() ),
		source_( nullptr )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Non-Const Section Constructor
	inline
	FArray6P( Section & s ) :
		Super( s, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( s.size() ),
		source_( nullptr )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Value Constructor
	inline
	FArray6P( T const & t ) :
		Super( t, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( star ), // Unbounded
		source_( nullptr )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Non-Const Value Constructor
	inline
	FArray6P( T & t ) :
		Super( t, ProxySentinel() ),
		I1_( 1 ),
		I2_( 1 ),
		I3_( 1 ),
		I4_( 1 ),
		I5_( 1 ),
		I6_( star ), // Unbounded
		source_( nullptr )
	{
		shift_set( 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		insert_as_observer();
	}


	/// @brief Copy + IndexRange Constructor
	inline
	FArray6P( FArray6P const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( &a )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Copy + IndexRange Constructor
	inline
	FArray6P( FArray6P & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( &a )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Real + IndexRange Constructor
	inline
	FArray6P( real_FArray const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( &a )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Real + IndexRange Constructor
	inline
	FArray6P( real_FArray & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( &a )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Super + IndexRange Constructor
	inline
	FArray6P( Super const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Super + IndexRange Constructor
	inline
	FArray6P( Super & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Base + IndexRange Constructor
	inline
	FArray6P( Base const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Base + IndexRange Constructor
	inline
	FArray6P( Base & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( a, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( dynamic_cast< SubjectMulti const * >( &a ) )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Section + IndexRange Constructor
	inline
	FArray6P( Section const & s, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( s, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( nullptr )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Section + IndexRange Constructor
	inline
	FArray6P( Section & s, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( s, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( nullptr )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Value + IndexRange Constructor
	inline
	FArray6P( T const & t, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( t, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( nullptr )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Non-Const Value + IndexRange Constructor
	inline
	FArray6P( T & t, IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) :
		Super( t, ProxySentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		I4_( I4_a ),
		I5_( I5_a ),
		I6_( I6_a ),
		source_( nullptr )
	{
		dimension_proxy();
		insert_as_observer();
	}


	/// @brief Destructor
	inline
	~FArray6P() override
	{
		if ( source_ ) source_->remove_observer( *this );
	}


public: // Assignment


	/// @brief Copy Assignment
	inline
	FArray6P &
	operator =( FArray6P const & a )
	{
		if ( this != &a ) {
			if ( ! equal_dimension( a ) ) dimension( a );
			Base::operator =( a );
		}
		return *this;
	}


	/// @brief Super Assignment
	inline
	FArray6P &
	operator =( Super const & a )
	{
		if ( this != &a ) {
			if ( ! equal_dimension( a ) ) dimension( a );
			Base::operator =( a );
		}
		return *this;
	}


	/// @brief Super Assignment Template
	template< typename U >
	inline
	FArray6P &
	operator =( FArray6< U > const & a )
	{
		if ( ! equal_dimension( a ) ) dimension( a );
		Base::operator =( a );
		return *this;
	}


	/// @brief += Array Template
	template< typename U >
	inline
	FArray6P &
	operator +=( FArray6< U > const & a )
	{
		Super::operator +=( a );
		return *this;
	}


	/// @brief -= Array Template
	template< typename U >
	inline
	FArray6P &
	operator -=( FArray6< U > const & a )
	{
		Super::operator -=( a );
		return *this;
	}


	/// @brief = Value
	inline
	FArray6P &
	operator =( T const & t )
	{
		Super::operator =( t );
		return *this;
	}


	/// @brief += Value
	inline
	FArray6P &
	operator +=( T const & t )
	{
		Super::operator +=( t );
		return *this;
	}


	/// @brief -= Value
	inline
	FArray6P &
	operator -=( T const & t )
	{
		Super::operator -=( t );
		return *this;
	}


	/// @brief *= Value
	inline
	FArray6P &
	operator *=( T const & t )
	{
		Super::operator *=( t );
		return *this;
	}


	/// @brief /= Value
	inline
	FArray6P &
	operator /=( T const & t )
	{
		Super::operator /=( t );
		return *this;
	}


public: // Subscript


	/// @brief array( i1, i2, i3, i4, i5, i6 ) const
	inline
	T const &
	operator ()( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 ) const
	{
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) && ( I4_.contains( i4 ) ) && ( I5_.contains( i5 ) ) && ( I6_.contains( i6 ) ) );
		return sarray_[ ( ( ( ( ( ( ( ( ( i6 * s5_ ) + i5 ) * s4_ ) + i4 ) * s3_ ) + i3 ) * s2_ ) + i2 ) * s1_ ) + i1 ];
	}


	/// @brief array( i1, i2, i3, i4, i5, i6 )
	inline
	T &
	operator ()( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 )
	{
		proxy_const_assert( not_const_proxy() );
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) && ( I4_.contains( i4 ) ) && ( I5_.contains( i5 ) ) && ( I6_.contains( i6 ) ) );
		return sarray_[ ( ( ( ( ( ( ( ( ( i6 * s5_ ) + i5 ) * s4_ ) + i4 ) * s3_ ) + i3 ) * s2_ ) + i2 ) * s1_ ) + i1 ];
	}


	/// @brief Section Starting at array( i1, i2, i3, i4, i5, i6 )
	inline
	Section const
	a( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 ) const
	{
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) && ( I4_.contains( i4 ) ) && ( I5_.contains( i5 ) ) && ( I6_.contains( i6 ) ) );
		size_type const offset( ( ( ( ( ( ( ( ( ( ( i6 * s5_ ) + i5 ) * s4_ ) + i4 ) * s3_ ) + i3 ) * s2_ ) + i2 ) * s1_ ) + i1 ) - shift_ );
		return Section( static_cast< T const * >( array_ + offset ), ( array_size_ != npos ) ? array_size_ - offset : npos );
	}


	/// @brief Non-Const Section Starting at array( i1, i2, i3, i4, i5, i6 )
	inline
	Section
	a( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 )
	{
		proxy_const_assert( not_const_proxy() );
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) && ( I4_.contains( i4 ) ) && ( I5_.contains( i5 ) ) && ( I6_.contains( i6 ) ) );
		size_type const offset( ( ( ( ( ( ( ( ( ( ( i6 * s5_ ) + i5 ) * s4_ ) + i4 ) * s3_ ) + i3 ) * s2_ ) + i2 ) * s1_ ) + i1 ) - shift_ );
		return Section( array_ + offset, ( array_size_ != npos ) ? array_size_ - offset : npos );
	}


	/// @brief Linear Index
	inline
	size_type
	index( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 ) const
	{
		assert( ( ( I1_.initialized() ) && ( I2_.initialized() ) && ( I3_.initialized() ) && ( I4_.initialized() ) && ( I5_.initialized() ) && ( I6_.initialized() ) ) );
		return ( ( ( ( ( ( ( ( ( ( ( i6 * s5_ ) + i5 ) * s4_ ) + i4 ) * s3_ ) + i3 ) * s2_ ) + i2 ) * s1_ ) + i1 ) - shift_ );
	}


public: // Predicate


	/// @brief Dimensions Initialized?
	inline
	bool
	dimensions_initialized() const
	{
		return ( ( I1_.initialized() ) && ( I2_.initialized() ) && ( I3_.initialized() ) && ( I4_.initialized() ) && ( I5_.initialized() ) && ( I6_.initialized() ) );
	}


	/// @brief Contains Indexed Element?
	inline
	bool
	contains( int const i1, int const i2, int const i3, int const i4, int const i5, int const i6 ) const
	{
		return ( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) && ( I4_.contains( i4 ) ) && ( I5_.contains( i5 ) ) && ( I6_.contains( i6 ) ) );
	}


	/// @brief Initializer Active?
	inline
	bool
	initializer_active() const
	{
		return false;
	}


public: // Inspector


	/// @brief IndexRange of Dimension 1
	inline
	IR const &
	I1() const
	{
		return I1_;
	}


	/// @brief Lower Index of Dimension 1
	inline
	int
	l1() const
	{
		return I1_.l();
	}


	/// @brief Upper Index of Dimension 1
	inline
	int
	u1() const
	{
		return I1_.u();
	}


	/// @brief IndexRange of Dimension 2
	inline
	IR const &
	I2() const
	{
		return I2_;
	}


	/// @brief Lower Index of Dimension 2
	inline
	int
	l2() const
	{
		return I2_.l();
	}


	/// @brief Upper Index of Dimension 2
	inline
	int
	u2() const
	{
		return I2_.u();
	}


	/// @brief IndexRange of Dimension 3
	inline
	IR const &
	I3() const
	{
		return I3_;
	}


	/// @brief Lower Index of Dimension 3
	inline
	int
	l3() const
	{
		return I3_.l();
	}


	/// @brief Upper Index of Dimension 3
	inline
	int
	u3() const
	{
		return I3_.u();
	}


	/// @brief IndexRange of Dimension 4
	inline
	IR const &
	I4() const
	{
		return I4_;
	}


	/// @brief Lower Index of Dimension 4
	inline
	int
	l4() const
	{
		return I4_.l();
	}


	/// @brief Upper Index of Dimension 4
	inline
	int
	u4() const
	{
		return I4_.u();
	}


	/// @brief IndexRange of Dimension 5
	inline
	IR const &
	I5() const
	{
		return I5_;
	}


	/// @brief Lower Index of Dimension 5
	inline
	int
	l5() const
	{
		return I5_.l();
	}


	/// @brief Upper Index of Dimension 5
	inline
	int
	u5() const
	{
		return I5_.u();
	}


	/// @brief IndexRange of Dimension 6
	inline
	IR const &
	I6() const
	{
		return I6_;
	}


	/// @brief Lower Index of Dimension 6
	inline
	int
	l6() const
	{
		return I6_.l();
	}


	/// @brief Upper Index of Dimension 6
	inline
	int
	u6() const
	{
		return I6_.u();
	}


	/// @brief Size of Dimension 6
	inline
	size_type
	size6() const
	{
		return I6_.size();
	}


public: // Modifier


	/// @brief Clear
	inline
	FArray6P &
	clear()
	{
		Super::clear();
		I1_.clear_no_notify();
		I2_.clear_no_notify();
		I3_.clear_no_notify();
		I4_.clear_no_notify();
		I5_.clear_no_notify();
		I6_.clear_no_notify();
		source_ = 0;
		return *this;
	}


	/// @brief Dimension by IndexRanges Even if Const
	inline
	FArray6P const &
	dim( IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a ) const
	{
		const_cast< FArray6P & >( *this ).dimension( I1_a, I2_a, I3_a, I4_a, I5_a, I6_a );
		return *this;
	}


	/// @brief Dimension by Array Even if Const
	template< typename U >
	inline
	FArray6P const &
	dim( FArray6< U > const & a ) const
	{
		const_cast< FArray6P & >( *this ).dimension( a );
		return *this;
	}


	/// @brief Dimension by IndexRanges
	inline
	FArray6P &
	dimension( IR const & I1_a, IR const & I2_a, IR const & I3_a, IR const & I4_a, IR const & I5_a, IR const & I6_a )
	{
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		I4_.assign_no_notify( I4_a );
		I5_.assign_no_notify( I5_a );
		I6_.assign_no_notify( I6_a );
		dimension_proxy();
		return *this;
	}


	/// @brief Dimension by Array
	template< typename U >
	inline
	FArray6P &
	dimension( FArray6< U > const & a )
	{
		I1_.assign_no_notify( a.I1() );
		I2_.assign_no_notify( a.I2() );
		I3_.assign_no_notify( a.I3() );
		I4_.assign_no_notify( a.I4() );
		I5_.assign_no_notify( a.I5() );
		I6_.assign_no_notify( a.I6() );
		dimension_proxy();
		return *this;
	}


	/// @brief Attach to Proxy Array
	inline
	FArray6P &
	attach( FArray6P const & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1_;
		I2_ = a.I2_;
		I3_ = a.I3_;
		I4_ = a.I4_;
		I5_ = a.I5_;
		I6_ = a.I6_;
		if ( source_ ) source_->remove_observer( *this );
		source_ = &a;
		a.insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Non-Const Proxy Array
	inline
	FArray6P &
	attach( FArray6P & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1_;
		I2_ = a.I2_;
		I3_ = a.I3_;
		I4_ = a.I4_;
		I5_ = a.I5_;
		I6_ = a.I6_;
		if ( source_ ) source_->remove_observer( *this );
		source_ = &a;
		a.insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Real Array
	inline
	FArray6P &
	attach( real_FArray const & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1_;
		I2_ = a.I2_;
		I3_ = a.I3_;
		I4_ = a.I4_;
		I5_ = a.I5_;
		I6_ = a.I6_;
		if ( source_ ) source_->remove_observer( *this );
		source_ = &a;
		a.insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Non-Const Real Array
	inline
	FArray6P &
	attach( real_FArray & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1_;
		I2_ = a.I2_;
		I3_ = a.I3_;
		I4_ = a.I4_;
		I5_ = a.I5_;
		I6_ = a.I6_;
		if ( source_ ) source_->remove_observer( *this );
		source_ = &a;
		a.insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Super Array
	inline
	FArray6P &
	attach( Super const & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1();
		I2_ = a.I2();
		I3_ = a.I3();
		I4_ = a.I4();
		I5_ = a.I5();
		I6_ = a.I6();
		if ( source_ ) source_->remove_observer( *this );
		source_ = dynamic_cast< SubjectMulti const * >( &a );
		if ( source_ ) source_->insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Non-Const Super Array
	inline
	FArray6P &
	attach( Super & a )
	{
		Base::attach( a );
		s1_ = a.s1_;
		s2_ = a.s2_;
		s3_ = a.s3_;
		s4_ = a.s4_;
		s5_ = a.s5_;
		I1_ = a.I1();
		I2_ = a.I2();
		I3_ = a.I3();
		I4_ = a.I4();
		I5_ = a.I5();
		I6_ = a.I6();
		if ( source_ ) source_->remove_observer( *this );
		source_ = dynamic_cast< SubjectMulti const * >( &a );
		if ( source_ ) source_->insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Base Array
	inline
	FArray6P &
	attach( Base const & a )
	{
		Base::attach( a, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = a.size();
		if ( source_ ) source_->remove_observer( *this );
		source_ = dynamic_cast< SubjectMulti const * >( &a );
		if ( source_ ) source_->insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Non-Const Base Array
	inline
	FArray6P &
	attach( Base & a )
	{
		Base::attach( a, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = a.size();
		if ( source_ ) source_->remove_observer( *this );
		source_ = dynamic_cast< SubjectMulti const * >( &a );
		if ( source_ ) source_->insert_observer( *this );
		return *this;
	}


	/// @brief Attach to Section
	inline
	FArray6P &
	attach( Section const & s )
	{
		Base::attach( s, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = s.size();
		if ( source_ ) source_->remove_observer( *this );
		source_ = 0;
		return *this;
	}


	/// @brief Attach to Non-Const Section
	inline
	FArray6P &
	attach( Section & s )
	{
		Base::attach( s, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = s.size();
		if ( source_ ) source_->remove_observer( *this );
		source_ = 0;
		return *this;
	}


	/// @brief Attach to Value
	inline
	FArray6P &
	attach( T const & t )
	{
		Base::attach( t, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = star; // Unbounded
		if ( source_ ) source_->remove_observer( *this );
		source_ = 0;
		return *this;
	}


	/// @brief Attach to Non-Const Value
	inline
	FArray6P &
	attach( T & t )
	{
		Base::attach( t, 5 );
		s1_ = s2_ = s3_ = s4_ = s5_ = 1;
		I1_ = 1;
		I2_ = 1;
		I3_ = 1;
		I4_ = 1;
		I5_ = 1;
		I6_ = star; // Unbounded
		if ( source_ ) source_->remove_observer( *this );
		source_ = 0;
		return *this;
	}


	/// @brief Detach from Source Array
	inline
	FArray6P &
	detach()
	{
		Base::detach();
		s1_ = s2_ = s3_ = s4_ = s5_ = 0;
		I1_.clear();
		I2_.clear();
		I3_.clear();
		I4_.clear();
		I5_.clear();
		I6_.clear();
		if ( source_ ) source_->remove_observer( *this );
		source_ = 0;
		return *this;
	}


public: // Observer Modifier


	/// @brief Update
	inline
	void
	update() override
	{
#ifdef OBJEXXFCL_PROXY_CONST_CHECKS
		if ( source_ ) {
			if ( const_proxy() ) {
				update_to( *dynamic_cast< Base const * >( source_ ) );
			} else {
				update_to( *dynamic_cast< Base * >( const_cast< SubjectMulti * >( source_ ) ) );
			}
		}
#else
		if ( source_ ) update_to( *dynamic_cast< Base const * >( source_ ) );
#endif // OBJEXXFCL_PROXY_CONST_CHECKS
		dimension_proxy();
	}


	/// @brief Update for Destruction of a Subject
	inline
	void
	destructed( Subject const & subject ) override
	{
		if ( ( source_ ) && ( &subject == source_ ) ) { // Source array is being destructed
			Base::detach();
			s1_ = s2_ = s3_ = s4_ = s5_ = 0;
			I1_.clear();
			I2_.clear();
			I3_.clear();
			I4_.clear();
			I5_.clear();
			I6_.clear();
			source_ = 0;
		}
	}


protected: // Functions


	/// @brief Dimension by IndexRanges
	inline
	void
	dimension_assign( SIR const & I1_a, SIR const & I2_a, SIR const & I3_a, SIR const & I4_a, SIR const & I5_a, SIR const & I6_a )
	{
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		I4_.assign_no_notify( I4_a );
		I5_.assign_no_notify( I5_a );
		I6_.assign_no_notify( I6_a );
		dimension_proxy();
	}


private: // Functions


	/// @brief Dimension by Current IndexRanges
	inline
	void
	dimension_proxy()
	{
		assert( I1_.not_unbounded() );
		assert( I2_.not_unbounded() );
		assert( I3_.not_unbounded() );
		assert( I4_.not_unbounded() );
		assert( I5_.not_unbounded() );
		s1_ = I1_.size();
		s2_ = I2_.size();
		s3_ = I3_.size();
		s4_ = I4_.size();
		s5_ = I5_.size();
		if ( dimensions_initialized() ) {
			if ( I6_.bounded_value() ) { // Bounded
				size_set( size_of( s1_, s2_, s3_, s4_, s5_, I6_.size() ) );
			} else if ( array_size_ != npos ) { // Unbounded with bounded data array
				size_type const slice_size( size_of( s1_, s2_, s3_, s4_, s5_ ) );
				if ( slice_size > 0 ) { // Infer upper index and size
					I6_.u_no_notify( I6_.lz() + ( array_size_ / slice_size ) - 1 );
					size_set( size_of( slice_size, I6_.size() ) );
				} else {
					size_set( array_size_ );
				}
			} else { // Unbounded with unbounded data array
				size_set( npos );
			}
			shift_set( ( ( ( ( ( ( ( ( ( I6_.lz() * s5_ ) + I5_.lz() ) * s4_ ) +  I4_.lz() ) * s3_ ) + I3_.lz() ) * s2_ ) + I2_.lz() ) * s1_ ) + I1_.lz() );
		} else {
			size_set( 0 );
			shift_set( 0 );
		}
	}


	/// @brief Insert as Observer of the IndexRanges and Source Array
	inline
	void
	insert_as_observer()
	{
		I1_.insert_observer( *this );
		I2_.insert_observer( *this );
		I3_.insert_observer( *this );
		I4_.insert_observer( *this );
		I5_.insert_observer( *this );
		I6_.insert_observer( *this );
		if ( source_ ) source_->insert_observer( *this );
	}

	/* // Unused private
	/// @brief Remove as Observer of the IndexRanges and Source Array
	inline
	void
	remove_as_observer()
	{
	I1_.remove_observer( *this );
	I2_.remove_observer( *this );
	I3_.remove_observer( *this );
	I4_.remove_observer( *this );
	I5_.remove_observer( *this );
	I6_.remove_observer( *this );
	if ( source_ ) source_->remove_observer( *this );
	}*/


private: // Data


	/// @brief Dimension 1 index range
	IR I1_;

	/// @brief Dimension 2 index range
	IR I2_;

	/// @brief Dimension 3 index range
	IR I3_;

	/// @brief Dimension 4 index range
	IR I4_;

	/// @brief Dimension 5 index range
	IR I5_;

	/// @brief Dimension 6 index range
	IR I6_;

	/// @brief Pointer (non-owning) to source array (0 if unknown)
	SubjectMulti const * source_;


}; // FArray6P


} // namespace ObjexxFCL


#endif // INCLUDED_ObjexxFCL_FArray6P_HH
