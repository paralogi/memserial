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

#if !defined( __has_builtin )
#define __has_builtin( x ) 0
#endif

/**
 *
 */
#if defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SERIAL_NATIVE_ENDIAN LittleEndian
#elif defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SERIAL_NATIVE_ENDIAN BigEndian
#elif defined( __BYTE_ORDER__ ) && defined( __ORDER_PDP_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#error The PDP endianness is not supported.
#elif defined( __LITTLE_ENDIAN__ )
#define SERIAL_NATIVE_ENDIAN LittleEndian
#elif defined( __BIG_ENDIAN__ )
#define SERIAL_NATIVE_ENDIAN BigEndian
#elif defined( _MSC_VER ) || defined( __i386__ ) || defined( __x86_64__ )
#define SERIAL_NATIVE_ENDIAN LittleEndian
#else
#error The endianness of this platform could not be determined.
#endif

/**
 *
 */
#if !defined( SERIAL_INTRINSICS_UNDEFINED )
#if defined( _MSC_VER ) && ( !defined( __clang__ ) || defined( __c2__ ) )
#include <cstdlib>
#define SERIAL_INTRINSIC_BSWAP16(x) _byteswap_ushort( x )
#define SERIAL_INTRINSIC_BSWAP32(x) _byteswap_ulong( x )
#define SERIAL_INTRINSIC_BSWAP64(x) _byteswap_uint64( x )
#elif ( defined( __clang__ ) && __has_builtin( __builtin_bswap32 ) && __has_builtin( __builtin_bswap64 ) ) || \
    ( defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 3 ) ) )
#if ( defined( __clang__ ) && __has_builtin( __builtin_bswap16 ) ) || \
    ( defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 8 ) ) )
#define SERIAL_INTRINSIC_BSWAP16( x ) __builtin_bswap16(x)
#else
#define SERIAL_INTRINSIC_BSWAP16( x ) __builtin_bswap32( x << 16 )
#endif
#define SERIAL_INTRINSIC_BSWAP32( x ) __builtin_bswap32( x )
#define SERIAL_INTRINSIC_BSWAP64( x ) __builtin_bswap64( x )
#elif defined(__linux__)
#include <byteswap.h>
#define SERIAL_INTRINSIC_BSWAP16(x) bswap_16(x)
#define SERIAL_INTRINSIC_BSWAP32(x) bswap_32(x)
#define SERIAL_INTRINSIC_BSWAP64(x) bswap_64(x)
#else
#define SERIAL_INTRINSICS_UNDEFINED
#endif
#endif

/**
 *
 */
template< std::size_t N >
struct reversible_traits {};

template<>
struct reversible_traits< 1 > {
    using ReversibleType = uint8_t;

    static uint8_t reverse( uint8_t x ) {
        return x;
    }
};

template<>
struct reversible_traits< 2 > {
    using ReversibleType = uint16_t;

    static uint16_t reverse( uint16_t x ) {
#ifdef SERIAL_INTRINSICS_UNDEFINED
        return x << 8 | x >> 8;
#else
        return SERIAL_INTRINSIC_BSWAP16( x );
#endif
    }
};

template<>
struct reversible_traits< 4 > {
    using ReversibleType = uint32_t;

    static uint32_t reverse( uint32_t x ) {
#ifdef SERIAL_INTRINSICS_UNDEFINED
        uint32_t step16 = x << 16 | x >> 16;
        return ( ( step16 << 8 ) & 0xff00ff00 ) | ( ( step16 >> 8 ) & 0x00ff00ff );
#else
        return SERIAL_INTRINSIC_BSWAP32( x );
#endif
    }
};

template<>
struct reversible_traits< 8 > {
    using ReversibleType = uint64_t;

    static uint64_t reverse( uint64_t x ) {
#ifdef SERIAL_INTRINSICS_UNDEFINED
        uint64_t step32 = x << 32 | x >> 32;
        uint64_t step16 = ( step32 & 0x0000FFFF0000FFFFULL ) << 16 | ( step32 & 0xFFFF0000FFFF0000ULL ) >> 16;
        return ( step16 & 0x00FF00FF00FF00FFULL ) << 8 | ( step16 & 0xFF00FF00FF00FF00ULL ) >> 8;
#else
        return SERIAL_INTRINSIC_BSWAP64( x );
#endif
    }
};

/**
 *
 */
template< class T >
static auto reverse_endian( T x ) -> typename std::enable_if< !std::is_floating_point< T >::value, T >::type {
    using uintN_t = typename reversible_traits< sizeof( T ) >::ReversibleType;
    return static_cast< T >( reversible_traits< sizeof( T ) >::reverse( static_cast< uintN_t >( x ) ) );
}

template< class T >
static auto reverse_endian( T x ) -> typename std::enable_if< std::is_floating_point< T >::value, T >::type {
    using uintN_t = typename reversible_traits< sizeof( T ) >::ReversibleType;
    union {
        T reversed;
        uintN_t reversible;
    } union_cast;
    union_cast.reversed = x;
    union_cast.reversible = reversible_traits< sizeof( T ) >::reverse( union_cast.reversible );
    return union_cast.reversed;
}

}} // --- namespace
