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
 * Класс исключения выбрасываемого методами сериализации.
 */
class SerialException : public std::exception {
public:
    enum ExceptionCode {
        ExcOutOfRange,
        ExcTypeMissmatch,
    };

    SerialException( ExceptionCode code ) :
            m_code( code ) {

    }

    ExceptionCode code() const noexcept {
        return m_code;
    }

    const char* what() const noexcept override {
        switch ( m_code ) {
        case ExcOutOfRange:
            return "buffer size is too small";
        case ExcTypeMissmatch:
            return "data type not matching";
        default:
            return "unknown exception case";
        }
    }

private:
    ExceptionCode m_code;
};

} // --- namespace
