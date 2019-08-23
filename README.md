# memserial
Library contains serialization methods that can be used in conjunction with user defined types.

Based on [magic_get](https://github.com/apolukhin/magic_get.git) library.

### Description
To enable serialization support for new data types use macro `ENABLE_SERIAL_TYPE_INFO( Type )`.
This macro provides information to generate an optimal serialization plan for the source type.

Serialized message is a raw sequence of bytes and includes header and data sections. 
The header is an 8-byte hash that is unique identifier for each registered type.
Hash is calculated according to type structure, the order and byte size of the internal fields, and the unique number provided during registration.
It is used to check operating systems compatibility requirements and message types of exchange protocol.

Current implementation is compatible with data types from standard library:
* `std::array`
* `std::bitset`
* `std::chrono`
* `std::string`
* `std::vector`

The only requirement is compiler with c++14 support. 

### Restrictions
* Serializable type is a structured data type that meets aggregate initialization requirements, with the exception of empty structures, union types, and references. 

* Structure fields may only include previously mentioned data types, including other serializable types.

* The structure as a whole and its fields should not have explicit alignment, this also applies to attributes like `[[gnu::packed]]`.

### Example
```c++
#include "serial_forward.h"

struct Article {
    std::string name;
    std::vector< Article > refs;
};

ENABLE_SERIAL_TYPE_INFO( Article )

int main() {
    Article article { "Article1", { { "Article2" }, { "Article3" }, } };
    std::string bytes = memserial::serialize( article );
    memserial::print( std::cout, bytes );
}
```
### Output
```
Article: 
   string[8]: "Article1"
   vector< Article >[2]: 
      0: Article: 
         string[8]: "Article2"
         vector< Article >[0]: empty
      1: Article: 
         string[8]: "Article3"
         vector< Article >[0]: empty
```
