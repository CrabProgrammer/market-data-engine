#include "mde/kraken_book.hpp"
#include "mde/kraken_checksum.hpp"

namespace mde::kraken {

BookSync::BookSync(std::size_t depth) : book_(depth) {}

BookSync::Result BookSync::on_message(const BookMessage& msg) {
    if(msg.type == BookMessage::Type::Snapshot)
    {
        book_.clear();
        have_snapshot_ = true;
    }
    else if(msg.type == BookMessage::Type::Update && have_snapshot_ == false)
    {
        return BookSync::Result::UpdateBeforeSnapshot;
    }

    for(const auto &lvl : msg.updates)
    {
        book_.apply(lvl.side, lvl.price, lvl.qty);
    }

    if(checksum(book_) != msg.checksum)
    {
        book_.clear();
        have_snapshot_ = false;
        return BookSync::Result::Mismatch;
    }
    return BookSync::Result::Ok;
}

const L2Book& BookSync::book() const { return book_; }

}  // namespace mde::kraken