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
constexpr void hash_combine( T& seed, T value ) {
    seed ^= value + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}

/**
 *
 */
template< typename T >
constexpr void hash_reduce( T& seed, T value ) {
    seed += value;
}

/**
 *
 */
struct SerialHash {
    uint64_t value;

    constexpr SerialHash() :
            value( 0 ) {
    }

    constexpr SerialHash( uint32_t head, uint32_t body ) :
            value( ( uint64_t( head ) << 32 ) + body ) {
    }

    constexpr SerialHash( uint64_t full ) :
            value( full ) {
    }

    constexpr uint32_t head() {
        return value >> 32;
    }

    constexpr uint32_t body() {
        return value;
    }

    constexpr uint64_t full() {
        return value;
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
            hash_combine( seed, uint32_t( data[ i ] ) );
        return seed;
    }

    template< typename T >
    constexpr T convert() const {
        return T{ data, size };
    }
};

/**
 *
 */
template< typename T, typename I = std::true_type >
struct SerialType {};

/**
 *
 */
struct SerialCounter {};

template< typename Tag, std::size_t Base, std::size_t Tail >
constexpr size_t_< Tail > counter_reminder( Tag, size_t_< Base >, size_t_< Tail > ) {
    return {};
}

/**
 *
 */
template< std::size_t Index >
struct SerialIdentity {
    using SerialType = std::false_type;
};

template<>
struct SerialIdentity< 0 > {
    using SerialType = std::true_type;
    using ValueType = nulltype;
};

/**
 *
 */
template< typename T >
struct SerialMetatype {
    static constexpr bool serial() { return false; }
    static constexpr SerialAlias alias() { return "undefined"; }
    static constexpr SerialHash hash() { return 0; }

    template< SerialEndian endian, typename Iterator >
    static constexpr auto iterator( const Iterator& i ) {
        return SerialIterator< endian, Iterator >( i );
    }
};

template<>
struct SerialMetatype< nulltype > {
    using IdentityType = SerialIdentity< 0 >;
    static constexpr bool serial() { return true; }
    static constexpr SerialAlias alias() { return "nulltype"; }
    static constexpr SerialHash hash() { return { alias().hash(), SERIAL_HASH_SALT }; }

    template< SerialEndian endian, typename Iterator >
    static constexpr auto iterator( const Iterator& i ) {
        return SerialIterator< endian, Iterator >( i );
    }
};

/**
 *
 */
struct SerialOuterTag {};
struct SerialInterTag : SerialOuterTag {};

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = std::enable_if_t< SerialIdentity< Index >::SerialType::value > >
bool search_serial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    return f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = std::enable_if_t< SerialIdentity< Begin >::SerialType::value > >
bool search_serial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    if ( search_serial( f, SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} ) )
        return true;
    return search_serial( f, SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
bool search_serial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return false;
}

/**
 *
 */
template< std::size_t Index, typename Func,
        typename = std::enable_if_t< SerialIdentity< Index >::SerialType::value > >
void foreach_serial( Func& f, SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    f( size_t_< Index >{} );
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(), typename Func,
        typename = std::enable_if_t< SerialIdentity< Begin >::SerialType::value > >
void foreach_serial( Func& f, SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    foreach_serial( f, SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    foreach_serial( f, SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
}

template< std::size_t Begin, std::size_t End, typename Func >
void foreach_serial( Func&, SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return;
}

/**
 *
 */
template< std::size_t Index,
        typename = std::enable_if_t< SerialIdentity< Index >::SerialType::value > >
std::size_t count_serial( SerialInterTag, size_t_< Index >, size_t_< Index > ) {
    return 1;
}

template< std::size_t Begin = 0, std::size_t End = std::numeric_limits< std::size_t >::max(),
        typename = std::enable_if_t< SerialIdentity< Begin >::SerialType::value > >
std::size_t count_serial( SerialInterTag = {}, size_t_< Begin > = {}, size_t_< End > = {} ) {
    constexpr std::size_t middle = Begin + ( End - Begin ) / 2;
    std::size_t begin = count_serial( SerialInterTag{}, size_t_< Begin >{}, size_t_< middle >{} );
    std::size_t end = count_serial( SerialInterTag{}, size_t_< middle + 1 >{}, size_t_< End >{} );
    return begin + end;
}

template< std::size_t Begin, std::size_t End >
std::size_t count_serial( SerialOuterTag, size_t_< Begin >, size_t_< End > ) {
    return 0;
}

/**
 *
 */
template< typename Func, std::size_t Begin, std::size_t End >
constexpr bool search_sequence( Func& f, size_t_< Begin >, size_t_< End > ) {
    if ( f( size_t_< Begin >{} ) )
        return true;
    return search_sequence( f, size_t_< Begin + 1 >{}, size_t_< End >{} );
}

template< typename Func, std::size_t Index >
constexpr bool search_sequence( Func& f, size_t_< Index >, size_t_< Index > ) {
    return false;
}

/**
 *
 */
template< typename Func, std::size_t Begin, std::size_t End >
constexpr void foreach_sequence( Func& f, size_t_< Begin >, size_t_< End > ) {
    f( size_t_< Begin >{} );
    foreach_sequence( f, size_t_< Begin + 1 >{}, size_t_< End >{} );
}

template< typename Func, std::size_t Index >
constexpr void foreach_sequence( Func& f, size_t_< Index >, size_t_< Index > ) {
    return;
}

}} // --- namespace

#define COUNTER_READ_BASE( Tag, Base, Tail ) \
counter_reminder( Tag{}, size_t_< Base >(), size_t_< Tail >() )

#define COUNTER_READ( Tag ) \
COUNTER_READ_BASE( Tag, 1, \
COUNTER_READ_BASE( Tag, 2, \
COUNTER_READ_BASE( Tag, 4, \
COUNTER_READ_BASE( Tag, 8, \
COUNTER_READ_BASE( Tag, 16, \
COUNTER_READ_BASE( Tag, 32, \
COUNTER_READ_BASE( Tag, 64, \
COUNTER_READ_BASE( Tag, 128, \
COUNTER_READ_BASE( Tag, 256, \
COUNTER_READ_BASE( Tag, 512, \
COUNTER_READ_BASE( Tag, 1024, \
                0 )))))))))))

#define COUNTER_INC( Tag ) \
constexpr size_t_< COUNTER_READ( Tag ) + 1 > \
counter_reminder( Tag&&, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & ~COUNTER_READ( Tag ) >, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & COUNTER_READ( Tag ) > ) \
        { return {}; }

#define SERIAL_TYPE( Type ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialType = std::true_type; \
    using ValueType = Type; \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr bool serial() { return true; } \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
    static constexpr SerialHash hash() { return { alias().hash(), SerialType< Type >::hash() }; } \
    \
    template< SerialEndian endian, typename Iterator > \
    static constexpr auto iterator( const Iterator& i ) { \
        return SerialIterator< endian, Iterator >( i ); \
    } \
}; \
}}

#define SERIAL_METATYPE_ARG0( Type ) \
template<> \
struct SerialMetatype< Type > { \
    static constexpr bool serial() { return false; } \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
};

#define SERIAL_METATYPE_ARG1( Type, ArgType0 ) \
template< ArgType0 Arg > \
struct SerialMetatype< Type< Arg > > { \
    static constexpr bool serial() { return false; } \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
};

#define SERIAL_METATYPE_ARG2( Type, ArgType0, ArgType1 ) \
template< ArgType0 Arg0, ArgType1 Arg1 > \
struct SerialMetatype< Type< Arg0, Arg1 > > { \
    static constexpr bool serial() { return false; } \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
};

#define SERIAL_METATYPE_ARGS( Type ) \
template< typename... Args > \
struct SerialMetatype< Type< Args... > > { \
    static constexpr bool serial() { return false; } \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
};

namespace memserial {
namespace detail {
SERIAL_METATYPE_ARG0( bool )
SERIAL_METATYPE_ARG0( unsigned char )
SERIAL_METATYPE_ARG0( unsigned short )
SERIAL_METATYPE_ARG0( unsigned int )
SERIAL_METATYPE_ARG0( unsigned long )
SERIAL_METATYPE_ARG0( unsigned long long )
SERIAL_METATYPE_ARG0( signed char )
SERIAL_METATYPE_ARG0( short )
SERIAL_METATYPE_ARG0( int )
SERIAL_METATYPE_ARG0( long )
SERIAL_METATYPE_ARG0( long long )
SERIAL_METATYPE_ARG0( char )
SERIAL_METATYPE_ARG0( wchar_t )
SERIAL_METATYPE_ARG0( char16_t )
SERIAL_METATYPE_ARG0( char32_t )
SERIAL_METATYPE_ARG0( float )
SERIAL_METATYPE_ARG0( double )
SERIAL_METATYPE_ARG0( long double )
SERIAL_METATYPE_ARG0( string )
SERIAL_METATYPE_ARG0( wstring )
SERIAL_METATYPE_ARG0( u16string )
SERIAL_METATYPE_ARG0( u32string )
SERIAL_METATYPE_ARG0( system_clock )
SERIAL_METATYPE_ARG0( steady_clock )
SERIAL_METATYPE_ARG0( nanoseconds )
SERIAL_METATYPE_ARG0( microseconds )
SERIAL_METATYPE_ARG0( milliseconds )
SERIAL_METATYPE_ARG0( seconds )
SERIAL_METATYPE_ARG0( minutes )
SERIAL_METATYPE_ARG0( hours )
SERIAL_METATYPE_ARG0( atto )
SERIAL_METATYPE_ARG0( femto )
SERIAL_METATYPE_ARG0( pico )
SERIAL_METATYPE_ARG0( nano )
SERIAL_METATYPE_ARG0( micro )
SERIAL_METATYPE_ARG0( milli )
SERIAL_METATYPE_ARG0( centi )
SERIAL_METATYPE_ARG0( deci )
SERIAL_METATYPE_ARG0( deca )
SERIAL_METATYPE_ARG0( hecto )
SERIAL_METATYPE_ARG0( kilo )
SERIAL_METATYPE_ARG0( mega )
SERIAL_METATYPE_ARG0( giga )
SERIAL_METATYPE_ARG0( tera )
SERIAL_METATYPE_ARG0( peta )
SERIAL_METATYPE_ARG0( exa )
SERIAL_METATYPE_ARGS( basic_string )
SERIAL_METATYPE_ARGS( vector )
SERIAL_METATYPE_ARGS( tuple )
SERIAL_METATYPE_ARG2( array, typename, std::size_t )
SERIAL_METATYPE_ARG1( bitset, std::size_t )
SERIAL_METATYPE_ARG2( time_point, typename, typename )
SERIAL_METATYPE_ARG2( duration, typename, typename )
SERIAL_METATYPE_ARG2( ratio, std::intmax_t, std::intmax_t )
SERIAL_METATYPE_ARG1( complex, typename )
}}
