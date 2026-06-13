// HashMap Test Suite
// Verifies correctness of the hash table container through unit tests:
//
// - lifecycle (default, copy, and move construction)
// - access (at, operator[])
// - modifiers (insert, update, erase, clear)
// - lookup (find, contains)
// - capacity management (size, automatic rehash)
// - iterators (forward and reverse traversal)
// - default, collision, and custom hash functions
//
// These tests validate the correctness and expected behavior of HashMap.

#include <iostream>
#include <cassert>
#include <string>
#include <cstddef>

#include "HashMap.h"

// Lifecycle Test
// Verifies default, copy, and move construction.
void lifecycle() {
	std::cout << "Lifecycle Test\n";
	HashMap<int, int> m1;
	m1.insert(1, 100);

	assert(m1.capacity() == 16);
	assert(m1.contains(1));

	std::cout << "[PASS] Default Construction\n";

	HashMap<int, int> m2(m1);
	(void)m2.update(1, 200);

	assert(m1.at(1) == 100);
	assert(m2.capacity() == 16);
	assert(m2.at(1) == 200);

	std::cout << "[PASS] Copy Construction\n";

	HashMap<int, int> m3(std::move(m1));

	assert(m1.capacity() == 0);
	assert(m1.empty());
	assert(m3.capacity() == 16);
	assert(m3.contains(1));

	std::cout << "[PASS] Move Construction\n\n";
}

// Access Test
// Verifies at() and operator[] behavior.
void access() {
	std::cout << "Access Test\n";

	HashMap<int, std::string> m1;
	m1.insert(21, "Banana");
	m1.insert(22, "Apple");
	m1.insert(23, "Mango");

	assert(m1.at(21) == "Banana");
	assert(m1.at(22) == "Apple");
	assert(m1.at(23) == "Mango");
	assert(m1.size() == 3);

	try {
		(void)m1.at(24);
		assert(false);
	}
	catch(const std::out_of_range&) {}

	std::cout << "[PASS] At Operation\n";

	assert(m1[24] == "");
	assert(m1.size() == 4);

	std::cout << "[PASS] Operation[]\n\n";
}

// Modifier Test
// Verifies insert, update, erase, and clear operations.
void modifier() {
	std::cout << "Modifier Test\n";

	HashMap<int, double> m1;

	m1.insert(10, 1.10);
	m1.insert(11, 2.25);
	m1.insert(12, 8.5);
	m1.insert(10, 86.21);

	assert(m1.size() == 3);
	assert(m1[10] == 1.10);
	assert(m1[12] == 8.5);

	std::cout << "[PASS] Insert\n";

	assert(m1.update(10, 89.90));
	assert(!m1.update(15, 68.7));
	assert(m1.at(10) == 89.90);

	std::cout << "[PASS] Update\n";

	assert(m1.erase(11));
	assert(!m1.erase(13));
	assert(m1.size() == 2);

	std::cout << "[PASS] Erase\n";

	assert(!m1.empty());

	m1.clear();

	assert(m1.size() == 0);
	assert(m1.empty());

	std::cout << "[PASS] Empty\n\n";
}

// Lookup Test
// Verifies find() and contains().
void lookup() {
	std::cout << "Lookup Test\n";

	HashMap<std::string, int> m1;

	m1.insert("x1", 100);
	m1.insert("y1", 200);

	auto it = m1.find("x1");

	assert(it != m1.end());
	assert(it->key == "x1");
	assert(it->value == 100);
	assert(m1.find("z1") == m1.end());

	std::cout << "[PASS] Find\n";

	assert(m1.contains("x1"));
	assert(m1.contains("y1"));
	assert(!m1.contains("z1"));

	std::cout << "[PASS] Contains\n\n";
}

// Capacity Test
// Verifies size tracking and automatic rehashing.
void capacity() {
	std::cout << "Capacity Test\n";

	HashMap<double, int> m1(5);

	m1.insert(1.5, 5000);

	assert(m1.size() == 1);

	m1.insert(2.5, 10000);
	m1.insert(3.5, 15000);

	assert(m1.size() == 3);
	assert(m1.capacity() == 5);

	std::cout << "[PASS] Size\n";

	m1.insert(4.5, 10000);

	assert(m1.capacity() > 5);

	std::cout << "[PASS] Rehash\n\n";
}

// Iterator Test
// Verifies forward and reverse iteration.
void iterator() {
	std::cout << "Iterator Test\n";

	HashMap<char, std::string> m1;

	m1.insert('a', "Aa");
	m1.insert('b', "Bb");
	m1.insert('c', "Cc");


	std::size_t fCount = 0;

	for(auto it = m1.begin(); it != m1.end(); ++it) fCount++;

	assert(fCount == m1.size());

	std::cout << "[PASS] Forward Iteration\n";

	std::size_t rCount = 0;

	for(auto it = m1.rbegin(); it != m1.rend(); ++it) rCount++;

	assert(rCount == m1.size());

	std::cout << "[PASS] Reverse Iteration\n\n";
}

// Hash Test
// Verifies default, collision, and custom hash functions.
void hash() {
	std::cout << "Hash Test\n";

	{
		HashMap<int, int> m1;

		m1.insert(1, 999);
		m1.insert(10, 99);

		assert(m1.size() == 2);
		assert(m1.contains(1));
		assert(m1.contains(10));
		assert(m1.at(1) == 999);
		assert(m1.at(10) == 99);

		std::cout << "[PASS] Default Hash\n";
	}

	{
		struct CollisionHash {
			std::size_t operator()(int) const {
				return 0;
			}
		};

		HashMap<int, int, CollisionHash> m2;

		m2.insert(56, 1000);
		m2.insert(65, 2000);

		assert(m2.size() == 2);
		assert(m2.contains(56));
		assert(m2.contains(65));
		assert(m2.at(56) == 1000);
		assert(m2.at(65) == 2000);

		std::cout << "[PASS] Collision Hash\n";
	}

	{
		struct ModuloHash {
			std::size_t operator()(int key) const {
				return static_cast<std::size_t>(key % 5);
			}
		};

		HashMap<int, int, ModuloHash> m3;

		m3.insert(30, 500);
		m3.insert(70, 400);

		assert(m3.size() == 2);
		assert(m3.contains(30));
		assert(m3.contains(70));
		assert(m3.at(30) == 500);
		assert(m3.at(70) == 400);

		std::cout << "[PASS] Modulo Hash\n";
	}
}

// Main Entry
// Runs all HashMap test groups.
int main()
{
	lifecycle();
	access();
	modifier();
	lookup();
	capacity();
	iterator();
	hash();

	return 0;
}



