# Benchmark hash functions
--------------------------

A simple benchmark to test the speed of some popular
hash functions, such as MurmurHash, CityHash, and SpookyHash.

The hashers in `include/hashers.hpp` are basically wrappers of the methods exposed by the [SMHasher](https://github.com/aappleby/smhasher) test suite.

#### Compile the code

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