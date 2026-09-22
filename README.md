# LRU Cache System (C++) — with TTL / Expiration

An interactive **Least Recently Used (LRU) cache** with optional per-item **TTL / expiration**, implemented in C++ using a hand-rolled doubly linked list combined with a hash map. `get` and `put` run in **O(1) average time**.

This README documents the current source **`LRU_Cache.cpp`** and its pre-built binary **`output/LRU_Cache.exe`**.

---

## Features

- **O(1) operations** — `put` / `get` both use a hash map for lookup and constant-pointer work on a doubly linked list for recency reordering and eviction.
- **Optional TTL** — every item can be inserted with a TTL in seconds; when its lifetime elapses, the item is detected as expired and purged from the cache.
- **Expired-item sweep** — expired entries are also removed (and reported) whenever the cache is displayed.
- **LRU eviction** — when a `put` would exceed capacity, the least recently used entry is removed automatically.
- **Interactive menu** — put, get, and inspect the cache state (LRU → MRU) after every operation.
- **Robust input handling** — non-numeric menu / capacity input never hangs the program; it prints a message and re-prompts.

---

## Data Structures Used and Why

1. **`std::unordered_map<string, Node*>`** — maps each key (a `string`) directly to its linked-list node, giving O(1) average membership tests and lookups (no scanning). Values are `int`.
2. **Hand-rolled doubly linked list (`Node` with `prev`/`next`)** — keeps entries ordered by recency: `head->next` is the LRU entry, `tail->prev` is the MRU entry. Doubly linked so removal of an arbitrary node is O(1); a singly linked list would need an O(n) walk to find the predecessor.
3. **Two dummy sentinel nodes (`head` and `tail`)** — permanent placeholders that never hold data, so insert/remove are the same four-pointer update regardless of list size (no empty-list special cases).

In short: the hash map answers *"does this key exist, and where?"* in O(1); the linked list answers *"which entry gets evicted next?"* in O(1).

---

## How It Behaves

- **`put(key, value)`** — prompts for a TTL choice.
  - With TTL selected (**1**), additionally prompts for a lifetime in seconds (must be > 0) and echoes `Item inserted with TTL = N seconds.`; arm, no expiration. `Item inserted without expiration.` is printed otherwise.
  - If the key already exists, its value is updated, its recency is bumped (and a new TTL / expiration applies); nothing is evicted.
  - If the key is new, it is inserted at the MRU end. Once the cache exceeds capacity, the LRU entry is removed and printed as `LRU item removed: key = value` *before* the insertion message.
  - The updated cache is displayed afterwards.
- **`get(key)`** — returns the value (`Value = N`) and bumps the entry to MRU. If the key is absent **or its TTL has elapsed**, it prints `Key not found or item has expired.` and purges the expired entry if one existed.
- **`get` on a missed/expired key never crashes** — the cache continues normally.
- **`display()`** — first purges any expired items (printing `Expired item removed: key = value` for each), then prints the order `LRU -> [key : value] -> ... <- MRU`, or `Cache is empty.` when no items remain.

---

## Complexity

| Operation | Complexity | Reason |
|---|---|---|
| `get(key)` | O(1) average | Hash map lookup + constant pointer reordering (expiry check is O(1)) |
| `put(key, value)` | O(1) average | Hash map lookup/insert + constant reordering/eviction |
| Space | O(capacity) | One `Node` + one hash map entry per cached item, plus two dummy nodes |

*Average, not worst-case*: `std::unordered_map` is O(1) average but O(n) under heavy hash collisions — standard for hash-based structures.

---

## How to Run

The program is pre-built — just run it (standard Windows/PowerShell command):

```bash
./output/LRU_Cache.exe
```

To rebuild the binary from source:

```bash
g++ -std=c++11 -Wall -Wextra -o output/LRU_Cache.exe LRU_Cache.cpp
```

On startup it asks for the cache capacity (a positive integer). A non-numeric capacity prints `Invalid input.`; `0` or less prints `Capacity must be greater than 0.` and exits. Then it shows a menu:

| Choice | Prompt | Behavior |
|---|---|---|
| **1. Put** | `Enter key:` `Enter value:` `Do you want TTL/expiration? (1 = Yes, 0 = No):` (`Enter TTL in seconds:` if yes) | Insert or update; optionally attach a TTL. |
| **2. Get** | `Enter key:` | Print `Value = N`, or `Key not found or item has expired.` |
| **3. Display Cache** | — | Print current order `LRU -> [...] <- MRU`, purging expired items first |
| **4. Exit** | — | Print `Thank you for using LRU Cache.` and quit |

Any other menu input prints `Invalid choice. Please try again.` and re-prompts (the program recovers instead of looping, crashing, or hanging).

---

## Example Session

Captured run with capacity 3 covering: insertion with/without TTL, retrieval that reorders recency, a miss, an update, and an eviction.

```
LRU CACHE SYSTEM
---------------------
Enter cache capacity: 3

---MENU
1. Put
2. Get
3. Display Cache
4. Exit
Enter your choice: 1

Enter key: a
Enter value: 100
Do you want TTL/expiration? (1 = Yes, 0 = No): 1
Enter TTL in seconds: 5
Item inserted with TTL = 5 seconds.

LRU -> [a : 100] <- MRU

Enter your choice: 1

Enter key: b
Enter value: 200
Do you want TTL/expiration? (1 = Yes, 0 = No): 0
Item inserted without expiration.

LRU -> [a : 100] -> [b : 200] <- MRU

Enter your choice: 1

Enter key: c
Enter value: 300
Do you want TTL/expiration? (1 = Yes, 0 = No): 0
Item inserted without expiration.

LRU -> [a : 100] -> [b : 200] -> [c : 300] <- MRU

Enter your choice: 2

Enter key: a
Value = 100

LRU -> [b : 200] -> [c : 300] -> [a : 100] <- MRU

Enter your choice: 2

Enter key: z
Key not found or item has expired.

LRU -> [b : 200] -> [c : 300] -> [a : 100] <- MRU

Enter your choice: 1

Enter key: d
Enter value: 400
Do you want TTL/expiration? (1 = Yes, 0 = No): 0
LRU item removed: b = 200
Item inserted without expiration.

LRU -> [c : 300] -> [a : 100] -> [d : 400] <- MRU

Enter your choice: 4

Thank you for using LRU Cache.
```

**Walkthrough:**
1. `put(a, 100)` with a 5-second TTL → inserted at MRU.
2. `put(b, 200)` and `put(c, 300)` without TTL fill the capacity-3 cache (keys are strings here, unlike many textbook LRU demos).
3. `get(a)` returns the value and bumps key `a` to MRU — the line changes to `[b] -> [c] -> [a]`.
4. `get(z)` never existed → `Key not found or item has expired.`, cache undisturbed.
5. `put(d, 400)` exceeds capacity → LRU entry (key `b`) is evicted, printed as `LRU item removed: b = 200`.

**Expiration (verified separately):** inserting with TTL = 1 second and calling `get` later returns `Key not found or item has expired.`. The same item is also swept (with `Expired item removed: ...`) the next time the cache is displayed, after which only `Cache is empty.` remains.

---

## Notes / Trade-offs

- Keys are `string`, values are `int`. Templating the class (`template <typename K, typename V>`) would generalize both without changing the algorithm. TTL is hardcoded in seconds (`long long` epoch time).
- TTL is checked lazily: an expired entry is discovered and purged when it is accessed or when the cache is displayed — there is no background sweeper thread.
- The class explicitly deletes its copy constructor and copy assignment; the linked-list nodes are owned by the single cache instance and released in the destructor.
- Capacity and TTL inputs are validated with friendly messages; a library-grade version might throw exceptions instead of printing to the console.
- `LRU_Cache.cpp` and the pre-built `output/LRU_Cache.exe` correspond to each other: rebuilding from source reproduces the same interactive behavior and messages documented here.