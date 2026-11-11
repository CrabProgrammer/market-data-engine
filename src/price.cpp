#include <charconv>
#include <system_error>
#include "mde/price.hpp"

namespace mde {
std::optional<std::int64_t> parse_ticks(std::string_view text, int decimals) {

    if(text.empty() || text.front() == '-' || decimals < 0 || decimals > 18) 
    {
        return std::nullopt; 
    }

    auto dec = static_cast<std::size_t>(decimals);
    auto splitter = text.find('.');
    std::string_view intPart = text.substr(0, splitter);
    std::string_view fractionPart = (splitter == std::string_view::npos)
                                ? std::string_view{} : text.substr(splitter + 1);
    std::int64_t result = 0;
    std::int64_t fraction = 0;
    
    if(!fractionPart.empty() && fractionPart.front() == '-')
    {
        return std::nullopt; 
    }

    {
        auto [ptr, ec] = std::from_chars(intPart.data(), intPart.data() + intPart.size(), result );
        if (ec != std::errc{} || ptr != intPart.data() + intPart.size() )        
            return std::nullopt;
    }

    if(fractionPart.size() > 0)
    {
        auto [ptr, ec] = std::from_chars(fractionPart.data(), fractionPart.data() + fractionPart.size(), fraction );
        if (ec != std::errc{} || ptr != fractionPart.data() + fractionPart.size() )
            return std::nullopt;
        
        if(fractionPart.size() > dec)
        {
            for(char c : fractionPart.substr(dec, fractionPart.size()))
            {
                if(c!='0')
                    return std::nullopt;
            }
        }
        
    }
    
    for(std::size_t i = 0; i < dec; ++i)
    {
        result*=10;
    }
    
    if(fractionPart.size() < dec )
    {
        for(std::size_t i = 0; i < dec-fractionPart.size(); ++i)
        {
            fraction*=10;
        }
    }
    else if(fractionPart.size() > dec)
    {
        for(std::size_t i = 0; i < fractionPart.size()-dec; ++i)
        {
            fraction/=10;
        }
    }
    result+=fraction;
    
    return result;

}
}  // namespace mde