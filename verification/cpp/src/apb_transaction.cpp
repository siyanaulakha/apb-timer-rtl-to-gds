#include "apb_transaction.hpp"

#include <iomanip>
#include <sstream>

std::string ApbTransaction::describe() const {
    std::ostringstream stream;
    stream << (direction == ApbDirection::Read ? "READ" : "WRITE")
           << " addr=0x" << std::hex << std::setw(3) << std::setfill('0') << address;
    if (direction == ApbDirection::Write) {
        stream << " data=0x" << std::setw(8) << write_data
               << " strobe=0x" << static_cast<unsigned>(strobe);
    } else {
        stream << " read=0x" << std::setw(8) << read_data;
    }
    stream << std::dec << " error=" << observed_error;
    return stream.str();
}
