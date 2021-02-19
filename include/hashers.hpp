#pragma once

#include "external/smhasher/src/MurmurHash2.h"
#include "external/smhasher/src/MurmurHash3.h"
#include "external/smhasher/src/City.h"
#include "external/smhasher/src/Spooky.h"

namespace hasher {

inline uint64_t mod64(uint64_t h, uint64_t n) {
    // h % n is much slower (~2X)
    return (uint64_t)(((__uint128_t)(h >> 32) * (__uint128_t)n) >> 32);
}

inline uint64_t mod128(__uint128_t h, uint64_t n) {
    return (uint64_t)(((h >> 64) * (__uint128_t)n) >> 64);
}

std::ostream& operator<<(std::ostream& os, __uint128_t const x) {
    static __uint128_t mask = (__uint128_t(1) << 64) - 1;
    uint64_t first = (uint64_t)(x & mask);
    uint64_t second = (uint64_t)(x >> 64);
    os << first << second;
    return os;
}

struct murmurhash2_64 {
    typedef uint64_t hash_type;

    static std::string name() {
        return "murmurhash2_64";
    }

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod64(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        return MurmurHash64A(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
        return MurmurHash64A(&val, sizeof(val), seed);
    }
};

struct murmurhash3_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "murmurhash3_128";
    }

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod128(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        hash_type out;
        MurmurHash3_x86_128(val.data(), val.size(), seed, &out);
        return out;
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
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

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod64(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        return CityHash64WithSeed(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
        return CityHash64WithSeed(reinterpret_cast<char const*>(&val), sizeof(val), seed);
    }
};

struct cityhash_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "cityhash_128";
    }

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod128(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        auto ret = CityHash128WithSeed(val.data(), val.size(), {seed, seed});
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = ret.first;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = ret.second;
        return out;
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
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

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod64(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        return SpookyHash::Hash64(val.data(), val.size(), seed);
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
        return SpookyHash::Hash64(&val, sizeof(val), seed);
    }
};

struct spookyhash_128 {
    typedef __uint128_t hash_type;

    static std::string name() {
        return "spookyhash_128";
    }

    static uint64_t mod(hash_type h, uint64_t n) {
        return mod128(h, n);
    }

    // specialization for std::string
    static hash_type hash(std::string const& val, uint64_t seed) {
        uint64_t h1 = seed, h2 = seed;
        SpookyHash::Hash128(val.data(), val.size(), &h1, &h2);
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = h1;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = h2;
        return out;
    }

    // specialization for uint64_t
    static hash_type hash(uint64_t val, uint64_t seed) {
        uint64_t h1 = seed, h2 = seed;
        SpookyHash::Hash128(&val, sizeof(val), &h1, &h2);
        hash_type out;
        *(reinterpret_cast<uint64_t*>(&out) + 0) = h1;
        *(reinterpret_cast<uint64_t*>(&out) + 1) = h2;
        return out;
    }
};

}  // namespace hasher
