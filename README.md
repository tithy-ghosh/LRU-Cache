# LRU Cache (C++)

Implementation of a Least Recently Used (LRU) Cache supporting `get(key)` and `put(key, value)` in **O(1) average time**, using a hand-rolled doubly linked list combined with a hash map.

## Data Structures Used and Why

**1. `unordered_map<int, Node*> cache`**
Maps each key directly to a pointer to its node in the linked list. This gives O(1) average lookup — no scanning required to check if a key exists or to find where it lives.

**2. Custom doubly linked list (`Node` struct with `prev`/`next` pointers)**
Maintains all cached entries ordered by recency: the node right after `head` is always the **least recently used (LRU)** entry, and the node right before `tail` is always the **most recently used (MRU)** entry.

A doubly linked list is used (rather than a singly linked one) because removing a node requires O(1) access to both its previous and next neighbors — with only `next` pointers, removal would require traversing from the head to find the predecessor, which is O(n).

**3. Two dummy sentinel nodes (`head` and `tail`)**
`head` and `tail` are permanent placeholder nodes that never hold real data. Every real node always sits somewhere between them. This removes the need for special-case logic when inserting into or removing from an empty list, or when inserting/removing at either end — every insertion and removal is a uniform four-pointer update.

Together, the hash map answers *"does this key exist, and where is it?"* in O(1), while the linked list answers *"which entry should be evicted next?"* in O(1) — neither structure alone can do both efficiently.

## How LRU Ordering Is Maintained

- The list is kept sorted by recency at all times: LRU end = right after `head`, MRU end = right before `tail`.
- **On `get(key)`:** if the key exists, its node is unlinked from its current position (`removeNode`) and re-inserted just before `tail` (`insertAtMRU`), marking it as most recently used.
- **On `put(key, value)`:**
  - If the key already exists, its value is updated and the node is moved to the MRU position (same as `get`).
  - If the key is new, a node is created and inserted at the MRU position. If this pushes the cache over capacity, the node right after `head` (the LRU entry) is removed from both the list and the hash map, and deleted.
- Because moving a node only requires four pointer reassignments (`removeNode` + `insertAtMRU`), reordering never depends on cache size — it's O(1) regardless of how many entries are cached.

## Time Complexity

| Operation | Complexity | Why |
|---|---|---|
| `get(key)` | O(1) average | Hash map lookup + pointer-only reordering |
| `put(key, value)` | O(1) average | Hash map lookup/insert + pointer-only reordering/eviction |

(Average, not worst-case, because `unordered_map` lookups are O(1) average but O(n) worst-case under hash collisions — this is standard for hash-based structures.)

## Space Complexity

O(capacity) — one `Node` (key, value, two pointers) and one hash map entry are stored per cached item, plus two fixed dummy nodes.

## How to Run

```bash
g++ -std=c++11 -o output/LRU_Cache.exe LRU_Cache.cpp
./output/LRU_Cache.exe
```

No external dependencies — standard library only (`<iostream>`, `<unordered_map>`).

The program is interactive: on startup it asks for the cache capacity, then presents a menu with four options:

1. **Put (Insert/Update)** — prompts for a key and value, calls `put(key, value)`, and prints the resulting cache state.
2. **Get (Retrieve)** — prompts for a key, calls `get(key)`, and prints the returned value (or "result: not found" if the result is -1).
3. **Display Cache** — prints the current cache contents from LRU to MRU without performing any operation.
4. **Exit** — ends the program.

Every `put()` and `get()` call prints the updated cache state (LRU → ... → MRU) immediately afterward, and an eviction (when capacity is exceeded) is printed explicitly with the evicted key — so a single session can demonstrate insertion, retrieval, recency reordering, and eviction end to end.

### Example Output

Below is an actual captured run (cache size 3) exercising every required behavior: a `get` on an empty cache, `put`s that fill the cache, a `get` that reorders recency, an update `put` on an existing key, a `get` on a nonexistent key, a `put` that triggers eviction of the least recently used entry, and a final `get` confirming the surviving keys are intact.

```
              LRU CACHE DEMO             
---------------------------------------- 
cache size: 3

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 2
key: 1

>> get(1)
   result: not found
  cache: (empty)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 1
key: 2
value: 3

>> put(2, 3)
  cache: [2:3]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 1
key: 3
value: 4

>> put(3, 4)
  cache: [2:3] -> [3:4]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 3

>> current state
  cache: [2:3] -> [3:4]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 2
key: 2

>> get(2)
   result: 3
  cache: [3:4] -> [2:3]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 1
key: 1
value: 1

>> put(1, 1)
  cache: [3:4] -> [2:3] -> [1:1]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 1
key: 2
value: 3

>> put(2, 3)
  cache: [3:4] -> [1:1] -> [2:3]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 2
key: 32

>> get(32)
   result: not found
  cache: [3:4] -> [1:1] -> [2:3]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 1
key: 99
value: 99

>> put(99, 99)
   evicted key 3 (cache was full)
  cache: [1:1] -> [2:3] -> [99:99]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 2
key: 1

>> get(1)
   result: 1
  cache: [2:3] -> [99:99] -> [1:1]   (oldest -> newest)

----------------------------------------
1. put   2. get   3. show cache   4. quit
----------------------------------------
> 4

bye!
```

**What this trace proves:**
- `get` on a missing/empty-cache key correctly returns "not found" rather than crashing or returning garbage.
- `put(2,3)` then `put(3,4)` fill the cache in insertion order.
- `get(2)` moves key 2 to the newest position — confirmed by the cache line changing from `[2:3] -> [3:4]` to `[3:4] -> [2:3]`.
- `put(1,1)` adds a third key without evicting (cache size 3, only 3 keys so far).
- `put(2,3)` again — key 2 already exists, so this is an *update*, not a new insert; it moves to newest without evicting, confirming updates don't trigger eviction.
- `get(32)` on a key that was never inserted correctly returns "not found" without disturbing the cache.
- `put(99,99)` is a 4th unique key on a capacity-3 cache — this correctly evicts key 3, the least recently used entry at that point (`evicted key 3 (cache was full)`).
- Final `get(1)` confirms key 1 is still present and returns its value, while also bumping it to newest.

## Notes / Trade-offs

- Keys and values are hardcoded as `int` for simplicity and to match the task's example. A templated version (`template<typename K, typename V>`) would generalize this without changing the underlying approach.
- Capacity is assumed positive per the task spec; a production version would validate this in the constructor and throw on `capacity <= 0`.
- TTL/expiration (optional bonus) is not implemented in this submission.