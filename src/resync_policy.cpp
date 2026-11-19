#include "mde/resync_policy.hpp"

namespace mde::kraken {
ResyncAction ResyncPolicy::on_sync(BookMessage::Type type, BookSync::Result result) noexcept { 

    if(waiting_)
    {
        if(type == BookMessage::Type::Snapshot)
        {
            if(result == BookSync::Result::Ok)
            {
                waiting_ = false;
                return ResyncAction::None;    
            }
            else
            {
                return start_resync();  
            }
        }
        else
        {
            return ResyncAction::None;    
        }
    }
    else
    {
        if(result == BookSync::Result::Ok)
        {
            return ResyncAction::None;    
        }
        else
        {
            return start_resync();
        }
    }
}

ResyncAction ResyncPolicy::on_parse_error() noexcept { 
    if(!waiting_)
    {
        return start_resync();
    }
    return ResyncAction::None; 
}

ResyncAction ResyncPolicy::start_resync() noexcept
{
    ++resyncs_;
    waiting_ = true;
    return ResyncAction::Resubscribe; 
}

}  // namespace mde::kraken 