#include "mde/l2_book.hpp"
#include <algorithm>
namespace mde {

L2Book::L2Book(std::size_t depth) : depth_(depth) {
    bids_.reserve(depth);   // pop_back() before emplace
    asks_.reserve(depth);
}

void L2Book::apply(Side side, Price price, Qty qty) {
    auto& sideArray = side == Side::Bid ? bids_ : asks_;

    for(std::size_t i = 0; i < sideArray.size(); ++i)
    {
        if(qty == 0)
        {
            if(sideArray[i].price == price)
            {
                sideArray.erase(sideArray.begin()+i);
                return;
            }
        }
        else
        {
            if(sideArray[i].price == price)
            {
                sideArray[i].qty = qty;
                return;
            }
            else if((side == Side::Bid && price > sideArray[i].price) ||
                    (side == Side::Ask && price < sideArray[i].price))
            {
                if(sideArray.size() == depth_)
                {
                    sideArray.pop_back();
                }

                sideArray.emplace(sideArray.begin()+i, price, qty);
                return;
            }
        }
    }

    if(sideArray.size() < depth_ && qty != 0 ) 
    {
        sideArray.emplace_back(price, qty);
    }
} 

std::optional<Level> L2Book::bestBid() const { 
    if(!bids_.empty()) return bids_.front();
    return std::nullopt; }
std::optional<Level> L2Book::bestAsk() const { 
    if(!asks_.empty()) return asks_.front();
    return std::nullopt; }
std::span<const Level> L2Book::bids() const { return bids_; }
std::span<const Level> L2Book::asks() const { return asks_; }

}  // namespace mde