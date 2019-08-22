# memserial
C++ template serialization library for "aggregate types".
Based on [magic_get](https://github.com/apolukhin/magic_get.git) library.

### Features
To enable serialization support for new messages a special macro 'ENABLE_SERIAL_TYPE_INFO( Type )', 
which takes the type of original message as a parameter.

Message is an aggregate data type with the exception of empty structures, "union" types, and references. 
By the way, any arithmetic type or array is also aggregate, and therefore can act as a message type.

### Restrictions:
* The structure as a whole and its fields should not have explicitly specified alignment, 
this also applies for attributes like '[[gnu::packed]]'.

* Structure fields may only include previously mentioned data types, 
including other registered message types.

* For structure fields with an undefined size, you can use 'std::string' and 'std::vector'.

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
Outputs:
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