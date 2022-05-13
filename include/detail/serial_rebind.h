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

namespace memserial {
namespace detail {

template< typename T, typename I >
struct rebind_serial {
    using ValueType = T;
    static constexpr std::size_t tuple_size = boost::pfr::tuple_size< ValueType >::value;

    template< std::size_t Index >
    struct tuple_field {
        using FieldType = typename boost::pfr::tuple_element< Index, ValueType >::type;
        static constexpr FieldType& get( ValueType& value ) {
            return boost::pfr::get< Index >( value );
        }
    };
};

}} // --- namespace
