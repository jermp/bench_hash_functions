# Benchmark hash functions

A simple benchmark to test the speed of some popular
hash functions, such as MurmurHash, CityHash, and SpookyHash.

The hashers in `include/hashers.hpp` are basically wrappers of the methods exposed by the [SMHasher](https://github.com/aappleby/smhasher) test suite.

#### Get and Compile the code

	git clone --recursive https://github.com/jermp/bench_hash_functions.git
	cd bench_hash_functions
	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release
	make

#### Run the benchmark

Run the `perf` executable. Some examples below.


- Example 1

		./perf 10000000

	will test all hashers on 10M random 64-bit keys.

- Example 2

		./perf 10000000 --input ~/Desktop/uk-2005.urls --hasher cityhash_64

	will test the hasher `cityhash_64` on 10M strings from the file `~/Desktop/uk-2005.urls`.


#### Results

Here are some results I collected on an Intel i9-9900K core (@3.60 GHz),
running Linux 5.
I used gcc 9.3.0 with all optimizations.

I measured the time it takes to compute the hash function.
Results are in average nanoseconds per key.

As input data, I used 100M keys: 64-bit keys, and strings from the GoogleBooks corpus
(average string length respectively equal to 13.97 and 19.06 for 2- and 3-grams.)

The suffix `_64` and `_128` indicates 64-bit and 128-bit hashes respectively.

|**Hasher** |**64-bit keys** | **GoogleBooks 2-grams**  | **GoogleBooks 3-grams**|
|-----------|---------------:|-------------------------:|-----------------------:|
| murmurhash2_64 | 2.80 | 8.10 | 9.48 |
| murmurhash3_128 | 9.98 | 14.21 | 15.09 |
| cityhash_64 | 2.97 | 5.08 | 5.51 |
| cityhash_128 | 6.29 | 8.98 | 9.81 |
| spookyhash_64 | 5.75 | 11.17 | 12.97 |
| spookyhash_128 | 5.73 | 11.33 | 13.15 |

From these results, **cityhash** is the clear winner.
