#include <ixwebsocket/IXWebSocket.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <string>
#include <string_view>
#include <optional>
#include <cstdlib>
#include "mde/kraken_instrument.hpp"
#include "mde/kraken_book.hpp"
#include "mde/resync_policy.hpp"

// Builds {"method":"<method>","params":{"channel":"book","symbol":["<symbol>"],"depth":<depth>}}
std::string book_request(std::string_view method, std::string_view symbol, int depth) {
    return std::string(R"({"method":")") + std::string(method) +
           R"(","params":{"channel":"book","symbol":[")" + std::string(symbol) +
           R"("],"depth":)" + std::to_string(depth) + "}}";
}

// Builds {"method":"<method>","params":{"channel":"instrument"}}
std::string instrument_request(std::string_view method) {
    return std::string(R"({"method":")") + std::string(method) +
           R"(","params":{"channel":"instrument"}})";
}

void resubscribe(ix::WebSocket& ws, std::string_view symbol, int depth) {
    ws.send(book_request("unsubscribe", symbol, depth));
    ws.send(book_request("subscribe", symbol, depth));
}

int main(int argc, char* argv[]) {
    const std::string symbol = (argc > 1) ? argv[1] : "BTC/USD";
    std::optional<mde::kraken::PairInfo> pair_info; 

    constexpr int kDepth = 10;

    ix::WebSocket ws;
    ws.setUrl("wss://ws.kraken.com/v2");    

    std::atomic<int> received{0};

    mde::kraken::BookSync sync{kDepth};
    mde::kraken::ResyncPolicy policy;
    int ok=0, mismatch=0, before_snapshot=0, ignored=0, parse_error=0;

    // Test hook: MDE_TAMPER_AT=N flips one checksum bit in the first book message at or after message N.
    const char* tamper_env = std::getenv("MDE_TAMPER_AT");
    const int tamper_at = tamper_env ? std::atoi(tamper_env) : -1;
    bool tampered = false;
    

    ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
        using T = ix::WebSocketMessageType;
        if (msg->type == T::Open) 
        {
            std::cout << "connected\n";
            ws.send(instrument_request("subscribe"));
        } 
        else if (msg->type == T::Message) 
        {
            ++received;
            if(!pair_info.has_value())
            {
                mde::kraken::PairInfo info;
                auto instrumentParseStatus = parse_instrument(msg->str, symbol, info);
                if(instrumentParseStatus == mde::kraken::ParseStatus::Ok)
                {
                    pair_info = info;
                    std::cout<<"Symbol: "<<symbol<<" Price_Decimals = "<<pair_info.value().price_decimals<<" Qty_Decimals = "<<pair_info.value().qty_decimals<<"\n";
                    ws.send(instrument_request("unsubscribe"));
                    ws.send(book_request("subscribe", symbol, kDepth));
                }
                else if (instrumentParseStatus == mde::kraken::ParseStatus::Ignored)
                {
                    ++ignored;
                }
                else
                {
                    ++parse_error;
                }
                return;
            }

            mde::kraken::BookMessage book_msg;
            auto bookParseStatus = parse_book_message(msg->str, pair_info->price_decimals, pair_info->qty_decimals, book_msg);
            if(bookParseStatus == mde::kraken::ParseStatus::Ignored)
            {
                ++ignored;
            }
            else if(bookParseStatus == mde::kraken::ParseStatus::Error)
            {
                ++parse_error;
                if (policy.on_parse_error() == mde::kraken::ResyncAction::Resubscribe)
                    resubscribe(ws, symbol, kDepth);
            }
            else
            {
                if (tamper_at >= 0 && !tampered && received >= tamper_at) {
                    book_msg.checksum ^= 1u;     // flip the lowest bit: now it can't match
                    tampered = true;
                }

                auto syncResult = sync.on_message(book_msg);
                
                if( syncResult == mde::kraken::BookSync::Result::Ok)
                    ++ok;
                else if( syncResult == mde::kraken::BookSync::Result::UpdateBeforeSnapshot)
                    ++before_snapshot;
                else if( syncResult == mde::kraken::BookSync::Result::Mismatch)
                    ++mismatch;

                if (policy.on_sync(book_msg.type, syncResult) == mde::kraken::ResyncAction::Resubscribe)
                    resubscribe(ws, symbol, kDepth);
            }
        } 
        else if (msg->type == T::Error) 
        {
            std::cout << "error: " << msg->errorInfo.reason << "\n";
        }
    });
    
    ws.start();   // connects on IXWebSocket's own thread
    std::this_thread::sleep_for(std::chrono::seconds(50));
    ws.stop();
    std::cout << "Ok: " << ok << "\nUpdates before snapshot: " << before_snapshot <<
             "\nMismatch: " << mismatch << "\nIgnored: " << ignored <<
             "\nError: " << parse_error << "\nResyncs: " << policy.resyncs() << "\n";
    std::cout << "Messages received: " << received << "\n";


}