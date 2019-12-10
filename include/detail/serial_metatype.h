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

    template< typename T >
    constexpr T convert() const {
        return T( data, size );
    }
};

/**
 *
 */
template< std::size_t Index >
struct SerialIdentity {
    using SerialTag = std::false_type;
    using InternalTag = std::false_type;
};

template<>
struct SerialIdentity< 0 > {
    using SerialTag = std::true_type;
    using InternalTag = std::true_type;
    using ValueType = nulltype;
};

/**
 *
 */
template< typename Type >
struct SerialMetatype {
    static constexpr uint64_t ident() { return SERIAL_HASH_MAX; }
    static constexpr string_view alias() { return "undefined"; }
};

template<>
struct SerialMetatype< nulltype > {
    static constexpr uint64_t ident() { return 0; }
    static constexpr string_view alias() { return "nulltype"; }
};

/**
 *
 */
template< typename T, typename I = std::true_type >
struct SerialHelpers {};

/**
 *
 */
struct SerialOuterTag {};
struct SerialInterTag : SerialOuterTag {};
struct SerialInnerTag : SerialInterTag {};

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
        typename = typename std::enable_if< SerialIdentity< Index >::InternalTag::value >::type >
bool searchSerial( Func&, SerialInnerTag, size_t_< Index >, size_t_< Index > ) {
    return false;
}

template< std::size_t Begin, std::size_t End, typename Func,
        typename = typename std::enable_if< SerialIdentity< End >::InternalTag::value >::type >
bool searchSerial( Func&, SerialInnerTag, size_t_< Begin >, size_t_< End > ) {
    return false;
}

template< std::size_t Index, typename Func,
        typename = typename std::enable_if< SerialIdentity< Index >::SerialTag::value >::type >
bool searchSerial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    return f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = typename std::enable_if< SerialIdentity< Begin >::SerialTag::value >::type >
bool searchSerial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    if ( searchSerial( f, SerialInnerTag{}, size_t_< Begin >{}, size_t_< middle >{} ) )
        return true;
    return searchSerial( f, SerialInnerTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
bool searchSerial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return false;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = typename std::enable_if< SerialIdentity< Index >::InternalTag::value >::type >
void foreachSerial( Func&, SerialInnerTag, size_t_< Index >, size_t_< Index > ) {
    return;
}

template< std::size_t Begin, std::size_t End, typename Func,
        typename = typename std::enable_if< SerialIdentity< End >::InternalTag::value >::type >
void foreachSerial( Func&, SerialInnerTag, size_t_< Begin >, size_t_< End > ) {
    return;
}

template< std::size_t Index, typename Func,
        typename = typename std::enable_if< SerialIdentity< Index >::SerialTag::value >::type >
void foreachSerial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = typename std::enable_if< SerialIdentity< Begin >::SerialTag::value >::type >
void foreachSerial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    foreachSerial( f, SerialInnerTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    foreachSerial( f, SerialInnerTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
void foreachSerial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return;
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

/**
 *
 */
constexpr void hashCombine( uint64_t& seed, uint64_t value ) {
    seed ^= value + 0x9e3779b97f4a7c16 + ( seed << 6 ) + ( seed >> 2 );
}

/**
 *
 */
constexpr void hashReduce( uint64_t& seed, uint64_t value ) {
    seed += value;
}

}} // --- namespace
