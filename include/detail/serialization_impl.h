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
struct MatchFunctor {
    bool& match_result;
    uint32_t match_hash;
    uint32_t value_hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialType< ValueType >::hash() != value_hash )
            return false;
        match_result = SerialType< ValueType >::match( match_hash );
        return true;
    }
};

/**
 *
 */
struct HashFunctor {
    uint64_t& hash;
    const std::string& alias;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( !SerialMetatype< ValueType >::alias().equal( alias.data(), alias.size() ) )
            return false;
        hash = SerialMetatype< ValueType >::hash().full();
        return true;
    }
};

/**
 *
 */
struct AliasFunctor {
    std::string& alias;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialMetatype< ValueType >::hash().full() != hash )
            return false;
        alias = SerialAlias{ SerialMetatype< ValueType >::alias() }.convert< std::string >();
        return true;
    }
};

} // --- namespace

/**
 *
 */
template< typename ByteArray, typename T >
ByteArray serialize( const T& value ) {

    using detail::SerialMetatype;
    using detail::SerialType;

    ByteArray bytes;
    bytes.resize( SerialType< T >::size( value ) + sizeof( uint64_t ) );

    using HashIteratorType = detail::SerialIteratorAlias< ByteArray >;
    HashIteratorType hash_begin( bytes.begin() );
    HashIteratorType hash_end( bytes.end() );

    SerialType< uint64_t >::bout( SerialMetatype< T >::hash().full(), hash_begin, hash_end );

    auto serial_begin = SerialMetatype< T >::template iterator< ByteArray >( hash_begin );
    auto serial_end = SerialMetatype< T >::template iterator< ByteArray >( hash_end );

    SerialType< T >::bout( value, serial_begin, serial_end );
    return bytes;
}

/**
 *
 */
template< typename T, typename ByteArray >
T parse( const ByteArray& bytes ) {

    using detail::SerialMetatype;
    using detail::SerialType;
    using detail::SerialHash;
    using detail::MatchFunctor;

    using HashIteratorType = detail::SerialIteratorConstAlias< ByteArray >;
    HashIteratorType hash_begin( bytes.begin() );
    HashIteratorType hash_end( bytes.end() );

    uint64_t hash;
    SerialType< uint64_t >::bin( hash, hash_begin, hash_end );

    if ( SerialMetatype< T >::hash().head() != SerialHash( hash ).head() ) {
        bool match_result = false;
        MatchFunctor functor{ match_result, SerialMetatype< T >::hash().body(), SerialHash( hash ).body() };
        search_serial( functor );
        if ( !match_result )
            throw SerialException( SerialException::ExcTypeMissmatch );
    }

    else if ( SerialMetatype< T >::hash().body() != SerialHash( hash ).body() ) {
        throw SerialException( SerialException::ExcBinaryIncompatible );
    }

    auto serial_begin = SerialMetatype< T >::template iterator< ByteArray >( hash_begin );
    auto serial_end = SerialMetatype< T >::template iterator< ByteArray >( hash_end );

    T value;
    SerialType< T >::bin( value, serial_begin, serial_end );
    return value;
}

/**
 *
 */
template< typename T >
uint64_t ident() {

    using detail::SerialMetatype;
    return SerialMetatype< T >::hash().full();
}

/**
 *
 */
uint64_t ident( const std::string& alias ) {

    using detail::HashFunctor;
    uint64_t hash = -1;
    HashFunctor functor{ hash, alias };
    search_serial( functor );
    return hash;
}

/**
 *
 */
template< typename T >
std::string alias() {

    using detail::SerialMetatype;
    using detail::SerialAlias;
    return SerialAlias{ SerialMetatype< T >::alias() }.convert< std::string >();
}

/**
 *
 */
std::string alias( uint64_t ident ) {

    using detail::AliasFunctor;
    std::string alias;
    AliasFunctor functor{ alias, ident };
    search_serial( functor );
    return alias;
}

} // --- namespace

#define SERIALIALIZE( Type, ByteArray ) \
template ByteArray serialize< ByteArray, Type >( const Type& ); \
template SerialStorage< ByteArray, BigEndian > serialize< SerialStorage< ByteArray, BigEndian >, Type >( const Type& ); \
template SerialStorage< ByteArray, LittleEndian > serialize< SerialStorage< ByteArray, LittleEndian >, Type >( const Type& ); \
template SerialStorage< ByteArray, NativeEndian > serialize< SerialStorage< ByteArray, NativeEndian >, Type >( const Type& );

#define SERIAL_PARSE( Type, ByteArray ) \
template Type parse< Type, ByteArray >( const ByteArray& ); \
template Type parse< Type, SerialWrapper< ByteArray, BigEndian > >( const SerialWrapper< ByteArray, BigEndian >& ); \
template Type parse< Type, SerialWrapper< ByteArray, LittleEndian > >( const SerialWrapper< ByteArray, LittleEndian >& ); \
template Type parse< Type, SerialWrapper< ByteArray, NativeEndian > >( const SerialWrapper< ByteArray, NativeEndian >& );

#define SERIAL_IDENT( Type ) \
template uint64_t ident< Type >();

#define SERIAL_ALIAS( Type ) \
template std::string alias< Type >();
