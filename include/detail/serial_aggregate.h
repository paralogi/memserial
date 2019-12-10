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
template< typename T >
struct SerialHelpers< T, is_aggregate< T > > {
    using ValueType = T;
    static constexpr std::size_t TupleNesting = rebind_aggregate< T >::TupleNesting;
    static constexpr uint64_t TupleSize = rebind_aggregate< T >::TupleSize;

    template< std::size_t Index >
    using TupleField = typename rebind_aggregate< T >::template TupleField< Index >;

    template< std::size_t Index >
    using FieldType = typename TupleField< Index >::FieldType;

    /**
     *
     */
    static constexpr bool matchHash( uint64_t hash ) {

        uint64_t type_hash = SERIAL_HASH_MAX;
        if ( type_hash == hash )
            return true;

        MatchHashFunctor functor{ type_hash, hash, SERIAL_NESTING_MAX };
        return searchSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
    }

    /**
     *
     */
    static constexpr uint64_t typeHash() {

        uint64_t type_hash = SERIAL_HASH_MAX;
        typeHash( type_hash );
        return type_hash;
    }

    static constexpr void typeHash( uint64_t& hash, std::size_t nesting = SERIAL_NESTING_MAX ) {

        if ( nesting == 0 )
            return;

        TypeHashFunctor functor{ hash, nesting };
        foreachSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        ValueType& value_ref = const_cast< ValueType& >( value );
        std::size_t byte_size = 0;

        ByteSizeFunctor functor{ value_ref, byte_size };
        foreachSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        using IteratorType = typename std::remove_reference< Iterator >::type;
        ValueType& value_ref = const_cast< ValueType& >( value );

        ToBytesFunctor< IteratorType > functor{ value_ref, begin, end };
        foreachSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        using IteratorType = typename std::remove_reference< Iterator >::type;

        FromBytesFunctor< IteratorType > functor{ value, begin, end };
        foreachSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data << ": ";
        if ( TupleSize == 0 ) {
            stream << "empty";
            return;
        }

        using StreamType = typename std::remove_reference< Stream >::type;
        ValueType& value_ref = const_cast< ValueType& >( value );
        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        ToDebugFunctor< StreamType > functor{ value_ref, stream, std::move( separator ), level };
        foreachSequence( functor, size_t_< 0 >{}, size_t_< TupleSize >{} );
    }

    /**
     *
     */
    struct MatchHashFunctor {
        uint64_t& type_hash;
        uint64_t hash;
        std::size_t nesting;

        template< std::size_t Index >
        constexpr bool operator()( size_t_< Index > ) {
            SerialHelpers< FieldType< Index > >::typeHash( type_hash, nesting - TupleNesting );
            return type_hash == hash;
        }
    };

    /**
     *
     */
    struct TypeHashFunctor {
        uint64_t& hash;
        std::size_t nesting;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialHelpers< FieldType< Index > >::typeHash( hash, nesting - TupleNesting );
        }
    };

    /**
     *
     */
    struct ByteSizeFunctor {
        ValueType& value;
        std::size_t& byte_size;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            byte_size += SerialHelpers< FieldType< Index > >::byteSize( TupleField< Index >::get( value ) );
        }
    };

    /**
     *
     */
    template< typename Iterator >
    struct ToBytesFunctor {
        ValueType& value;
        Iterator& begin;
        Iterator& end;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialHelpers< FieldType< Index > >::toBytes( TupleField< Index >::get( value ), begin, end );
        }
    };

    /**
     *
     */
    template< typename Iterator >
    struct FromBytesFunctor {
        ValueType& value;
        Iterator& begin;
        Iterator& end;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialHelpers< FieldType< Index > >::fromBytes( TupleField< Index >::get( value ), begin, end );
        }
    };

    /**
     *
     */
    template< typename Stream >
    struct ToDebugFunctor {
        ValueType& value;
        Stream& stream;
        std::string separator;
        uint8_t level;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            stream << separator.c_str();
            if ( is_primitive< FieldType< Index > >::value )
                stream << SerialMetatype< FieldType< Index > >::alias().data << ": ";
            SerialHelpers< FieldType< Index > >::toDebug( TupleField< Index >::get( value ), stream, level + 1 );
        }
    };
};

}} // --- namespace
