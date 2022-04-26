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
struct ReducedHashFunctor {
    uint64_t& hash;

    template< std::size_t Index >
    constexpr void operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        hash_reduce( hash, SerialMetatype< ValueType >::hash().full() );
    }
};

/**
 *
 */
struct PerfectHashFunctor {
    std::vector< uint64_t >& hash;

    template< std::size_t Index >
    constexpr void operator()( size_t_< Index > ) {
        using ValueType = typename SerialIdentity< Index >::ValueType;
        hash[ Index ] = SerialMetatype< ValueType >::hash().full();
    }
};

/**
 *
 */
static uint64_t reduced_hash() {

    uint64_t hash = 0;
    ReducedHashFunctor functor{ hash };
    foreach_serial( functor );
    return hash;
}

/**
 *
 */
static std::vector< uint64_t > perfect_hash() {

    std::vector< uint64_t > hash( count_serial() );
    PerfectHashFunctor functor{ hash };
    foreach_serial( functor );
    return hash;
}

/**
 *
 */
extern const uint64_t reduced_hash_static = memserial::detail::reduced_hash();
extern const std::vector< uint64_t > perfect_hash_static = memserial::detail::perfect_hash();

}} // --- namespace
