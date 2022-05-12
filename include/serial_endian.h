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

#include <string>

namespace memserial {

#if !defined( SERIAL_ENDIAN_DEFAULT )
#define SERIAL_ENDIAN_DEFAULT NativeEndian
#endif

/**
 * \~english
 * \brief Declaration of bytes ordering in memory.
 * BigEndian or LittleEndian refers to write order starting with most or least significant byte.
 * NativeEndian reffers to the current endianness used in system.
 * \~russian
 * \brief Объявление порядка байт в памяти.
 * BigEndian или LittleEndian относится к порядку записи, начиная со старшего или младшего байта.
 * NativeEndian относится к текущему порядку байт, используемому в системе.
 */
enum SerialEndian {
    BigEndian,
    LittleEndian,
    NativeEndian,
    DefaultEndian = SERIAL_ENDIAN_DEFAULT
};

/**
 * \~english
 * \brief The helper class used in serialization methods.
 * \~russian
 * \brief Вспомогательный класс, используемый в методах сериализации.
 */
template< typename ByteArray, SerialEndian endian = DefaultEndian >
class SerialWrapper : public ByteArray {
public:
    using type = ByteArray;
    static constexpr SerialEndian order = endian;
};

template< SerialEndian endian = DefaultEndian >
using StringWrapper = SerialWrapper< std::string, endian >;

} // --- namespace
