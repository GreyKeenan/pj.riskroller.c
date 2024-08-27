#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

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

uint8_t roll(void) {
	uint64_t randomNumber = 0;
	while (1) {
		randomNumber = xorshiftr128plus(&randomState) & 0x07;
		if (randomNumber < 6) return randomNumber;
	}
}



uint16_t stringToInt16(const char *string, uint16_t *destination) {
	int i = 0; //TODO err if wraps
	char c = 0;
	uint64_t total = 0;
	while (1) {
		c = string[i];
		if (c == '\0') {
			break;
		}
		if (i == INT_MAX) {
			return 3;
		}
		if (c < '0' || '9' < c) {
			*destination = total;
			return 1;
		}

		total *= 10;
		total += c - '0';

		if (total > 65535) {
			return 2;
		}

		i++;
	}

	*destination = total;
	return 0;
}
int processArguments(int argc, const char **argv, uint16_t *aggressorDestination, uint16_t *defenderDestination) {
	if (argc < 2) return 1;

	if (stringToInt16(argv[1], aggressorDestination)) return 2;

	if (argc < 3) {
		*defenderDestination = *aggressorDestination;
		return 0;
	}

	if (stringToInt16(argv[2], defenderDestination)) return 3;

	return 0;
}


void sortRolls(uint8_t rollCount, uint8_t *rolls) {
	for (int j = rollCount; j > 0; --j) {
		for (int i = 1; i < j; ++i) {
			if (rolls[i - 1] < rolls[i]) {
				rolls[i] += rolls[i - 1];
				rolls[i - 1] = rolls[i] - rolls[i - 1];
				rolls[i] -= rolls[i - 1];
			}
		}
	}
}

bool doBattle(uint16_t aggressor, uint16_t defender) {

	const uint16_t aggressorOrig = aggressor;
	const uint16_t defenderOrig = defender;

	printf("Attacker: %d /vs/ Defender: %d\n", aggressor, defender);

	uint8_t aggressorRolls[3] = {0};
	uint8_t defenderRolls[2] = {0};

	uint8_t aggressorRollCount = 0;
	uint8_t defenderRollCount = 0;
	while (1) {
		switch (aggressor) {
			case 0: 
				printf("\nAttacker: %d /vs/ Defender: %d\n", aggressorOrig, defenderOrig);
				printf("Defender wins with (%d) remaining!\n", defender);
				return false;
			case 1: //TODO retreat option at certain amount ??
				aggressorRollCount = 1;
				break;
			case 2:
				aggressorRollCount = 2;
				break;
			default:
				aggressorRollCount = 3;
				break;
		}
		switch (defender) {
			case 0: 
				printf("\nAttacker: %d /vs/ Defender: %d\n", aggressorOrig, defenderOrig);
				printf("Attacker wins with (%d) remaining!\n", aggressor);
				return true;
			case 1:
				defenderRollCount = 1;
				break;
			default:
				if (aggressorRollCount == 1) {
					defenderRollCount = 1;
					break;
				}
				defenderRollCount = 2;
				break;
		}
		
		for (int i = 0; i < aggressorRollCount; ++i) {
			aggressorRolls[i] = roll();
		}
		for (int i = 0; i < defenderRollCount; ++i) {
			defenderRolls[i] = roll();
		}
		sortRolls(aggressorRollCount, aggressorRolls);
		sortRolls(defenderRollCount, defenderRolls);

		
		int8_t aggressorLost = 0;
		int8_t defenderLost = 0;

		for (int i = 0; i < defenderRollCount; ++i) {
			if (aggressorRolls[i] > defenderRolls[i]) {
				//defender--;
				defenderLost++;
				continue;
			}

			//aggressor--;
			aggressorLost++;
			continue;
		}
		aggressor -= aggressorLost;
		defender -= defenderLost;

		printf("( %d, %c, %c ) /vs/ ( %d, %c ) -> -%d\t-%d\n",
			aggressorRolls[0] + 1,
			(aggressorRollCount > 1)? aggressorRolls[1] + '1' : 'X',
			(aggressorRollCount > 2)? aggressorRolls[2] + '1' : 'X',
			defenderRolls[0] + 1,
			(defenderRollCount > 1)? defenderRolls[1] + '1' : 'X',

			aggressorLost,
			defenderLost
		);
		
	}
}


int main(int argc, const char **argv) {

	uint64_t s = (uint64_t)time(NULL);
	randomState = seed(s);
	printf("\nseed: %lu\n", s);
	//printf("randomState post-seed: %lu, %lu\n", randomState.x, randomState.y);

	uint16_t aggressor = 0;
	uint16_t defender = 0;

	switch (processArguments(argc, argv, &aggressor, &defender)) {
		case 0: break;
		case 1: 
		case 2:
		case 3:
			printf("Unable to parse cli arguments.\n\n");
			return 1;
		default:
			printf("Unrecognized processArguments() return value.\n\n");
			return 1;
	}

	printf("\n");
	doBattle(aggressor, defender);
	printf("\n");

	return 0;
}
