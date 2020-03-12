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

#if !defined( DISABLE_STATIC_HASH )
extern const uint64_t ReducedHashStatic;
extern const std::vector< uint64_t > PerfectHashStatic;
#endif

/**
 *
 */
struct MatchFunctor {
    bool& match_result;
    uint32_t match_hash;
    uint32_t value_hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialHelpers< ValueType >::typeHash() != value_hash )
            return false;
        match_result = SerialHelpers< ValueType >::matchHash( match_hash );
        return true;
    }
};

/**
 *
 */
template< typename ByteArray >
struct HashFunctor {
    uint64_t& hash;
    const ByteArray& alias;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( !SerialMetatype< ValueType >::alias().equal( alias.data(), alias.size() ) )
            return false;
        hash = SerialMetatype< ValueType >::hash().fullHash();
        return true;
    }
};

/**
 *
 */
template< typename ByteArray >
struct AliasFunctor {
    ByteArray& alias;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialMetatype< ValueType >::hash().fullHash() != hash )
            return false;
        SerialAlias type_alias = SerialMetatype< ValueType >::alias();
        alias = type_alias.convert< ByteArray >();
        return true;
    }
};

/**
 *
 */
template< typename Stream, typename ByteArray >
struct PrintFunctor {
    Stream& stream;
    const ByteArray& bytes;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialMetatype< ValueType >::hash().fullHash() != hash )
            return false;
        print( stream, parse< ValueType >( bytes ) );
        return true;
    }
};

} // --- namespace

/**
 *
 */
uint64_t serialVersion() {

    using detail::reducedHash;
    return reducedHash();
}

/**
 *
 */
bool checkVersion() {

#if !defined( DISABLE_STATIC_HASH )
    using detail::reducedHash;
    using detail::ReducedHashStatic;
    return reducedHash() == ReducedHashStatic;
#else
    return true;
#endif
}

/**
 *
 */
template< typename T >
bool checkVersion() {

#if !defined( DISABLE_STATIC_HASH )
    using detail::PerfectHashStatic;
    using detail::SerialMetatype;

    uint64_t hash = SerialMetatype< T >::hash().fullHash();
    auto result = std::find( begin( PerfectHashStatic ), end( PerfectHashStatic ), hash );
    return result != std::end( PerfectHashStatic );
#else
    return true;
#endif
}

/**
 *
 */
template< typename ByteArray, typename T >
ByteArray serialize( const T& value ) {

    assert( checkVersion< T >() );

    using detail::SerialMetatype;
    using detail::SerialHelpers;

    uint64_t hash = SerialMetatype< T >::hash().fullHash();
    std::size_t size = SerialHelpers< T >::byteSize( value ) + sizeof( uint64_t );

    ByteArray bytes;
    bytes.resize( size );
    auto begin = bytes.begin();
    auto end = bytes.end();

    SerialHelpers< uint64_t >::toBytes( hash, begin, end );
    SerialHelpers< T >::toBytes( value, begin, end );
    return bytes;
}

/**
 *
 */
template< typename T, typename ByteArray >
T parse( const ByteArray& bytes ) {

    assert( checkVersion< T >() );

    using detail::SerialMetatype;
    using detail::SerialHelpers;
    using detail::SerialHash;
    using detail::MatchFunctor;

    uint64_t hash;
    auto begin = bytes.begin();
    auto end = bytes.end();
    SerialHelpers< uint64_t >::fromBytes( hash, begin, end );

    SerialHash match_hash = SerialMetatype< T >::hash();
    SerialHash value_hash( hash );

    if ( match_hash.aliasHash() != value_hash.aliasHash() ) {
        bool match_result = false;
        MatchFunctor functor{ match_result, match_hash.typeHash(), value_hash.typeHash() };
        searchSerial( functor );
        if ( !match_result )
            throw SerialException( SerialException::ExcTypeMissmatch );
    }

    else if ( match_hash.typeHash() != value_hash.typeHash() ) {
        throw SerialException( SerialException::ExcBinaryIncompatible );
    }

    T value;
    SerialHelpers< T >::fromBytes( value, begin, end );
    return value;
}

/**
 *
 */
template< typename T >
uint64_t ident() {

    assert( checkVersion< T >() );

    using detail::SerialMetatype;
    using detail::SerialHash;

    SerialHash hash = SerialMetatype< T >::hash();
    return hash.fullHash();
}

/**
 *
 */
template< typename ByteArray >
uint64_t ident( const ByteArray& alias ) {

    using detail::HashFunctor;

    uint64_t hash = -1;
    HashFunctor< ByteArray > functor{ hash, alias };
    searchSerial( functor );
    return hash;
}

/**
 *
 */
template< typename T, typename ByteArray >
ByteArray alias() {

    assert( checkVersion< T >() );

    using detail::SerialMetatype;
    using detail::SerialAlias;

    SerialAlias alias = SerialMetatype< T >::alias();
    return alias.convert< ByteArray >();
}

/**
 *
 */
template< typename ByteArray >
ByteArray alias( uint64_t ident ) {

    using detail::AliasFunctor;

    ByteArray alias;
    AliasFunctor< ByteArray > functor{ alias, ident };
    searchSerial( functor );
    return alias;
}

/**
 *
 */
template< typename Stream, typename T >
void print( Stream&& stream, const T& value ) {

    assert( checkVersion< T >() );

    using detail::SerialHelpers;
    SerialHelpers< T >::toDebug( value, stream, 0 );
}

/**
 *
 */
template< typename Stream, typename ByteArray >
void trace( Stream&& stream, const ByteArray& bytes ) {

    using detail::SerialHelpers;
    using detail::PrintFunctor;
    using StreamType = typename std::remove_reference< Stream >::type;

    try {
        uint64_t hash;
        auto begin = bytes.begin();
        auto end = bytes.end();
        SerialHelpers< uint64_t >::fromBytes( hash, begin, end );

        PrintFunctor< StreamType, ByteArray > functor{ stream, bytes, hash };
        searchSerial( functor );
    }
    catch ( const SerialException& ) {
        return;
    }
}

using detail::nulltype;
template std::string serialize< std::string, nulltype >( const nulltype& );
template nulltype parse< nulltype, std::string >( const std::string& );
template uint64_t ident< nulltype >();
template uint64_t ident< std::string >( const std::string& alias );
template std::string alias< nulltype, std::string >();
template std::string alias< std::string >( uint64_t );
template void print< std::ostream&, nulltype >( std::ostream&, const nulltype& );
template void print< std::ostream, nulltype >( std::ostream&&, const nulltype& );
template void trace< std::ostream&, std::string >( std::ostream&, const std::string& );
template void trace< std::ostream, std::string >( std::ostream&&, const std::string& );

#if defined( QT_CORE_LIB )

template QByteArray serialize< QByteArray, nulltype >( const nulltype& );
template nulltype parse< nulltype, QByteArray >( const QByteArray& );
template uint64_t ident< QByteArray >( const QByteArray& alias );
template QByteArray alias< nulltype, QByteArray >();
template QByteArray alias< QByteArray >( uint64_t );
template void print< QDebug&, nulltype >( QDebug&, const nulltype& );
template void print< QDebug, nulltype >( QDebug&&, const nulltype& );
template void trace< std::ostream&, QByteArray >( std::ostream&, const QByteArray& );
template void trace< std::ostream, QByteArray >( std::ostream&&, const QByteArray& );
template void trace< QDebug&, std::string >( QDebug&, const std::string& );
template void trace< QDebug, std::string >( QDebug&&, const std::string& );
template void trace< QDebug&, QByteArray >( QDebug&, const QByteArray& );
template void trace< QDebug, QByteArray >( QDebug&&, const QByteArray& );

QDebug operator<<( QDebug dbg, long double value ) {
    dbg << double( value );
    return dbg.maybeSpace();
}

#endif

} // --- namespace
