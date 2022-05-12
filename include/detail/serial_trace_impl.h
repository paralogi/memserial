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
template< typename Stream, typename ByteArray >
struct TraceFunctor {
    using HashIteratorType = detail::SerialIteratorConstAlias< ByteArray >;

    Stream& stream;
    HashIteratorType& begin;
    HashIteratorType& end;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialMetatype< ValueType >::hash().full() != hash )
            return false;
        ValueType value{};
        auto serial_begin = SerialMetatype< ValueType >::template iterator< HashIteratorType::order >( begin );
        auto serial_end = SerialMetatype< ValueType >::template iterator< HashIteratorType::order >( end );
        SerialType< ValueType >::bin( value, serial_begin, serial_end );
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

    using detail::SerialType;
    SerialType< T >::debug( value, stream, 0 );
}

/**
 *
 */
template< typename ByteArray, typename Stream >
void trace( const ByteArray& bytes, Stream&& stream ) {

    using detail::SerialType;
    using detail::TraceFunctor;
    using StreamType = typename std::remove_reference< Stream >::type;
    using HashIteratorType = detail::SerialIteratorConstAlias< ByteArray >;

    try {
        uint64_t hash;
        HashIteratorType hash_begin( bytes.begin() );
        HashIteratorType hash_end( bytes.end() );
        SerialType< uint64_t >::bin( hash, hash_begin, hash_end );

        TraceFunctor< StreamType, ByteArray > functor{ stream, hash_begin, hash_end, hash };
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
