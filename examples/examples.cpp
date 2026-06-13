// HashMap Examples
// Demonstrates basic usage of the hash table container:
//
// - basic usage (insert, operator[], size, capacity)
// - access (at, operator[])
// - modifiers (insert, update, erase, clear)
// - lookup (find, contains)
// - capacity management (load factor, rehash)
// - iterators (forward and reverse traversal)
// - copy and move semantics
// - default, collision, and custom hash functions
//
// These examples illustrate the core features and intended usage of HashMap.

#include <iostream>
#include <string>

#include "HashMap.h"

// Basic Example
// Tests basic insertion and access
void basic_example() {
	std::cout << "Basic Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(2) << "\n";

	std::cout << "[DONE]\n\n";
}

// Copy Example
// Tests deep copy construction
void copy_example() {
	std::cout << "Copy Example\n";

	HashMap<int, std::string> m1;
	m1.insert(1, "Earth");

	HashMap<int, std::string> m2(m1);

	std::cout << m2.at(1) << "\n";

	std::cout << "[DONE]\n\n";
}

// Move Example
// Tests move construction
void move_example() {
	std::cout << "Move Example\n";

	HashMap<int, std::string> m1;
	m1.insert(1, "Egg");

	HashMap<int, std::string> m2(std::move(m1));

	std::cout << m2.at(1) << "\n";

	std::cout << "[DONE]\n\n";
}

// Lifecycle Runner
// Runs basic, copy, and move tests
void lifecycle() {
	basic_example();
	copy_example();
	move_example();
}

// at() Example
// Tests exception-safe access
void at_example() {
	std::cout << "At Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << "m1.at(1) = " << m1.at(1) << "\n";
	std::cout << "m1.at(2) = " << m1.at(2) << "\n";

	try {
		std::cout << m1.at(999) << "\n";
	} catch (const std::out_of_range& e) {
		std::cout << "Exception: " << e.what() << "\n";
	}

	std::cout << "[DONE]\n\n";
}

// operator[] Example
// Tests insert + overwrite behavior
void operator_bracket_example() {
	std::cout << "Operator[] Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << "m1[1] = " << m1[1] << "\n";

	std::cout << "m1[3] before assignment = " << m1[3] << "\n";

	m1[3] = "Cherry";

	std::cout << "m1[3] after assignment = " << m1[3] << "\n";

	m1[1] = "Green Apple";

	std::cout << "m1[1] updated = " << m1[1] << "\n";

	std::cout << "[DONE]\n\n";
}

// Access Runner
// Runs at() and operator[] tests
void access() {
	at_example();
	operator_bracket_example();
}

// Insert Example
// Tests insert operation
void insert_example() {
	std::cout << "Insert Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(2) << "\n";
	std::cout << m1.at(3) << "\n";

	std::cout << "[DONE]\n\n";
}

// Update Example
// Tests value updates
void update_example() {
	std::cout << "Update Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	(void)m1.update(1, "Green Apple");
	(void)m1.update(2, "Yellow Banana");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(2) << "\n";

	std::cout << "[DONE]\n\n";
}

// Erase Example
// Tests deletion
void erase_example() {
	std::cout << "Erase Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	(void)m1.erase(2);

	std::cout << "contains 2? " << m1.contains(2) << "\n";

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(3) << "\n";

	std::cout << "[DONE]\n\n";
}

// Clear Example
// Tests full container reset
void clear_example() {
	std::cout << "Clear Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	std::cout << "size before clear = " << m1.size() << "\n";

	m1.clear();

	std::cout << "size after clear = " << m1.size() << "\n";
	std::cout << "empty? " << m1.empty() << "\n";

	std::cout << "[DONE]\n\n";
}

// Modifier Runner
// Groups insert, update, erase, clear tests
void modifier() {
	insert_example();
	update_example();
	erase_example();
	clear_example();
}

// Find Example
// Tests iterator return from find()
void find_example() {
	std::cout << "Find Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	auto it = m1.find(1);

	if (it != m1.end()) {
		std::cout << "Found key 1: " << it->value << "\n";
	}

	auto it2 = m1.find(999);

	if (it2 == m1.end()) {
		std::cout << "Key 999 not found\n";
	}

	std::cout << "[DONE]\n\n";
}

// Contains Example
// Tests key existence check
void contains_example() {
	std::cout << "Contains Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << "contains 1? " << m1.contains(1) << "\n";
	std::cout << "contains 2? " << m1.contains(2) << "\n";
	std::cout << "contains 999? " << m1.contains(999) << "\n";

	std::cout << "[DONE]\n\n";
}

// Lookup Runner
// Groups find and contains tests
void lookup() {
	find_example();
	contains_example();
}

// Size Example
// Tests size tracking
void size_example() {
	std::cout << "Size Example\n";

	HashMap<int, std::string> m1;

	std::cout << "size = " << m1.size() << "\n";

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << "size after inserts = " << m1.size() << "\n";

	(void)m1.erase(1);

	std::cout << "size after erase = " << m1.size() << "\n";

	m1.clear();

	std::cout << "size after clear = " << m1.size() << "\n";

	std::cout << "[DONE]\n\n";
}

// Capacity Example
// Tests bucket capacity initialization
void capacity_example() {
	std::cout << "Capacity Example\n";

	HashMap<int, std::string> m1(16);

	std::cout << "initial capacity = 16\n";

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	std::cout << "size = " << m1.size() << "\n";

	m1.clear();

	std::cout << "size after clear = " << m1.size() << "\n";

	std::cout << "[DONE]\n\n";
}

// Rehash Example
// Tests automatic resizing (load factor behavior)
void rehash_example() {
	std::cout << "Rehash Example\n";

	HashMap<int, std::string> m1(4);

	std::cout << "initial bucketCount = 4\n";

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");

	std::cout << "size = " << m1.size() << "\n";

	m1.insert(3, "Cherry");

	std::cout << "after insert 3:\n";
	std::cout << "size = " << m1.size() << "\n";

	m1.insert(4, "Date");

	std::cout << "after insert 4 (should trigger rehash if load > 0.75):\n";
	std::cout << "size = " << m1.size() << "\n";

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(4) << "\n";

	std::cout << "[DONE]\n\n";
}

// Capacity Runner
// Groups size, capacity, and rehash tests
void capacity() {
	size_example();
	capacity_example();
	rehash_example();
}

// Forward Iterator Example
// Tests iteration from begin to end
void forward_iterator_example() {
	std::cout << "Forward Iterator Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	for (auto it = m1.begin(); it != m1.end(); ++it) {
		std::cout << it->key << " -> " << it->value << "\n";
	}

	std::cout << "[DONE]\n\n";
}

// Reverse Iterator Example
// Tests reverse iteration from rbegin to rend
void reverse_iterator_example() {
	std::cout << "Reverse Iterator Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	for (auto it = m1.rbegin(); it != m1.rend(); ++it) {
		std::cout << it->key << " -> " << it->value << "\n";
	}

	std::cout << "[DONE]\n\n";
}

// Iterator Runner
// Runs forward and reverse iterator tests
void iterator() {
	forward_iterator_example();
	reverse_iterator_example();
}

// Default Hash Example
// Uses std::hash
void default_hash_example() {
	std::cout << "Default Hash Example\n";

	HashMap<int, std::string> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(2) << "\n";
	std::cout << m1.at(3) << "\n";

	std::cout << "[DONE]\n\n";
}

// Collision Hash Example
// Forces collisions using constant hash
void collision_hash_example() {
	struct CollisionHash {
		std::size_t operator()(const int&) const {
			return 0;
		}
	};

	std::cout << "Collision Hash Example\n";

	HashMap<int, std::string, CollisionHash> m1;

	m1.insert(1, "Apple");
	m1.insert(2, "Banana");
	m1.insert(3, "Cherry");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(2) << "\n";
	std::cout << m1.at(3) << "\n";

	std::cout << "[DONE]\n\n";
}

// Modulo Hash Example
// Uses key % 5 as hash function
void modulo_hash_example() {
	struct ModuloHash {
		std::size_t operator()(const int& key) const {
			return key % 5;
		}
	};

	std::cout << "Modulo Hash Example\n";

	HashMap<int, std::string, ModuloHash> m1;

	m1.insert(1, "Apple");
	m1.insert(6, "Banana");
	m1.insert(11, "Cherry");
	m1.insert(2, "Date");
	m1.insert(7, "Elderberry");

	std::cout << m1.at(1) << "\n";
	std::cout << m1.at(6) << "\n";
	std::cout << m1.at(11) << "\n";

	std::cout << "[DONE]\n\n";
}

// Hash Runner
// Tests all hash strategies
void hash() {
	default_hash_example();
	collision_hash_example();
	modulo_hash_example();
}

// Main entry
int main() {
	lifecycle();
	access();
	modifier();
	lookup();
	capacity();
	iterator();
	hash();

	return 0;
}

