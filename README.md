# memserial
Library contains serialization methods that can be used in conjunction with user defined types.

Based on [magic_get](https://github.com/apolukhin/magic_get.git) library.

### Description
To enable serialization support for new data types use macro `SERIAL_TYPE_INFO( Type )`.
This macro provides information to generate an optimal serialization plan for the source type.

Serialized message is a raw sequence of bytes and includes header and data sections. 
The header is an 8-byte hash that is unique identifier for each registered type.
Hash is calculated according to type structure, the order and byte size of the internal fields, and alias provided during registration.
It is used to check operating systems compatibility requirements and data types binary compatibility.

Current implementation is compatible with data types from standard library:
* `std::array`
* `std::bitset`
* `std::chrono`
* `std::complex`
* `std::string`
* `std::tuple`
* `std::vector`

Library can be optionally compiled with Qt5 to enable `QByteArray` support.

The only requirement is compiler with c++14. 

### Restrictions
* Serializable type is a structured data type that meets aggregate initialization requirements. 

* The structure as a whole and its fields should not have explicit alignment, this also applies to attributes like `[[gnu::packed]]`.

* Nesting of structures is limited to prevent looping in case of recursion. Maximum nesting level can be altered by `SERIAL_NESTING_LIMIT` macro.

### Example 1
```c++
#include "serial_forward.h"

struct Article {
    std::string name;
    std::vector< Article > refs;
};

int main() {
    Article article { "Article1", {
        Article{ "Article2" },
        Article{ "Article3" }
    } };

    std::string bytes = memserial::serialize( article );
    Article value = memserial::parse< Article >( bytes );
    memserial::print( value );
}
```
Output:
```
undefined: 
   string[8]: "Article1"
   vector< undefined >[2]: 
      0: undefined: 
         string[8]: "Article2"
         vector< undefined >[0]: empty
      1: undefined: 
         string[8]: "Article3"
         vector< undefined >[0]: empty
```

### Example 2
```c++
#include "serial_forward.h"

struct Header {
    std::string name;
};

struct Article {
    std::string name;
    std::vector< Article > refs;
};

SERIAL_TYPE_INFO( Header )
SERIAL_TYPE_INFO( Article )

int main() {
    Article article { "Article1", {
        Article{ "Article2" },
        Article{ "Article3" }
    } };

    std::string bytes = memserial::serialize( article );

    memserial::print( memserial::parse< Header >( bytes ) );
    std::cout << '\n';

    memserial::trace( bytes );
}
```
Output:
```
Header: 
   string[8]: "Article1"
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
