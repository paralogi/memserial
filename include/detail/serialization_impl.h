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
    bool match_result;
    uint32_t match_hash;
    uint32_t real_hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( SerialType< ValueType >::hash() != real_hash )
            return false;
        match_result = SerialType< ValueType >::match( match_hash );
        return true;
    }
};

/**
 *
 */
struct HashFunctor {
    uint64_t hash;
    const std::string& alias;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( serial_alias< ValueType >() != alias )
            return false;
        hash = serial_hash< ValueType >();
        return true;
    }
};

/**
 *
 */
struct AliasFunctor {
    std::string alias;
    uint64_t hash;

    template< std::size_t Index >
    constexpr bool operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        if ( serial_hash< ValueType >() != hash )
            return false;
        alias = serial_alias< ValueType >();
        return true;
    }
};

} // --- namespace

/**
 *
 */
template< typename ByteArray, typename T >
ByteArray serialize( const T& value ) {

    ByteArray bytes;
    bytes.resize( size( value ) );
    serialize( bytes, value );
    return bytes;
}

/**
 *
 */
template< typename ByteArray, typename T >
void serialize( ByteArray& bytes, const T& value ) {

    if ( size( value ) > bytes.size() )
        throw SerialException( SerialException::ExcBufferOverflow );

    using detail::SerialMetatype;
    using detail::SerialType;

    using SerialIteratorType = detail::SerialIteratorAlias< ByteArray >;
    SerialIteratorType begin( bytes.begin() );

    SerialType< uint64_t >::bout( detail::serial_hash< T >(), begin );

    using IteratorType = typename SerialIteratorType::iterator;
    constexpr auto serial_order = SerialIteratorType::order;
    auto serial_begin = SerialMetatype< T >::template iterator< serial_order >( IteratorType( begin ) );

    SerialType< T >::bout( value, serial_begin );
}

/**
 *
 */
template< typename T, typename ByteArray >
T parse( const ByteArray& bytes ) {

    T value;
    parse( value, bytes );
    return value;
}

/**
 *
 */
template< typename T, typename ByteArray = std::string >
void parse( T& value, const ByteArray& bytes ) {

    if ( size< T >() > bytes.size() )
        throw SerialException( SerialException::ExcBufferOverflow );

    using detail::SerialMetatype;
    using detail::SerialType;
    using detail::SerialHash;

    using SerialIteratorType = detail::SerialIteratorConstAlias< ByteArray >;
    SerialIteratorType begin( bytes.begin() );

    uint64_t hash;
    SerialType< uint64_t >::bin( hash, begin );

    if ( SerialMetatype< T >::alias().hash() != SerialHash( hash ).head() ) {
        detail::MatchFunctor functor{ false, SerialType< T >::hash(), SerialHash( hash ).tail() };
        if ( !search_serial( functor ) || !functor.match_result )
            throw SerialException( SerialException::ExcLayoutIncompatible );
    }

    else if ( SerialType< T >::hash() != SerialHash( hash ).tail() )
        throw SerialException( SerialException::ExcBinaryIncompatible );

    using IteratorType = typename SerialIteratorType::iterator;
    constexpr auto serial_order = SerialIteratorType::order;
    auto serial_begin_copy = SerialMetatype< T >::template iterator< serial_order >( IteratorType( begin ) );
    auto serial_begin = SerialMetatype< T >::template iterator< serial_order >( IteratorType( begin ) );
    auto serial_end = SerialMetatype< T >::template iterator< serial_order >( IteratorType( bytes.end() ) );

    SerialType< T >::init( value, serial_begin, serial_end );
    SerialType< T >::bin( value, serial_begin_copy );
}

/**
 *
 */
template< typename T >
uint64_t size() {

    return detail::SerialType< uint64_t >::size() + detail::SerialType< T >::size();
}

/**
 *
 */
template< typename T >
uint64_t size( const T& value ) {

    return detail::SerialType< uint64_t >::size() + detail::SerialType< T >::size( value );
}

/**
 *
 */
template< typename ByteArray = std::string >
uint64_t hash( const ByteArray& bytes ) {

    if ( detail::SerialType< uint64_t >::size() > bytes.size() )
        throw SerialException( SerialException::ExcBufferOverflow );

    using SerialIteratorType = detail::SerialIteratorConstAlias< ByteArray >;
    SerialIteratorType begin( bytes.begin() );

    uint64_t hash;
    detail::SerialType< uint64_t >::bin( hash, begin );
    return hash;
}

/**
 *
 */
template< typename T >
uint64_t ident() {

    return detail::serial_hash< T >();
}

/**
 *
 */
uint64_t ident( const std::string& alias ) {

    detail::HashFunctor functor{ uint64_t( -1 ), alias };
    search_serial( functor );
    return functor.hash;
}

/**
 *
 */
template< typename T >
std::string alias() {

    return detail::serial_alias< T >();
}

/**
 *
 */
std::string alias( uint64_t ident ) {

    detail::AliasFunctor functor{ {}, ident };
    search_serial( functor );
    return functor.alias;
}

} // --- namespace

#define SERIALIALIZE( Type, ByteArray ) \
template ByteArray serialize< ByteArray, Type >( const Type& ); \
template SerialWrapper< ByteArray, BigEndian > serialize< SerialWrapper< ByteArray, BigEndian >, Type >( const Type& ); \
template SerialWrapper< ByteArray, LittleEndian > serialize< SerialWrapper< ByteArray, LittleEndian >, Type >( const Type& ); \
template SerialWrapper< ByteArray, NativeEndian > serialize< SerialWrapper< ByteArray, NativeEndian >, Type >( const Type& ); \
template void serialize< ByteArray, Type >( ByteArray&, const Type& ); \
template void serialize< SerialWrapper< ByteArray, BigEndian >, Type >( SerialWrapper< ByteArray, BigEndian >&, const Type& ); \
template void serialize< SerialWrapper< ByteArray, LittleEndian >, Type >( SerialWrapper< ByteArray, LittleEndian >&, const Type& ); \
template void serialize< SerialWrapper< ByteArray, NativeEndian >, Type >( SerialWrapper< ByteArray, NativeEndian >&, const Type& );

#define SERIAL_PARSE( Type, ByteArray ) \
template Type parse< Type, ByteArray >( const ByteArray& ); \
template Type parse< Type, SerialWrapper< ByteArray, BigEndian > >( const SerialWrapper< ByteArray, BigEndian >& ); \
template Type parse< Type, SerialWrapper< ByteArray, LittleEndian > >( const SerialWrapper< ByteArray, LittleEndian >& ); \
template Type parse< Type, SerialWrapper< ByteArray, NativeEndian > >( const SerialWrapper< ByteArray, NativeEndian >& ); \
template void parse< Type, ByteArray >( Type&, const ByteArray& ); \
template void parse< Type, SerialWrapper< ByteArray, BigEndian > >( Type&, const SerialWrapper< ByteArray, BigEndian >& ); \
template void parse< Type, SerialWrapper< ByteArray, LittleEndian > >( Type&, const SerialWrapper< ByteArray, LittleEndian >& ); \
template void parse< Type, SerialWrapper< ByteArray, NativeEndian > >( Type&, const SerialWrapper< ByteArray, NativeEndian >& );

#define SERIAL_SIZE( Type ) \
template uint64_t size< Type >(); \
template uint64_t size< Type >( const Type& );

#define SERIAL_HASH( ByteArray ) \
template uint64_t hash< ByteArray >( const ByteArray& );

#define SERIAL_IDENT( Type ) \
template uint64_t ident< Type >();

#define SERIAL_ALIAS( Type ) \
template std::string alias< Type >();
