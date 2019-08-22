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
