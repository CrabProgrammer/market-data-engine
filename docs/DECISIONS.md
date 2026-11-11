# Design decisions

## D1. One side of the L2 book: sorted std::vector (depth 10)

- 10 levels × 16 B = 160 B = 3 contiguous cache lines. A linear search or
  a shift on insert/remove touches only those lines, and they stay in L1/L2
  because the book is updated all the time.
- std::map: one heap node per level, so each insert allocates, and walking
  the tree follows pointers. The next address is only known after the
  current node is loaded, so cold misses happen one after another.
- Flat array indexed by tick: BTC's tick is $0.10, so a $10,000 move is
  100,000 ticks. The array would be huge and mostly empty, or would need
  re-centring when the price leaves its range.
- Revisit if depth grows to hundreds of levels (shifts get expensive).

## D2. L2Book storage details

- **`reserve(depth)` in the constructor.** `apply()` must never allocate on the hot path:
  an allocation can take a lock inside `malloc` or cause a page fault, so its latency is
  unpredictable. The vector never grows past `depth`: at full depth `apply()` calls
  `pop_back()` before `emplace()`, and `emplace_back()` only runs below full depth.
- **Best price first.** Top of book is `front()`, O(1). Insert and erase shift up to
  `depth` elements, O(depth); at depth 10 that is at most 160 bytes in about 3 cache
  lines, moved as one contiguous block with no pointer chasing. At a much larger depth
  I would keep the best price at the back and find levels with binary search.
- **Getters return `std::span<const Level>`.** No copy, read-only. Unlike
  `const std::vector<Level>&`, the span doesn't expose the container, so the storage
  can change (e.g. to a fixed-size `std::array`) without changing the interface.
  Like a reference, it doesn't own the data: it is valid only while the book exists,
  and `apply()` changes what it shows.

## D3 — Prices and quantities are signed int64_t ticks

- **Why not double?** You can't store a price like 0.1 exactly in binary, so the same
price can come out as two slightly different doubles. Then `==` stops working: an
update doesn't find its level, the old level stays in the book, and the checksum fails.
With integer ticks, equal prices are always equal.

- **Why 64-bit?** Quantities have 8 decimals. An int32 tops out around 2.1 billion,
which is only 21.47 BTC. A single 50 BTC level wouldn't fit. int64 has plenty of room.

- **Why signed?** If a quantity ever goes below zero, say from a bug or a message
arriving out of order, I want to see a negative number and catch it with an assert.
An unsigned value would quietly wrap around to a huge number that looks perfectly valid.

- **Parsing:** `parse_ticks` turns the text straight into ticks with `std::from_chars`,
so a double never gets involved. The number of decimals comes from the instrument
(Kraken's `instrument` channel says 1 for price and 8 for qty on BTC/USD).
Anything outside 0–18 is rejected.