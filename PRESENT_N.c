#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ROUND_MAX_SIZE 2
#define N 2 // N <= 4 
#define MAX_bit 4 * N

// s-box
const unsigned short sBox4[] = { 0xc,0x5,0x6,0xb,0x9,0x0,0xa,0xd,0x3,0xe,0xf,0x8,0x4,0x7,0x1,0x2 };
const unsigned short decBox3[] = { 0x0000,0x2000,0x4000,0x6000,0x8000,0xa000,0xc000,0xe000 };
const unsigned short invsBox4[] = { 0x5,0xe,0xf,0x8,0xC,0x1,0x2,0xD,0xB,0x4,0x6,0x3,0x0,0x7,0x9,0xA };


typedef uint16_t u16;
typedef int16_t s16;

void ps(u16* state)
{
	printf("%x\n", state[0]);
	return;
}

// encryption version if operation = 0 
// decrypt    version if operation = 1
u16 SboxConversion(u16 in, int operation) {
	// exit when operation is neither 0 nor 1
	if(operation != 0 && operation != 1) exit(-1);

	u16 out = 0;
	int bit_tank = 0;
	for (int bit = 0; bit < MAX_bit; ++bit) {
			// get the bit of "in" from LSB
			bit_tank |= (in & 1) << (bit % 4);
			in >>= 1;

			// use s-box if bit_tank is 4 bits
			if ((bit + 1) % 4 == 0) {
				if(operation == 0) out |= sBox4[bit_tank] << (bit + 1 - 4);
				if(operation == 1) out |= invsBox4[bit_tank] << (bit + 1 - 4);
				bit_tank = 0;
			}
		}
	return out;
}

// encryption version if operation = 0 
// decrypt    version if operation = 1
u16 pLayerConversion(u16 in, int operation) {
	// exit when operation is neither 0 nor 1
	if(operation != 0 && operation != 1) exit(-1);

	u16 out = 0;

	for (int i = 0; i < MAX_bit; ++i) {
		if (i == MAX_bit - 1) {
			out ^= (in >> ((MAX_bit - 1) - i)) & 1;
		}
		else {
			// calculation of position depends on whether operation is encrypt or decrypt
			int pLayer_value = (operation == 0) ?  N : 4;
			int position = pLayer_value * i % (MAX_bit - 1);

			out ^= ((in >> ((MAX_bit - 1) - i)) & 1) << ((MAX_bit - 1) - position);
		}
	}
	return out;
}

void Encrypt(u16* state, u16* aKey)
{

	// counter
	u16 round = 1;
	u16 temp;
	// Variables Key Scheduling

	// Variables pLayer
	u16 temp_0;
	u16 temp_1;
	u16 temp_2;
	u16 temp_3;
	u16 j = 1;
	s16 position;
	s16 reste;
	u16 key[5];

	key[4] = aKey[4];
	key[3] = aKey[3];
	key[2] = aKey[2];
	key[1] = aKey[1];
	key[0] = aKey[0];

	for (round = 1; round < ROUND_MAX_SIZE; round++)
	{
		//	addRoundkey
		state[0] ^= key[1];

		// sBox
		state[0] = SboxConversion(state[0], 0);
		
		// pLayer
		state[0] = pLayerConversion(state[0], 0);
		//	-------------------* Key Scheduling -------------------***
				// <<61 ==(rot)== >>19	
		temp_0 = key[0];
		temp_1 = key[1];

		key[0] = key[1] >> 3;
		key[0] |= decBox3[key[2] & 0x07];

		key[1] = key[2] >> 3;
		key[1] |= decBox3[key[3] & 0x07];

		key[2] = key[3] >> 3;
		key[2] |= decBox3[key[4] & 0x07];

		key[3] = key[4] >> 3;
		key[3] |= decBox3[temp_0 & 0x07];

		key[4] = temp_0 >> 3;
		key[4] |= decBox3[temp_1 & 0x07];

		//sBox
		temp = key[4] >> 12;
		key[4] &= 0x0FFF;
		key[4] |= sBox4[temp] << 12;

		//Permutation
		if (round & 0x01)key[0] ^= 0x8000;
		key[1] ^= (round >> 1);
		//	-------------------* Key Scheduling End-------------------***
	}
	//	------------------- addRoundkey -------------------*******
	state[0] ^= key[1];

	//	------------------- addRoundkey End -------------------***
	return;
}


void Decrypt(u16* state, u16* aKey)
{
	// counter
	u16 round;
	// Variables Key Scheduling
	u16 subkey[31][4];
	u16 key[5];
	u16 temp;
	// Variables pLayer
	u16 j;
	u16 temp_0;
	u16 temp_1;
	u16 temp_2;
	u16 temp_3;
	s16 position;

	key[4] = aKey[4];
	key[3] = aKey[3];
	key[2] = aKey[2];
	key[1] = aKey[1];
	key[0] = aKey[0];

	//	------------------- Key Scheduling -------------------
	for (round = 1; round < 32; round++)
	{
		// <<61 ==(rot)== >>19	
		temp_0 = key[0];
		temp_1 = key[1];

		key[0] = key[1] >> 3;
		key[0] |= decBox3[key[2] & 0x07];

		key[1] = key[2] >> 3;
		key[1] |= decBox3[key[3] & 0x07];

		key[2] = key[3] >> 3;
		key[2] |= decBox3[key[4] & 0x07];

		key[3] = key[4] >> 3;
		key[3] |= decBox3[temp_0 & 0x07];

		key[4] = temp_0 >> 3;
		key[4] |= decBox3[temp_1 & 0x07];

		//sBox
		temp = key[4] >> 12;
		key[4] &= 0x0FFF;
		key[4] |= sBox4[temp] << 12;

		//Permutation
		if (round & 0x01)key[0] ^= 0x8000;
		key[1] ^= (round >> 1);

		subkey[round - 1][3] = key[4];
		subkey[round - 1][2] = key[3];
		subkey[round - 1][1] = key[2];
		subkey[round - 1][0] = key[1];
	}

	//	------------------- Key Scheduling End -------------------

	for (round = ROUND_MAX_SIZE - 1; round > 0; round--)
	{
		//	addRoundkey
		state[0] ^= subkey[round - 1][0];
		
		//	pLayer
		state[0] = pLayerConversion(state[0], 1);

		//	sBox
		state[0] = SboxConversion(state[0], 1);
	}
	//	------------------- addRoundkey -------------------
	state[0] ^= aKey[1];

	//	------------------- addRoundkey End -------------------
}



int main(void)
{
	u16 key[5] = { 0,0,0,0,0 };
	u16 state[1] = { 0x00 };

	ps(state);

	Encrypt(state, key);

	ps(state);

	Decrypt(state, key);

	ps(state);

	return 0;
}
