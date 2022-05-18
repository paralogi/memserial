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
template< SerialEndian endian, typename Iterator >
struct SerialIterator : Iterator {
    using iterator = Iterator;
    static constexpr SerialEndian order = endian;

    SerialIterator( const Iterator& iterator ) :
            Iterator( iterator ) {
    }

    SerialIterator( const typename Iterator::iterator_type& iterator ) :
            Iterator( iterator ) {
    }

    auto operator&() {
        return &( *this )[ 0 ];
    }

    template< typename ValueType >
    void bin( ValueType& value );

    template< typename ValueType >
    void bin( ValueType* value, std::size_t size );

    template< typename ValueType >
    void bout( const ValueType& value );

    template< typename ValueType >
    void bout( const ValueType* value, std::size_t size );
};

/**
 *
 */
template< SerialEndian endian >
struct iterator_traits {
    template< typename ValueType, typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {
        value = reverse_endian( reinterpret_cast< const ValueType& >( *begin ) );
        begin += sizeof( ValueType );
    }

    template< typename ValueType, typename Iterator >
    static void bin( ValueType* value, std::size_t size, Iterator& begin ) {
        for ( std::size_t i = 0; i < size; i++, begin += sizeof( value ) )
            value[ i ] = reverse_endian( reinterpret_cast< const ValueType& >( *begin ) );
        begin += sizeof( ValueType ) * size;
    }

    template< typename ValueType, typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {
        reinterpret_cast< ValueType& >( *begin ) = reverse_endian( value );
        begin += sizeof( ValueType );
    }

    template< typename ValueType, typename Iterator >
    static void bout( const ValueType* value, std::size_t size, Iterator& begin ) {
        for ( std::size_t i = 0; i < size; i++, begin += sizeof( value ) )
            reinterpret_cast< ValueType& >( *begin ) = reverse_endian( value[ i ] );
        begin += sizeof( ValueType ) * size;
    }
};

template<>
struct iterator_traits< NativeEndian > {
    template< typename ValueType, typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {
        value = reinterpret_cast< const ValueType& >( *begin );
        begin += sizeof( ValueType );
    }

    template< typename ValueType, typename Iterator >
    static void bin( ValueType* value, std::size_t size, Iterator& begin ) {
        std::memcpy( value, &begin, sizeof( ValueType ) * size );
        begin += sizeof( ValueType ) * size;
    }

    template< typename ValueType, typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {
        reinterpret_cast< ValueType& >( *begin ) = value;
        begin += sizeof( ValueType );
    }

    template< typename ValueType, typename Iterator >
    static void bout( const ValueType* value, std::size_t size, Iterator& begin ) {
        std::memcpy( &begin, value, sizeof( ValueType ) * size );
        begin += sizeof( ValueType ) * size;
    }
};

/**
 *
 */
template< typename Iterator, typename = std::true_type >
struct rebind_iterator {
    using iterator = Iterator;
};

template<>
struct rebind_iterator< char*, std::true_type > {
    using iterator = typename std::string::iterator;
};

template<>
struct rebind_iterator< const char*, std::true_type > {
    using iterator = typename std::string::const_iterator;
};

/**
 *
 */
template< SerialEndian endian, typename Iterator >
template< typename ValueType >
void SerialIterator< endian, Iterator >::bin( ValueType& value ) {
    constexpr auto internal_endian = endian_traits< endian, sizeof( ValueType ) >::internal_endian;
    iterator_traits< internal_endian >::bin( value, *this );
}

template< SerialEndian endian, typename Iterator >
template< typename ValueType >
void SerialIterator< endian, Iterator >::bin( ValueType* value, std::size_t size ) {
    constexpr auto internal_endian = endian_traits< endian, sizeof( ValueType ) >::internal_endian;
    iterator_traits< internal_endian >::bin( value, size, *this );
}

template< SerialEndian endian, typename Iterator >
template< typename ValueType >
void SerialIterator< endian, Iterator >::bout( const ValueType& value ) {
    constexpr auto internal_endian = endian_traits< endian, sizeof( ValueType ) >::internal_endian;
    iterator_traits< internal_endian >::bout( value, *this );
}

template< SerialEndian endian, typename Iterator >
template< typename ValueType >
void SerialIterator< endian, Iterator >::bout( const ValueType* value, std::size_t size ) {
    constexpr auto internal_endian = endian_traits< endian, sizeof( ValueType ) >::internal_endian;
    iterator_traits< internal_endian >::bout( value, size, *this );
}

template< typename ByteArray >
using SerialIteratorAlias = SerialIterator<
        rebind_endian< ByteArray >::internal_endian,
        typename rebind_iterator< typename ByteArray::iterator >::iterator >;

template< typename ByteArray >
using SerialIteratorConstAlias = SerialIterator<
        rebind_endian< ByteArray >::internal_endian,
        typename rebind_iterator< typename ByteArray::const_iterator >::iterator >;

}} // --- namespace
