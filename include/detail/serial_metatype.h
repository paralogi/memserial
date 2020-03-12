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
constexpr void hashCombine( T& seed, T value ) {
    seed ^= value + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}

/**
 *
 */
template< typename T >
constexpr void hashReduce( T& seed, T value ) {
    seed += value;
}

/**
 *
 */
struct SerialHash {
    uint64_t hash;

    constexpr SerialHash( uint32_t alias_hash, uint32_t type_hash ) :
            hash( ( uint64_t( alias_hash ) << 32 ) + type_hash ) {
    }

    constexpr SerialHash( uint64_t full_hash ) :
            hash( full_hash ) {
    }

    constexpr uint32_t aliasHash() {
        return hash >> 32;
    }

    constexpr uint32_t typeHash() {
        return hash;
    }

    constexpr uint64_t fullHash() {
        return hash;
    }
};

/**
 *
 */
struct SerialAlias {
    char const* data;
    std::size_t size;

    constexpr SerialAlias() :
            data( "" ),
            size( 0 ) {
    }

    template< std::size_t N >
    constexpr SerialAlias( const char ( &str )[ N ] ) :
            data( str ),
            size( N - 1 ) {
    }

    constexpr bool equal( const SerialAlias& str ) const {
        return equal( str.data, str.size );
    }

    constexpr bool equal( const char* str, std::size_t n ) const {
        if ( size != n )
            return false;
        for ( std::size_t i = 0; i < size; ++i ) {
            if ( data[ i ] != str[ i ] )
                return false;
        }
        return true;
    }

    constexpr uint32_t hash() const {
        uint32_t seed = 0;
        for ( std::size_t i = 0; i < size; ++i )
            hashCombine( seed, uint32_t( data[ i ] ) );
        return seed;
    }

    template< typename T >
    constexpr T convert() const {
        return T( data, size );
    }
};

/**
 *
 */
template< typename T, typename I = std::true_type >
struct SerialHelpers {};

/**
 *
 */
template< std::size_t Index >
struct SerialIdentity {
    using SerialTag = std::false_type;
};

template<>
struct SerialIdentity< 0 > {
    using SerialTag = std::true_type;
    using ValueType = nulltype;
};

/**
 *
 */
template< typename T >
struct SerialMetatype {
    static constexpr SerialAlias alias() { return "undefined"; }
    static constexpr SerialHash hash() { return SERIAL_HASH_MAX; }
};

template<>
struct SerialMetatype< nulltype > {
    static constexpr SerialAlias alias() { return "nulltype"; }
    static constexpr SerialHash hash() { return { alias().hash(), SERIAL_HASH_MAX }; }
};

/**
 *
 */
struct SerialOuterTag {};
struct SerialInterTag : SerialOuterTag {};

/**
 *
 */
struct SerialCounter {};

/**
 *
 */
template< typename Tag, std::size_t Base, std::size_t Tail >
constexpr size_t_< Tail > counterReminder( Tag, size_t_< Base >, size_t_< Tail > ) {
    return {};
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = typename std::enable_if< SerialIdentity< Index >::SerialTag::value >::type >
bool searchSerial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    return f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = typename std::enable_if< SerialIdentity< Begin >::SerialTag::value >::type >
bool searchSerial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    if ( searchSerial( f, SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} ) )
        return true;
    return searchSerial( f, SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
bool searchSerial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return false;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = typename std::enable_if< SerialIdentity< Index >::SerialTag::value >::type >
void foreachSerial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = typename std::enable_if< SerialIdentity< Begin >::SerialTag::value >::type >
void foreachSerial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    foreachSerial( f, SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    foreachSerial( f, SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
void foreachSerial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return;
}

/**
 *
 */
template< std::size_t Index,
        typename = typename std::enable_if< SerialIdentity< Index >::SerialTag::value >::type >
std::size_t countSerial( SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    return 1;
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(),
        typename = typename std::enable_if< SerialIdentity< Begin >::SerialTag::value >::type >
std::size_t countSerial( SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    std::size_t begin = countSerial( SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    std::size_t end = countSerial( SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
    return begin + end;
}

template< std::size_t Begin, std::size_t End >
std::size_t countSerial( SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return 0;
}

/**
 *
 */
template< typename Func, std::size_t Begin, std::size_t End >
constexpr bool searchSequence( Func& f, size_t_< Begin >, size_t_< End > ) {
    if ( f( size_t_< Begin >{} ) )
        return true;
    return searchSequence( f, size_t_< Begin + 1 >{}, size_t_< End >{} );
}

template< typename Func, std::size_t Index >
constexpr bool searchSequence( Func& f, size_t_< Index >, size_t_< Index > ) {
    return false;
}

/**
 *
 */
template< typename Func, std::size_t Begin, std::size_t End >
constexpr void foreachSequence( Func& f, size_t_< Begin >, size_t_< End > ) {
    f( size_t_< Begin >{} );
    foreachSequence( f, size_t_< Begin + 1 >{}, size_t_< End >{} );
}

template< typename Func, std::size_t Index >
constexpr void foreachSequence( Func& f, size_t_< Index >, size_t_< Index > ) {
    return;
}

}} // --- namespace
