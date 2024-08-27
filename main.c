#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct xorshiftr128plus_state {
	uint64_t x, y;
};

struct xorshiftr128plus_state randomState = {0};



static inline uint64_t splitmx64(uint64_t *s) {
	*s += 0x9e779b97f4a7c15;
	uint64_t result = *s;
	result ^= (result >> 30) * 0Xbf58476d1ce4e5b9;
	result ^= (result >> 27) * 0X94d049bb133111eb;
	result ^= result >> 31;
	return result;
}
static inline struct xorshiftr128plus_state seed(uint64_t seed) {
	return (struct xorshiftr128plus_state) {
		.x = splitmx64(&seed),
		.y = splitmx64(&seed)
	};
}
static inline uint64_t xorshiftr128plus(struct xorshiftr128plus_state *state) {
	
	uint64_t x = state->x;
	state->x = state->y;
	
	x ^= x << 23;
	x ^= x >> 17;
	x ^= state->y;

	state->y += x;

	return x;
}


uint8_t roll() {
	uint64_t randomNumber = 0;
	while (1) {
		randomNumber = xorshiftr128plus(&randomState) & 0x07;
		if (randomNumber < 6) return randomNumber;
	}
}

int main(int argc, char **argv) {

	randomState = seed(1);
	printf("randomState post-seed(): %lu, %lu\n", randomState.x, randomState.y);

	uint64_t rollCounts[6] = {0};

	for (int i = 0; i < 1000000000; ++i) {
		rollCounts[roll()]++;
	}

	for (int i = 0; i < 6; ++i) {
		printf("rollCounts[%d]: %lu\n", i, rollCounts[i]);
	}

	return 0;
}
