#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include "external/cmd_line_parser/include/parser.hpp"

#include "include/hashers.hpp"

using namespace hasher;

std::vector<std::string> read_string_collection(uint64_t n, char const* filename) {
    std::ifstream input(filename);
    if (!input.good()) throw std::runtime_error("Error in opening file.");
    std::string s;
    uint64_t max_string_length = 0;
    uint64_t sum_of_lengths = 0;
    std::vector<std::string> strings;
    while (std::getline(input, s)) {
        if (s.size() > max_string_length) max_string_length = s.size();
        sum_of_lengths += s.size();
        strings.push_back(s);
        if (strings.size() % 1000000 == 0) {
            std::cout << "read " << strings.size() << " strings" << std::endl;
        }
        if (strings.size() == n) break;
    }
    input.close();
    strings.shrink_to_fit();
    std::cout << "num_strings " << strings.size() << std::endl;
    std::cout << "max_string_length " << max_string_length << std::endl;
    std::cout << "total_length " << sum_of_lengths << std::endl;
    std::cout << "avg_string_length " << std::fixed << std::setprecision(2)
              << static_cast<double>(sum_of_lengths) / strings.size() << std::endl;
    return strings;
}

template <typename Uint>
std::vector<Uint> distinct_keys(uint64_t n) {
    auto gen = std::mt19937_64(std::random_device()());
    std::vector<Uint> keys(n * 1.05);              // allocate a vector slightly larger than needed
    std::generate(keys.begin(), keys.end(), gen);  // fill with random numbers
    std::sort(keys.begin(), keys.end());           // sort the keys
    std::unique(keys.begin(), keys.end());         // remove the duplicates
    uint64_t size = keys.size();
    while (size < n) {  // unlikely if max is large and n * 1.05 > n
        keys.push_back(keys.back() + 1);
        size++;
    }
    assert(std::adjacent_find(keys.begin(), keys.end()) == keys.end());  // must be all distinct
    std::shuffle(keys.begin(), keys.end(), gen);
    if (keys.size() > n) keys.resize(n);
    assert(keys.size() == n);
    return keys;
}

template <typename Iterator, typename Hasher>
void perf(Iterator keys, uint64_t n) {
    std::cout << "\n=== " << Hasher::name() << " ===" << std::endl;

    typedef typename Hasher::hash_type hash_type;
    typedef std::chrono::high_resolution_clock clock_type;
    static const uint64_t runs = 5;
    uint64_t sum = 0;
    Hasher h;
    uint64_t seed = std::random_device()();

    auto start = clock_type::now();
    for (uint64_t r = 0; r != runs; ++r) {
        Iterator begin = keys;
        for (uint64_t i = 0; i != n; ++i) {
            auto const& key = *begin;
            hash_type hash = h.hash(key, seed);
            sum += hash;
            ++begin;
        }
    }
    auto stop = clock_type::now();
    double elapsed = static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count());
    double nanosec_per_key = elapsed / (runs * n);

    std::cout << "#ignore: " << sum << std::endl;
    std::cout << "Hash -- nanosec_per_key = " << nanosec_per_key << std::endl;

    {
        sum = 0;
        uint64_t num_buckets = n / 100;
        if (num_buckets == 0) return;
        // std::vector<uint64_t> sizes(num_buckets, 0);
        auto start = clock_type::now();
        for (uint64_t r = 0; r != runs; ++r) {
            Iterator begin = keys;
            for (uint64_t i = 0; i != n; ++i) {
                auto const& key = *begin;
                hash_type hash = h.hash(key, seed);
                uint64_t bucket = h.mod(hash, num_buckets);
                // ++sizes[bucket];
                sum += bucket;
                ++begin;
            }
        }
        auto stop = clock_type::now();
        double elapsed = static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count());
        double nanosec_per_key = elapsed / (runs * n);

        std::cout << "#ignore: " << sum << std::endl;
        std::cout << "Hash+Mod -- nanosec_per_key = " << nanosec_per_key << std::endl;

        // for (auto x : sizes) std::cout << x << " ";
        // std::cout << std::endl;
    }
}

template <typename Iterator>
void perf(Iterator keys, uint64_t n, std::string const& hasher_type) {
    if (hasher_type == "") {
        std::cout << "\n*** No hasher specified: testing all hashers..." << std::endl;
        perf<Iterator, murmurhash2_64>(keys, n);
        perf<Iterator, murmurhash3_128>(keys, n);
        perf<Iterator, cityhash_64>(keys, n);
        perf<Iterator, cityhash_128>(keys, n);
        perf<Iterator, spookyhash_64>(keys, n);
        perf<Iterator, spookyhash_128>(keys, n);
    }

    else if (hasher_type == "murmurhash2_64") {
        perf<Iterator, murmurhash2_64>(keys, n);
    } else if (hasher_type == "murmurhash3_128") {
        perf<Iterator, murmurhash3_128>(keys, n);
    }

    else if (hasher_type == "cityhash_64") {
        perf<Iterator, cityhash_64>(keys, n);
    } else if (hasher_type == "cityhash_128") {
        perf<Iterator, cityhash_128>(keys, n);
    }

    else if (hasher_type == "spookyhash_64") {
        perf<Iterator, spookyhash_64>(keys, n);
    } else if (hasher_type == "spookyhash_128") {
        perf<Iterator, spookyhash_128>(keys, n);
    }

    else {
        std::cout << "unknown hasher_type" << std::endl;
    }
}

int main(int argc, char** argv) {
    cmd_line_parser::parser parser(argc, argv);
    parser.add("n", "The size of the input.");
    parser.add("hasher_type",
               "Hasher type. See include/hashers.hpp for a list of available types. If not "
               "specified, all hashers will be tested.",
               "--hasher", false);
    parser.add("input_filename",
               "A string input file name. If not provided, then n 64-bit random keys will "
               "be used as input instead.",
               "--input", false);
    if (!parser.parse()) return 1;

    auto n = parser.get<uint64_t>("n");

    std::string hasher_type("");
    if (parser.parsed("hasher_type")) hasher_type = parser.get<std::string>("hasher_type");

    if (parser.parsed("input_filename")) {
        auto input_filename = parser.get<std::string>("input_filename");
        std::vector<std::string> keys = read_string_collection(n, input_filename.c_str());
        perf(keys.begin(), keys.size(), hasher_type);
    } else {  // use n random 64-bit keys
        std::vector<uint64_t> keys = distinct_keys<uint64_t>(n);
        perf(keys.begin(), keys.size(), hasher_type);
    }

    return 0;
}