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
struct string_view {
    char const* data;
    std::size_t size;

    constexpr string_view() :
            data( "" ),
            size( 0 ) {
    }

    template< std::size_t N >
    constexpr string_view( const char ( &str )[ N ] ) :
            data( str ),
            size( N - 1 ) {
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

    std::string string() const {
        return { data, size };
    }
};

/**
 *
 */
struct SerialCounter {};

/**
 *
 */
template< std::size_t Index >
struct SerialIdentity {};

template<>
struct SerialIdentity< 0 > {
    static constexpr nulltype value() { return {}; }
    static constexpr string_view alias() { return "nulltype"; }
};

/**
 *
 */
template< typename Type >
struct SerialMetatype {
    static constexpr uint64_t ident() { return -1; }
    static constexpr string_view alias() { return "undefined"; }
};

template< typename Type >
struct SerialMetatype< enum_t< Type > > {
    using UnderlyingType = typename std::underlying_type< Type >::type;
    static constexpr uint64_t ident() { return SerialMetatype< UnderlyingType >::ident(); }
    static constexpr string_view alias() { return SerialMetatype< UnderlyingType >::alias(); }
};

template<>
struct SerialMetatype< nulltype > {
    static constexpr uint64_t ident() { return 0; }
    static constexpr string_view alias() { return "nulltype"; }
};

/**
 *
 */
struct SerialOuterTag {};
struct SerialInterTag : SerialOuterTag {};
struct SerialInnerTag : SerialInterTag {};

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
template< std::size_t Index,
        typename = decltype( SerialIdentity< Index >::alias() ) >
constexpr uint64_t countSerial( SerialInnerTag, size_t_< Index >, size_t_< Index > ) {
    return Index;
}

template< std::size_t Begin, std::size_t End,
        typename = decltype( SerialIdentity< End >::alias() ) >
constexpr uint64_t countSerial( SerialInnerTag, size_t_< Begin >, size_t_< End > ) {
    return End;
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(),
        typename = decltype( SerialIdentity< Begin >::alias() ) >
constexpr uint64_t countSerial( SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    constexpr uint64_t begin = countSerial( SerialInnerTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    constexpr uint64_t end = countSerial( SerialInnerTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
    return std::max( begin, end );
}

template< std::size_t Begin, std::size_t End >
constexpr uint64_t countSerial( SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return 0;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = decltype( SerialIdentity< Index >::value() ) >
constexpr bool invokeIfSerial( Func&& f, long ) {
    return f( SerialIdentity< Index >::value() );
}

template< std::size_t Index, typename Func >
constexpr bool invokeIfSerial( Func&&, int ) {
    return false;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = decltype( SerialIdentity< Index >::alias() ) >
void foreachSerial( Func&& f, SerialInnerTag, size_t_< Index >, size_t_< Index > ) {
    invokeIfSerial< Index >( std::forward< Func >( f ), 1L );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = decltype( SerialIdentity< Begin >::alias() ) >
void foreachSerial( Func&& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    foreachSerial( std::forward< Func >( f ), SerialInnerTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    foreachSerial( std::forward< Func >( f ), SerialInnerTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
void foreachSerial( Func&& f, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = decltype( SerialIdentity< Index >::alias() ) >
bool searchSerial( Func&& f, SerialInnerTag, size_t_< Index >, size_t_< Index > ) {
    return invokeIfSerial< Index >( std::forward< Func >( f ), 1L );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = decltype( SerialIdentity< Begin >::alias() ) >
bool searchSerial( Func&& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    if ( searchSerial( std::forward< Func >( f ), SerialInnerTag{}, size_t_< Begin >{}, size_t_< middle >{} ) )
        return true;
    return searchSerial( std::forward< Func >( f ), SerialInnerTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
bool searchSerial( Func&& f, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return false;
}

/**
 *
 */
template< typename SizeT >
constexpr void hashCombine( SizeT& seed, SizeT value ) {
    seed ^= value + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}

template< typename Type >
constexpr uint64_t hash() {
    uint64_t seed = SerialMetatype< Type >::ident();
    hashCombine( seed, uint64_t( sizeof( Type ) ) );
    return seed;
}

}} // --- namespace
