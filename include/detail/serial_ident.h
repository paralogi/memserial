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

template< typename T >
static constexpr uint64_t serial_hash() {
    return SerialHash{ SerialMetatype< T >::alias().hash(), SerialType< T >::hash() }.full();
}

template< typename T >
static constexpr std::string serial_alias() {
    return SerialAlias{ SerialMetatype< T >::alias() }.convert< std::string >();
}

}} // --- namespace
