#define SERIAL_TYPES_ONLY
#include "serial_precompile.cpp"

namespace memserial {
namespace detail {

extern const uint64_t SerialHashStatic = serialHash();

}} // --- namespace
