#pragma once

#include "external/smhasher/src/MurmurHash2.h"
#include "external/smhasher/src/MurmurHash3.h"
#include "external/smhasher/src/City.h"
#include "external/smhasher/src/Spooky.h"

namespace hasher {

struct murmurhash2_64 {
    typedef uint64_t hash_type;

    static std::string name() {
        return "murmurhash2_64";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        return MurmurHash64A(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        return MurmurHash64A(&val, sizeof(val), seed);
    }
};

struct murmurhash3_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "murmurhash3_128";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        hash_type out;
        MurmurHash3_x86_128(val.data(), val.size(), seed, &out);
        return out;
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        hash_type out;
        MurmurHash3_x86_128(&val, sizeof(val), seed, &out);
        return out;
    }
};

struct cityhash_64 {
    typedef uint64_t hash_type;

    static std::string name() {
        return "cityhash_64";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        return CityHash64WithSeed(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        return CityHash64WithSeed(reinterpret_cast<char const*>(&val), sizeof(val), seed);
    }
};

struct cityhash_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "cityhash_128";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        auto ret = CityHash128WithSeed(val.data(), val.size(), {seed, seed});
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = ret.first;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = ret.second;
        return out;
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        auto ret =
            CityHash128WithSeed(reinterpret_cast<char const*>(&val), sizeof(val), {seed, seed});
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = ret.first;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = ret.second;
        return out;
    }
};

struct spookyhash_64 {
    typedef uint64_t hash_type;

    static std::string name() {
        return "spookyhash_64";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        return SpookyHash::Hash64(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        return SpookyHash::Hash64(&val, sizeof(val), seed);
    }
};

struct spookyhash_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "spookyhash_128";
    }

    // specialization for std::string
    hash_type hash(std::string const& val, uint64_t seed) {
        uint64_t h1 = seed, h2 = seed;
        SpookyHash::Hash128(val.data(), val.size(), &h1, &h2);
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = h1;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = h2;
        return out;
    }

    // specialization for uint64_t
    hash_type hash(uint64_t val, uint64_t seed) {
        uint64_t h1 = seed, h2 = seed;
        SpookyHash::Hash128(&val, sizeof(val), &h1, &h2);
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = h1;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = h2;
        return out;
    }
};

}  // namespace hasher
