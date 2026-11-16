#include "mde/kraken_book.hpp"
#include "mde/kraken_checksum.hpp"

namespace mde::kraken {

BookSync::BookSync(std::size_t depth) : book_(depth) {}

BookSync::Result BookSync::on_message(const BookMessage& msg) {
    ///TODO
}

const L2Book& BookSync::book() const { return book_; }

}  // namespace mde::kraken