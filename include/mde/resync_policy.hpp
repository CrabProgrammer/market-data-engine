#pragma once
#include "mde/kraken_book.hpp"

namespace mde::kraken {

enum class ResyncAction { None, Resubscribe };

class ResyncPolicy {
public:
    ResyncAction on_sync(BookMessage::Type type, BookSync::Result r) noexcept;
    ResyncAction on_parse_error() noexcept;

    bool waiting_for_snapshot() const noexcept { return waiting_; }
    int  resyncs() const noexcept { return resyncs_; }
    
private:
    ResyncAction start_resync() noexcept;

    bool waiting_ = true;
    int  resyncs_ = 0;
};

}  // namespace mde::kraken