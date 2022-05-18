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
template< typename Stream, typename Iterator >
struct TraceFunctor {
    Stream& stream;
    Iterator& begin;
    uint64_t size;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( serial_hash< ValueType >() != hash )
            return false;
        if ( SerialType< ValueType >::size() > size )
            return true;
        using IteratorType = typename Iterator::iterator;
        constexpr auto serial_order = Iterator::order;
        auto serial_begin_copy = SerialMetatype< ValueType >::template iterator< serial_order >( IteratorType( begin ) );
        auto serial_begin = SerialMetatype< ValueType >::template iterator< serial_order >( IteratorType( begin ) );
        auto serial_end = SerialMetatype< ValueType >::template iterator< serial_order >( IteratorType( begin + size ) );
        ValueType value{};
        SerialType< ValueType >::init( value, serial_begin, serial_end );
        SerialType< ValueType >::bin( value, serial_begin_copy );
        SerialType< ValueType >::debug( value, stream, 0 );
        return true;
    }
};

} // --- namespace

/**
 *
 */
template< typename T, typename Stream >
void print( const T& value, Stream&& stream ) {

    detail::SerialType< T >::debug( value, stream, 0 );
}

/**
 *
 */
template< typename ByteArray, typename Stream >
void trace( const ByteArray& bytes, Stream&& stream ) {

    try {
        if ( detail::SerialType< uint64_t >::size() > bytes.size() )
            return;

        using IteratorType = detail::SerialIteratorConstAlias< ByteArray >;
        IteratorType begin( bytes.begin() );

        uint64_t hash;
        detail::SerialType< uint64_t >::bin( hash, begin );

        using StreamType = typename std::remove_reference< Stream >::type;
        detail::TraceFunctor< StreamType, IteratorType > functor{ stream, begin,
            bytes.size() - detail::SerialType< uint64_t >::size(), hash };
        search_serial( functor );
    }
    catch ( const SerialException& ) {
        return;
    }
}

} // --- namespace

#define SERIAL_PRINT( Type, Stream ) \
template void print< Type, Stream& >( const Type&, Stream& ); \
template void print< Type, Stream >( const Type&, Stream&& );

#define SERIAL_TRACE( ByteArray, Stream ) \
template void trace< ByteArray, Stream& >( const ByteArray&, Stream& ); \
template void trace< SerialWrapper< ByteArray, BigEndian >, Stream& >( const SerialWrapper< ByteArray, BigEndian >&, Stream& ); \
template void trace< SerialWrapper< ByteArray, LittleEndian >, Stream& >( const SerialWrapper< ByteArray, LittleEndian >&, Stream& ); \
template void trace< SerialWrapper< ByteArray, NativeEndian >, Stream& >( const SerialWrapper< ByteArray, NativeEndian >&, Stream& ); \
template void trace< ByteArray, Stream >( const ByteArray&, Stream&& ); \
template void trace< SerialWrapper< ByteArray, BigEndian >, Stream >( const SerialWrapper< ByteArray, BigEndian >&, Stream&& ); \
template void trace< SerialWrapper< ByteArray, LittleEndian >, Stream >( const SerialWrapper< ByteArray, LittleEndian >&, Stream&& ); \
template void trace< SerialWrapper< ByteArray, NativeEndian >, Stream >( const SerialWrapper< ByteArray, NativeEndian >&, Stream&& );
