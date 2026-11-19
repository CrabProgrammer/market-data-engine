## M3 results (2026-11-19)

BTC/USD and ETH/USD, 50 seconds each. Decimals come from Kraken's instrument channel.

- BTC/USD (decimals 1/8): 7606 updates ok, 0 mismatches, 0 errors
- ETH/USD (decimals 2/8): 7371 updates ok, 0 mismatches, 0 errors

Fault injection: `MDE_TAMPER_AT=2000` flips one checksum bit.
BTC/USD: 1 mismatch, 1 resubscribe, then 10051 updates ok.
37 updates were dropped because the book was invalid until the new snapshot came.