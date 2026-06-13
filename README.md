# HashMap

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://en.cppreference.com/w/cpp/23)
[![Status](https://img.shields.io/badge/status-learning%20project-green)](https://github.com/privateMwb/HashMap)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A custom `std::unordered_map`-like hash table implemented from scratch in **C++23**, built to explore open-addressing alternatives, separate chaining, manual memory management, rehashing strategies, and bidirectional iterator design.

---

## Table of Contents

- [Overview](#overview)
- [Motivation](#motivation)
- [Features](#features)
- [Design Overview](#design-overview)
  - [Internal Structure](#internal-structure)
  - [Hash & Bucket Index](#hash--bucket-index)
  - [Collision Resolution](#collision-resolution)
  - [Load Factor & Rehashing](#load-factor--rehashing)
  - [insert vs operator\[\]](#insert-vs-operator)
  - [update vs operator\[\]](#update-vs-operator)
  - [Iterator Design](#iterator-design)
  - [Memory Management](#memory-management)
- [Complexity](#complexity)
- [Quick Example](#quick-example)
  - [Basic Usage](#basic-usage)
  - [Access](#access)
  - [Modifiers](#modifiers)
  - [Lookup](#lookup)
  - [Iterators](#iterators)
  - [Custom Hash Function](#custom-hash-function)
  - [Copy & Move](#copy--move)
- [Core API](#core-api)
  - [Constructor & Destructor](#constructor--destructor)
  - [Access](#access-1)
  - [Modifiers](#modifiers-1)
  - [Lookup](#lookup-1)
  - [Capacity & State](#capacity--state)
  - [Iterators](#iterators-1)
- [Benchmark Results](#benchmark-results)
  - [Insert](#insert)
  - [Lookup](#lookup-2)
  - [Erase](#erase)
  - [Update](#update)
  - [Operator\[\]](#operator)
  - [Insert With Rehash](#insert-with-rehash)
  - [Iteration](#iteration)
  - [Copy Construction](#copy-construction)
  - [Move Construction](#move-construction)
  - [Mixed Workload](#mixed-workload)
  - [Summary](#summary)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Notes](#notes)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

HashMap is a chaining hash table built from scratch — no STL containers for storage, no default allocator. It manages a raw heap-allocated array of `Node*` bucket heads, resolves collisions via singly-linked lists, and automatically rehashes when the load factor exceeds 0.75.

It also includes a custom bidirectional iterator that traverses across both chains and buckets, supporting both forward and reverse iteration with range-based for loop compatibility.

---

## Motivation

This project was built to deeply understand:

- Hash table internals — buckets, chaining, load factor, rehashing
- Manual heap management (`new`, `delete`, raw pointer arrays)
- The Rule of 5 (copy/move constructors and assignment operators)
- Bidirectional iterator design across a non-contiguous structure
- Custom hash function support via template parameters
- Collision behavior and how chaining handles it
- Performance tradeoffs vs `std::unordered_map`

---

## Features

- Separate chaining via singly-linked `Node<K, V>` lists
- Automatic rehashing at 0.75 load factor (doubles bucket count)
- Custom hash function support via template parameter
- `insert` — add new key-value pair (no-op if key exists)
- `update` — modify value for existing key (returns bool)
- `erase` — remove by key (returns bool)
- `clear` — destroy all elements, retain bucket array
- `operator[]` — insert-or-access (default-constructs value if key absent)
- `at()` — bounds-checked access, throws `std::out_of_range`
- `find()` — returns iterator to node, or `end()`
- `contains()` — O(1) key existence check
- Full copy & move semantics (Rule of 5)
- Custom bidirectional iterator with const and reverse variants
- Range-based for loop support

---

## Design Overview

### Internal Structure

```
buckets (Node<K,V>**)
  ↓
[ bucket 0 ] → Node{key, value} → Node{key, value} → nullptr
[ bucket 1 ] → nullptr
[ bucket 2 ] → Node{key, value} → nullptr
[ bucket 3 ] → nullptr
     ...
[ bucket N ] → Node{key, value} → nullptr

bucketCount  = total allocated bucket slots
elementCount = total live nodes across all chains
```

Each bucket is the head of a singly-linked list. Nodes within the same bucket share the same hash index (modulo bucket count).

---

### Hash & Bucket Index

The bucket for any key is computed as:

```cpp
index = hasher(key) % bucketCount;
```

`hasher` defaults to `std::hash<K>` but can be any callable matching `std::size_t(const K&)`, supplied as a template argument:

```cpp
HashMap<int, std::string>                    // uses std::hash<int>
HashMap<int, std::string, MyHash>            // uses MyHash
```

---

### Collision Resolution

HashMap uses **separate chaining** — colliding keys land in the same bucket and form a linked list. New nodes are prepended (inserted at the head), making insertion O(1) per bucket regardless of chain length.

```
bucket[i] → [newest] → [older] → [oldest] → nullptr
```

Under a good hash function with load factor ≤ 0.75, chains stay short (average ~1 node), so lookup remains amortized O(1).

---

### Load Factor & Rehashing

The load factor is checked after every insertion:

```cpp
if ((double)elementCount / bucketCount > 0.75)
    rehash(bucketCount * 2);
```

Rehashing allocates a new bucket array twice the size, relinks all existing nodes into the new array (no copies, just pointer rewiring), and frees the old array. Node objects themselves are reused — no allocation or deallocation of `Node` memory occurs during rehash.

```
Before rehash (bucketCount = 4):
  bucket[0] → A → B
  bucket[2] → C

After rehash (bucketCount = 8):
  bucket[0] → A
  bucket[4] → B
  bucket[2] → C    (positions determined by hash % 8)
```

---

### insert vs operator[]

| Method          | Key exists    | Key absent                        |
|-----------------|---------------|-----------------------------------|
| `insert(k, v)`  | No-op         | Inserts new node                  |
| `operator[](k)` | Returns ref   | Default-constructs V, inserts, returns ref |

`operator[]` is the only method that can silently grow the map with a default value — calling `m[k]` on a missing key inserts `V{}` and returns a reference to it.

---

### update vs operator[]

| Method          | Key exists         | Key absent    |
|-----------------|--------------------|---------------|
| `update(k, v)`  | Updates, returns true | No-op, returns false |
| `operator[](k) = v` | Updates       | Inserts V{} first, then assigns |

`update()` is the explicit, safe way to modify an existing value — it will never insert a new key.

---

### Iterator Design

The iterator wraps four pieces of state:

```
Iterator
├── current      — pointer to current Node
├── buckets      — pointer to the bucket array
├── bucketIndex  — which bucket current lives in
└── bucketCount  — total buckets (for bounds)
```

**Forward (`++`):** follows `current->next` within the chain; when the chain ends, scans forward through buckets for the next non-null head.

**Backward (`--`):** when `current == nullptr` (i.e. at `end()`), scans backward from the last bucket to find the last node. Otherwise, walks from the bucket head to find the node before `current`.

Both `iterator` and `const_iterator` are produced by the same `Iterator<K, V, IsConst>` template. Reverse iterators are `std::reverse_iterator<iterator>` wrappers.

> **Iterator invalidation:** rehashing invalidates all iterators. Any insertion that triggers a rehash (crossing the 0.75 load factor threshold) invalidates existing iterators. `erase` and `clear` also invalidate iterators to affected nodes.

---

### Memory Management

HashMap owns all `Node` objects and the bucket array:

- **Construction:** allocates `Node<K,V>*[bucketCount]` and zero-initializes
- **Insertion:** `new Node<K,V>(key, value)` per entry
- **Erasure:** `delete node` for the removed node only
- **Clear:** deletes all nodes, zeroes bucket heads, retains bucket array
- **Destructor / `release()`:** deletes all nodes and the bucket array itself
- **Move:** transfers raw pointers, nulls out the source — O(1), no allocation

---

## Complexity

| Operation         | Average  | Worst Case | Notes                                        |
|-------------------|:--------:|:----------:|----------------------------------------------|
| `insert`          | O(1)     | O(n)       | Worst case: all keys in one chain            |
| `update`          | O(1)     | O(n)       | Lookup + in-place write                      |
| `erase`           | O(1)     | O(n)       | Traverses chain to find and unlink           |
| `operator[]`      | O(1)     | O(n)       | Lookup + optional insert                     |
| `at()`            | O(1)     | O(n)       | Bounds-checked lookup                        |
| `find()`          | O(1)     | O(n)       | Returns iterator                             |
| `contains()`      | O(1)     | O(n)       | Boolean lookup                               |
| `clear()`         | O(n)     | O(n)       | Destroys all nodes                           |
| `rehash()`        | O(n)     | O(n)       | Relinks all nodes into new bucket array      |
| Copy construction | O(n)     | O(n)       | Allocates and inserts all nodes              |
| Move construction | O(1)     | O(1)       | Pointer swap only                            |
| `++iterator`      | O(1)     | O(b)       | b = bucket count; scans for next non-empty   |
| `--iterator`      | O(k)     | O(n)       | k = chain length in current bucket           |

> Average-case O(1) assumes a well-distributed hash function and load factor ≤ 0.75.

---

## Quick Example

### Basic Usage

```cpp
#include "HashMap.h"
#include <iostream>

int main() {
    HashMap<int, std::string> m;

    m.insert(1, "Apple");
    m.insert(2, "Banana");
    m.insert(3, "Cherry");

    std::cout << m.at(1) << "\n"; // Apple
    std::cout << m.size() << "\n"; // 3
}
```

---

### Access

```cpp
HashMap<int, std::string> m;
m.insert(1, "Apple");

// at() — throws std::out_of_range if key not found
std::cout << m.at(1) << "\n"; // Apple
try {
    m.at(99); // throws
} catch (const std::out_of_range& e) { }

// operator[] — inserts default value if key not found
m[2] = "Banana";              // inserts key 2
std::cout << m[2] << "\n";    // Banana
std::cout << m[99] << "\n";   // inserts "" (default std::string), returns ref
```

---

### Modifiers

```cpp
HashMap<int, int> m;
m.insert(1, 100);
m.insert(1, 200); // no-op — key already exists, value stays 100

bool updated = m.update(1, 999); // true — value is now 999
bool missed  = m.update(5, 0);   // false — key 5 doesn't exist

bool erased = m.erase(1); // true
bool missed2 = m.erase(1); // false — already gone

m.insert(10, 1);
m.insert(20, 2);
m.clear(); // removes all nodes; m.size() == 0
```

---

### Lookup

```cpp
HashMap<std::string, int> m;
m.insert("x", 100);
m.insert("y", 200);

// find — returns iterator or end()
auto it = m.find("x");
if (it != m.end()) {
    std::cout << it->key   << "\n"; // x
    std::cout << it->value << "\n"; // 100
}

// contains — bool check
std::cout << m.contains("x") << "\n"; // 1
std::cout << m.contains("z") << "\n"; // 0
```

---

### Iterators

```cpp
HashMap<int, std::string> m;
m.insert(1, "Apple");
m.insert(2, "Banana");
m.insert(3, "Cherry");

// Forward iteration
for (auto it = m.begin(); it != m.end(); ++it) {
    std::cout << it->key << " -> " << it->value << "\n";
}

// Range-based for
for (auto& node : m) {
    std::cout << node.key << " -> " << node.value << "\n";
}

// Reverse iteration
for (auto it = m.rbegin(); it != m.rend(); ++it) {
    std::cout << it->key << " -> " << it->value << "\n";
}
```

---

### Custom Hash Function

```cpp
// Force all keys into bucket 0 (worst case — every key collides)
struct CollisionHash {
    std::size_t operator()(int) const { return 0; }
};

HashMap<int, int, CollisionHash> m;
m.insert(1, 10);
m.insert(2, 20); // same bucket as 1 — forms a chain

// Custom modulo hash
struct ModuloHash {
    std::size_t operator()(int key) const {
        return static_cast<std::size_t>(key % 5);
    }
};

HashMap<int, std::string, ModuloHash> m2;
m2.insert(1, "A");
m2.insert(6, "B"); // key 6 % 5 == 1 — same bucket as key 1
```

---

### Copy & Move

```cpp
HashMap<int, std::string> original;
original.insert(1, "Apple");

// Deep copy — independent from original
HashMap<int, std::string> copy(original);
copy.update(1, "Mango");
std::cout << original.at(1) << "\n"; // Apple (unchanged)

// Move — transfers ownership, original becomes empty
HashMap<int, std::string> moved(std::move(original));
std::cout << original.empty()   << "\n"; // 1
std::cout << moved.at(1)        << "\n"; // Apple
```

---

## Core API

### Constructor & Destructor

```cpp
explicit HashMap(std::size_t bucketCount = 16); // default 16 buckets
~HashMap();

HashMap(const HashMap& other);              // deep copy
HashMap& operator=(const HashMap& other);   // deep copy assign

HashMap(HashMap&& other) noexcept;          // transfer ownership
HashMap& operator=(HashMap&& other) noexcept;
```

---

### Access

```cpp
// Bounds-checked — throws std::out_of_range if key not found
[[nodiscard]] V&       at(const K& key);
[[nodiscard]] const V& at(const K& key) const;

// Insert-or-access — default-constructs V if key absent
[[nodiscard]] V& operator[](const K& key);
```

---

### Modifiers

```cpp
void insert(const K& key, const V& value); // no-op if key exists

[[nodiscard]] bool update(const K& key, const V& value); // false if key not found
[[nodiscard]] bool erase(const K& key);                  // false if key not found

void clear(); // destroys all nodes; bucket array retained
```

---

### Lookup

```cpp
[[nodiscard]] iterator       find(const K& key);
[[nodiscard]] const_iterator find(const K& key) const;
// Returns end() if not found

[[nodiscard]] bool contains(const K& key) const noexcept;
```

---

### Capacity & State

```cpp
[[nodiscard]] std::size_t size()     const noexcept; // element count
[[nodiscard]] std::size_t capacity() const noexcept; // bucket count
[[nodiscard]] bool        empty()    const noexcept;
```

---

### Iterators

```cpp
iterator       begin()  noexcept;    iterator       end()  noexcept;
const_iterator begin()  const noexcept; const_iterator end() const noexcept;
const_iterator cbegin() const noexcept; const_iterator cend() const noexcept;

reverse_iterator       rbegin() noexcept; reverse_iterator       rend() noexcept;
const_reverse_iterator rbegin() const noexcept; const_reverse_iterator rend() const noexcept;
const_reverse_iterator crbegin() const noexcept; const_reverse_iterator crend() const noexcept;
```

Iterators dereference to `Node<K, V>&`, exposing `.key` and `.value` directly.

---


## Benchmark Results

All benchmarks compare HashMap against `std::unordered_map` across 100K, 500K, and 1M elements. Times in microseconds (µs). Compiled without `-O2`.

---

### Insert

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 50,785       | 82,014                  | **1.6×**     |
| 500,000   | 225,540      | 449,009                 | **2.0×**     |
| 1,000,000 | 398,304      | 796,485                 | **2.0×**     |

---

### Lookup

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 8,223        | 18,293                  | **2.2×**     |
| 500,000   | 41,212       | 91,392                  | **2.2×**     |
| 1,000,000 | 80,747       | 175,706                 | **2.2×**     |

Consistent 2.2× advantage — `std::unordered_map`'s extra indirection through `std::pair` and allocator overhead compounds at scale.

---

### Erase

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 22,465       | 61,701                  | **2.7×**     |
| 500,000   | 86,500       | 241,744                 | **2.8×**     |
| 1,000,000 | 173,955      | 553,257                 | **3.2×**     |

The gap widens with scale — HashMap's simpler node structure and direct `delete` call outpace `std::unordered_map`'s allocator-managed erasure.

---

### Update

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 8,795        | 56,805                  | **6.5×**     |
| 500,000   | 41,924       | 209,528                 | **5.0×**     |
| 1,000,000 | 86,570       | 410,615                 | **4.7×**     |

HashMap's biggest win. `update()` is a direct pointer write to the node's value field. `std::unordered_map`'s `operator[]` goes through its bucket lookup, pair access, and write path — significantly more overhead.

---

### Operator[]

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 9,396        | 21,600                  | **2.3×**     |
| 500,000   | 37,926       | 108,518                 | **2.9×**     |
| 1,000,000 | 76,458       | 217,197                 | **2.8×**     |

---

### Insert With Rehash

Starting from 1 bucket to force maximum rehashing at every load factor threshold.

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 52,410       | 88,517                  | **1.7×**     |
| 500,000   | 222,153      | 435,708                 | **2.0×**     |
| 1,000,000 | 450,965      | 876,012                 | **1.9×**     |

Rehash overhead is comparable to normal insertion — the node pointer rewiring strategy keeps rehash cost low. Both maps scale similarly under forced rehashing.

---

### Iteration

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 8,774        | 10,124                  | **1.2×**     |
| 500,000   | 42,453       | 52,029                  | **1.2×**     |
| 1,000,000 | 84,027       | 106,843                 | **1.3×**     |

Modest but consistent advantage. Both iterate over sparse bucket arrays, but HashMap's simpler node structure (just `key`, `value`, `next`) has better cache locality than `std::unordered_map`'s internal node layout.

---

### Copy Construction

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 43,463       | 82,522                  | **1.9×**     |
| 500,000   | 191,845      | 371,028                 | **1.9×**     |
| 1,000,000 | 399,195      | 757,890                 | **1.9×**     |

Consistent ~2× speedup — HashMap's copy path (iterate + insert) avoids the allocator overhead that `std::unordered_map` carries per node.

---

### Move Construction

| Count     | HashMap (µs) | std::unordered_map (µs) | Notes          |
|-----------|-------------:|------------------------:|:---------------|
| 100,000   | 1            | 4                       | Both near-zero |
| 500,000   | 1            | 3                       | Both near-zero |
| 1,000,000 | 2            | 4                       | Both near-zero |

Both implementations are O(1) pointer swaps. The small raw difference is within measurement noise.

---

### Mixed Workload

Interleaved insert, lookup (`contains`), update, and erase operations.

| Count     | HashMap (µs) | std::unordered_map (µs) | Speedup      |
|-----------|-------------:|------------------------:|:------------:|
| 100,000   | 26,831       | 80,608                  | **3.0×**     |
| 500,000   | 171,251      | 487,210                 | **2.8×**     |
| 1,000,000 | 352,806      | 978,667                 | **2.8×**     |

The most realistic benchmark. HashMap's ~3× advantage under mixed load reflects the compounded benefit across all operation types — particularly `update()` and `erase()` where the gap is largest.

---

### Summary

| Operation            | Winner   | Speedup at 1M elements |
|----------------------|----------|:----------------------:|
| Insert               | HashMap  | ~2.0×                  |
| Lookup               | HashMap  | ~2.2×                  |
| Erase                | HashMap  | ~3.2×                  |
| Update               | HashMap  | ~4.7×                  |
| Operator[]           | HashMap  | ~2.8×                  |
| Insert With Rehash   | HashMap  | ~1.9×                  |
| Iteration            | HashMap  | ~1.3×                  |
| Copy Construction    | HashMap  | ~1.9×                  |
| Move Construction    | Tie      | Both ~O(1)             |
| Mixed Workload       | HashMap  | ~2.8×                  |

> HashMap wins every benchmark, with the largest advantages in `update` (~4.7×) and `erase` (~3.2×). These gains come from a simpler node layout, direct pointer operations, and the absence of `std::pair` and allocator indirection. Move construction is a wash — both are pointer swaps.

---

## Project Structure

```
HashMap/
├── include/
│   ├── HashMap.h          # Class declaration, type aliases
│   ├── HashMap.tpp        # Template method definitions
│   ├── Node.h             # Node<K, V> struct (key, value, next)
│   └── Iterator.h         # Bidirectional iterator (const + reverse support)
│
├── benchmarks/
│   ├── benchmarks.cpp     # 10 benchmark suites vs std::unordered_map
│   └── utils/
│       ├── Table.h        # Benchmark result table formatting
│       └── Table.tpp
│
├── tests/
│   └── test.cpp           # Unit tests: lifecycle, access, modifiers, iterators, hashing
│
├── examples/
│   └── examples.cpp       # Usage examples for all major features
│
├── README.md
└── LICENSE
```

---

## Build Instructions

### Requirements

- C++23-compatible compiler: GCC 13+, Clang 17+, or MSVC 19.38+
- No external dependencies — header-only core library

### Compile & Run Tests

```bash
g++ -std=c++23 tests/test.cpp -Iinclude -o build/tests
./build/tests
```

### Compile & Run Examples

```bash
g++ -std=c++23 examples/examples.cpp -Iinclude -o build/examples
./build/examples
```

### Compile & Run Benchmarks

```bash
g++ -std=c++23 benchmarks/benchmarks.cpp -Iinclude -Ibenchmarks/utils -o build/benchmarks
./build/benchmarks
```

> Use `-O2` or `-O3` for production-representative benchmark results.

---

## Notes

- **Not production-ready.** This is an educational project — use `std::unordered_map` in real codebases.
- `insert()` silently ignores duplicate keys — it will not overwrite an existing value. Use `update()` or `operator[]` to modify existing entries.
- `operator[]` inserts a default-constructed value for missing keys. If you only want to read, use `at()` or `find()` to avoid accidental insertions.
- Iterator traversal order is not guaranteed to match insertion order — elements are visited in bucket order, which depends on the hash function and bucket count.
- Iterator invalidation occurs on any rehash (triggered by insertion past load factor 0.75), `erase`, and `clear`.
- The reverse iterator (`--`) on a long chain is O(k) where k is the chain length, because the singly-linked list must be walked from the head to find the previous node. This is expected behavior for a singly-linked chaining implementation.
- `clear()` retains the bucket array. Bucket count does not shrink after `clear()`.

---

## Contributing

Learning-focused PRs and improvements are welcome. Some areas worth exploring:

- `emplace()` — in-place construction to avoid temporary copies
- `reserve()` — pre-allocate buckets to a specific count
- Shrink-on-erase (reduce bucket count when load factor drops too low)
- `std::initializer_list` constructor
- CMake build system integration
- CI pipeline (GitHub Actions)

---

## License

[MIT](LICENSE) — free to use, modify, and distribute for educational and personal purposes.     