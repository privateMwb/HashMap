// HashMap vs std::unordered_map Benchmark Suite
// Measures performance of core hash table operations:
//
// - insert (normal insertion performance)
// - lookup (key search using contains/count)
// - erase (removal by key)
// - update (modifying existing values)
// - operator[] (element access performance)
// - insert with rehash (growth and rehash overhead)
// - iteration (traversal through all elements)
// - copy construction (deep copy performance)
// - move construction (ownership transfer cost)
// - mixed workload (combined insert, lookup, update, and erase)
//
// Benchmarks compare HashMap with std::unordered_map.

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>

#include "HashMap.h"
#include "utils/Table.h"

// Benchmark Timer
// measures the execution time of a callable and returns
template<typename F>
auto duration(F&& func) {
	auto start = std::chrono::steady_clock::now();

	func();

	auto end = std::chrono::steady_clock::now();

	return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

// Compiler Optimization Barrier
// prevents the compiler from optimizing away values
template<typename T>
void black_box(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
	asm volatile("" : : "g"(value) : "memory");
#else
	volatile const T& ptr = &value;
	(void)ptr;
#endif
}

// Insert Benchmark
// measures insertion performance under normal growth
void insert() {
	static constexpr std::size_t SMALL = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG = 1'000'000;

	std::vector<std::size_t> config = {SMALL, MEDIUM, BIG};
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		HashMap<int, int> m1;

		for(std::size_t i = 0; i < config.size(); ++i) {
			auto m1_duration = duration([&]() {
				for(std::size_t j = 0; j < config[i]; ++j) {
					m1.insert(j, j);
					black_box(&m1);
				}
				m1.clear();
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		std::unordered_map<int, int> m2;

		for(std::size_t i = 0; i < config.size(); ++i) {
			auto m2_duration = duration([&]() {
				for(std::size_t j = 0; j < config[i]; ++j) {
					m2.insert({j, j});
					black_box(&m2);
				}
				m2.clear();
			});
			
			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Insert Benchmark",
	{"Count", "HashMap", "std::unordered_map"},
	data, 60);
}

// Lookup Benchmark
// measures key lookup performance for existing elements
void lookup() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m1.contains(j));
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m2.count(j));
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Lookup Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Erase Benchmark
// measures removal performance by key
void erase() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m1.erase(j));
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m2.erase(j));
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Erase Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Update Benchmark
// measures performance when updating existing mapped values
void update() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m1.update(j, j + 1));
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					m2[j] = j + 1;
					black_box(m2[j]);
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Update Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Operator[] Benchmark
// measures element access performance using operator[]
void operatorBracket() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m1[j]);
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					black_box(m2[j]);
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Operator[] Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Insert With Rehash Benchmark
// measures insertion performance when repeated rehashing occurs
void insertWithRehash() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap — starts with 1 bucket to force maximum rehashing
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1(1);

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					m1.insert(j, j);
					black_box(&m1);
				}
				m1.clear();
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map — starts with 1 bucket
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2(1);

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					m2.insert({ j, j });
					black_box(&m2);
				}
				m2.clear();
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Insert With Rehash Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Iteration Benchmark
// measures traversal performance across all stored elements
void iteration() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				for (auto it = m1.begin(); it != m1.end(); ++it) {
					black_box(it->value);
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				for (auto it = m2.begin(); it != m2.end(); ++it) {
					black_box(it->second);
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Iteration Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Copy Construction Benchmark
// measures deep copy performance for populated hash tables
void copyConstruct() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				HashMap<int, int> copy(m1);
				black_box(&copy);
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				std::unordered_map<int, int> copy(m2);
				black_box(&copy);
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Copy Construction Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Move Construction Benchmark
// measures ownership transfer performance using move construction
void moveConstruct() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			for (std::size_t j = 0; j < config[i]; ++j)
				m1.insert(j, j);

			auto m1_duration = duration([&]() {
				HashMap<int, int> moved(std::move(m1));
				black_box(&moved);
				m1 = std::move(moved);
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			for (std::size_t j = 0; j < config[i]; ++j)
				m2.insert({ j, j });

			auto m2_duration = duration([&]() {
				std::unordered_map<int, int> moved(std::move(m2));
				black_box(&moved);
				m2 = std::move(moved);
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Move Construction Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Mixed Workload Benchmark
// measures realistic mixed operations including insert, lookup, update, and erase
void mixedWorkload() {
	static constexpr std::size_t SMALL  = 100'000;
	static constexpr std::size_t MEDIUM = 500'000;
	static constexpr std::size_t BIG    = 1'000'000;

	std::vector<std::size_t> config = { SMALL, MEDIUM, BIG };
	std::vector<long> m1_durations;
	std::vector<long> m2_durations;

	// HashMap
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			HashMap<int, int> m1;

			auto m1_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					m1.insert(j, j);

					if (j % 3 == 0) black_box(m1.contains(j));
					if (j % 5 == 0) black_box(m1.update(j, j + 1));
					if (j % 7 == 0) black_box(m1.erase(j));
				}
			});

			m1_durations.push_back(m1_duration.count());
		}
	}

	// std::unordered_map
	{
		for (std::size_t i = 0; i < config.size(); ++i) {
			std::unordered_map<int, int> m2;

			auto m2_duration = duration([&]() {
				for (std::size_t j = 0; j < config[i]; ++j) {
					m2.insert({ j, j });

					if (j % 3 == 0) black_box(m2.count(j));
					if (j % 5 == 0) { m2[j] = j + 1; black_box(m2[j]); }
					if (j % 7 == 0) black_box(m2.erase(j));
				}
			});

			m2_durations.push_back(m2_duration.count());
		}
	}

	std::vector<std::vector<std::string>> data{
		Table::convert(config),
		Table::convert(m1_durations, "us"),
		Table::convert(m2_durations, "us")
	};

	Table::table(
	    "Mixed Workload Benchmark",
	    { "Count", "HashMap", "std::unordered_map" },
	    data, 60);
}

// Main Entry
int main() {
	insert();
	lookup();
	erase();
	update();
	operatorBracket();
	insertWithRehash();
	iteration();
	copyConstruct();
	moveConstruct();
	mixedWorkload();

	return 0;
}
