// Copyright 2019 Ilya Kiselev.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

namespace memserial {
namespace detail {

/**
 *
 */
struct TypeHashFunctor {
    uint64_t& m_seed;
    uint32_t m_nesting;

    TypeHashFunctor( uint64_t& seed, uint32_t nesting ) :
            m_seed( seed ),
            m_nesting( nesting ) {
    }

    template< typename T >
    void operator()( T& field ) {
        hashCombine( m_seed, SerialHelpers< T >::typeHash( m_nesting ) );
    }
};

/**
 *
 */
struct ByteSizeFunctor {
    std::size_t& m_byte_size;

    ByteSizeFunctor( std::size_t& byte_size ) :
            m_byte_size( byte_size ) {
    }

    template< typename T >
    void operator()( T& field ) {
        m_byte_size += SerialHelpers< T >::byteSize( field );
    }
};

/**
 *
 */
template< typename Iterator >
struct ToBytesFunctor {
    Iterator& m_begin;
    Iterator& m_end;

    ToBytesFunctor( Iterator& begin, Iterator& end ) :
            m_begin( begin ),
            m_end( end ) {
    }

    template< typename T >
    void operator()( T& field ) {
        SerialHelpers< T >::toBytes( field, m_begin, m_end );
    }
};

/**
 *
 */
template< typename Iterator >
struct FromBytesFunctor {
    Iterator& m_begin;
    Iterator& m_end;

    FromBytesFunctor( Iterator& begin, Iterator& end ) :
            m_begin( begin ),
            m_end( end ) {
    }

    template< typename T >
    void operator()( T& field ) {
        SerialHelpers< T >::fromBytes( field, m_begin, m_end );
    }
};

/**
 *
 */
template< typename Stream >
struct ToDebugFunctor {
    Stream& m_stream;
    std::string m_separator;
    uint8_t m_level;

    ToDebugFunctor( Stream& stream, std::string&& separator, uint8_t level ) :
            m_stream( stream ),
            m_separator( std::forward< std::string >( separator ) ),
            m_level( level ) {
    }

    template< typename T >
    void operator()( T& field ) {
        m_stream << m_separator.c_str();
        if ( is_primitive< T >::value )
            m_stream << SerialMetatype< T >::alias().data << ": ";
        SerialHelpers< T >::toDebug( field, m_stream, m_level + 1 );
    }
};

#ifdef OPTIMAL_LIBRARY_SIZE
struct AggregateFunctor {
    template< typename T > struct FunctorType;
    template< uint8_t N > using IntegralType = std::integral_constant< uint8_t, N >;

    enum {
        TypeHash,
        ByteSize,
        ToBytes,
        FromBytes,
        ToOStream,
        ToQDebug,
    };

    uint8_t m_type;
    void* m_functor;

    template< typename T >
    AggregateFunctor( T& functor ) :
            m_type( FunctorType< T >::value ),
            m_functor( &functor ) {
    }

    template< typename T >
    void operator()( T& field ) {
        if ( m_type == TypeHash )
            ( *reinterpret_cast< TypeHashFunctor* >( m_functor ) )( field );
        else if ( m_type == ByteSize )
            ( *reinterpret_cast< ByteSizeFunctor* >( m_functor ) )( field );
        else if ( m_type == ToBytes )
            ( *reinterpret_cast< ToBytesFunctor< std::string::iterator >* >( m_functor ) )( field );
        else if ( m_type == FromBytes )
            ( *reinterpret_cast< FromBytesFunctor< const char* >* >( m_functor ) )( field );
        else if ( m_type == ToOStream )
            ( *reinterpret_cast< ToDebugFunctor< std::ostream >* >( m_functor ) )( field );
#ifdef QT_CORE_LIB
        else if ( m_type == ToQDebug )
            ( *reinterpret_cast< ToDebugFunctor< QDebug >* >( m_functor ) )( field );
#endif
    }
};

template<> struct AggregateFunctor::FunctorType< TypeHashFunctor > : IntegralType< TypeHash > {};
template<> struct AggregateFunctor::FunctorType< ByteSizeFunctor > : IntegralType< ByteSize > {};
template<> struct AggregateFunctor::FunctorType< ToBytesFunctor< std::string::iterator > > : IntegralType< ToBytes > {};
template<> struct AggregateFunctor::FunctorType< FromBytesFunctor< const char* > > : IntegralType< FromBytes > {};
template<> struct AggregateFunctor::FunctorType< ToDebugFunctor< std::ostream > > : IntegralType< ToOStream > {};
#ifdef QT_CORE_LIB
template<> struct AggregateFunctor::FunctorType< ToDebugFunctor< QDebug > > : IntegralType< ToQDebug > {};
#endif
#endif

/**
 *
 */
template< typename T >
struct SerialHelpers< T, is_aggregate< T > > {
    using ValueType = T;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        if ( nesting-- <= 0 )
            return seed;

        ValueType value;
        TypeHashFunctor functor( seed, nesting );

#ifdef OPTIMAL_LIBRARY_SIZE
        boost::pfr::for_each_field( value, AggregateFunctor( functor ) );
#else
        boost::pfr::for_each_field( value, std::move( functor ) );
#endif
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        std::size_t byte_size = 0;
        ValueType& value_ref = const_cast< ValueType& >( value );
        ByteSizeFunctor functor( byte_size );

#ifdef OPTIMAL_LIBRARY_SIZE
        boost::pfr::for_each_field( value_ref, AggregateFunctor( functor ) );
#else
        boost::pfr::for_each_field( value_ref, std::move( functor ) );
#endif
        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        using IteratorType = typename std::remove_reference< Iterator >::type;
        ValueType& value_ref = const_cast< ValueType& >( value );
        ToBytesFunctor< IteratorType > functor( begin, end );

#ifdef OPTIMAL_LIBRARY_SIZE
        boost::pfr::for_each_field( value_ref, AggregateFunctor( functor ) );
#else
        boost::pfr::for_each_field( value_ref, std::move( functor ) );
#endif
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        using IteratorType = typename std::remove_reference< Iterator >::type;
        FromBytesFunctor< IteratorType > functor( begin, end );

#ifdef OPTIMAL_LIBRARY_SIZE
        boost::pfr::for_each_field( value, AggregateFunctor( functor ) );
#else
        boost::pfr::for_each_field( value, std::move( functor ) );
#endif
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data << ": ";

        if ( boost::pfr::tuple_size_v< ValueType > == 0 ) {
            stream << "empty";
            return;
        }

        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        using StreamType = typename std::remove_reference< Stream >::type;
        ValueType& value_ref = const_cast< ValueType& >( value );
        ToDebugFunctor< StreamType > functor( stream, std::move( separator ), level );

#ifdef OPTIMAL_LIBRARY_SIZE
        boost::pfr::for_each_field( value_ref, AggregateFunctor( functor ) );
#else
        boost::pfr::for_each_field( value_ref, std::move( functor ) );
#endif
    }
};

}} // --- namespace
