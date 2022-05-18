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

#include <stdexcept>

namespace memserial {

/**
 * \~english
 * \brief The exception class thrown by serialization methods.
 * \~russian
 * \brief Класс исключения выбрасываемого методами сериализации.
 */
class SerialException : public std::exception {
public:
    enum ExceptionCode {
        ExcArrayOverflow,
        ExcBufferOverflow,
        ExcLayoutIncompatible,
        ExcBinaryIncompatible
    };

    SerialException( ExceptionCode code ) :
            m_code( code ) {

    }

    ExceptionCode code() const noexcept {
        return m_code;
    }

    const char* what() const noexcept override {
        switch ( m_code ) {
        case ExcArrayOverflow:
            return "dynamic array size must be less than 2^32";
        case ExcBufferOverflow:
            return "data size of serialized buffer is too small to fit the type";
        case ExcLayoutIncompatible:
            return "data structure of serialized buffer does not match the type";
        case ExcBinaryIncompatible:
            return "data type of serialized buffer is binary incompatible with the original type";
        default:
            return "unknown exception case";
        }
    }

private:
    ExceptionCode m_code;
};

} // --- namespace
