#include <CubeState.h>

CubeState::CubeState(std::array<int, 26> cubeletPermutations, std::array<int, 26> cubeletOrientations, std::array<int, 6> faceOrientations) {
	for (int i = 0; i < 26; i++)
	{
		this->cubeletPermutations[i] = cubeletPermutations[i];
		this->cubeletOrientations[i] = cubeletOrientations[i];
		if (i < 6)
			this->faceOrientations[i] = faceOrientations[i];
	}
}

CubeState::~CubeState() {
}

void CubeState::applyMove(int move) {
	applyMove(move, this->cubeletPermutations, this->cubeletOrientations, this->faceOrientations);
}

std::array<int, 26> CubeState::getCubeletPermutations() const {
	return this->cubeletPermutations;
}

std::array<int, 26> CubeState::getCubeletOrientations() const {
	return this->cubeletOrientations;
}

std::array<int, 6> CubeState::getFaceOrientations() const {
	return this->faceOrientations;
}

bool CubeState::isValid() const {
	int cornerOrientationSum = 0;
	for (int i = 0; i < 8; i++)
		cornerOrientationSum += this->cubeletOrientations[i];
	if (cornerOrientationSum % 3 != 0)
		return false;

	int edgeOrientationSum = 0;
	for (int i = 8; i < 20; i++)
		edgeOrientationSum += this->cubeletOrientations[i];
	if (edgeOrientationSum % 2 != 0)
		return false;

	if (parityOdd(this->cubeletPermutations, 20))
		return false;

	return true;
}

bool CubeState::parityOdd(std::span<const int> arr, int len) const {
	bool odd = false;
	for (int i = 0; i < len; i++)
		for (int j = 0; j < len; j++)
			odd ^= arr[i] < arr[j];

	return odd;
}

void CubeState::applyMove(int move, std::array<int, 26> cubeletPermutations, std::array<int, 26> cubeletOrientations, std::array<int, 6> faceOrientations) {
	int perm[26];
	int orient[26];
	int fOrient[6];

	for (int i = 0; i < 26; i++) {
		orient[i] = cubeletOrientations[this->moveTable[move][0][i]];
		if (orient[i] >= 0)
			orient[i] += this->moveTable[move][1][i];

		perm[i] = cubeletPermutations[this->moveTable[move][0][i]];
		if (perm[i] >= 0) {
			perm[i] = this->moveTable[move][2][perm[i]];
			if (perm[i] >= 0)
				orient[i] += this->moveTable[move][3][perm[i]];
		}
		if (orient[i] >= 0) {
			if (i < 8) {
				while (orient[i] > 2)
					orient[i] -= 3;
			}
			else if (i < 20) {
				while (orient[i] > 1)
					orient[i] -= 2;
			}
			else {
				while (orient[i] > 3)
					orient[i] -= 4;
			}
		}
	}
	for (int i = 0; i < 6; i++) {
		fOrient[i] = faceOrientations[this->moveTable[move][0][i + 20] - 20] + this->moveTable[move][4][i];
		while (fOrient[i] > 3)
			fOrient[i] -= 4;
	}

	for (int i = 0; i < 26; i++) {
		cubeletPermutations[i] = perm[i];
		cubeletOrientations[i] = orient[i];
		if (i < 6)
			faceOrientations[i] = fOrient[i];
	}
}

void CubeState::reflect(std::array<int, 26> cubeletPermutations, std::array<int, 26> cubeletOrientations, std::array<int, 6> faceOrientations) const {
	int perm[26];
	int orient[26];
	int fOrient[6];

	for (int i = 0; i < 26; i++) {
		perm[i] = cubeletPermutations[this->refPerm[i]];
		orient[i] = cubeletOrientations[this->refPerm[i]];

		if (orient[i] >= 0)
			if (i < 8)
				orient[i] ^= 3;
			else if (i >= 20)
				orient[i] = 4 - orient[i];

		if (i < 20 && perm[i] >= 0)
			perm[i] = this->refPerm[perm[i]];
	}

	for (int i = 0; i < 6; i++) {
		fOrient[i] = 4 - faceOrientations[this->refPerm[i + 20] - 20];
		if (i != 1 && i != 4)
			fOrient[i] ^= 2;
	}

	for (int i = 0; i < 26; i++) {
		cubeletPermutations[i] = perm[i];
		cubeletOrientations[i] = orient[i];
		if (i < 6)
			faceOrientations[i] = fOrient[i];
	}
}

void CubeState::sym(int move, std::array<int, 26> cubeletPermutations, std::array<int, 26> cubeletOrientations, std::array<int, 6> faceOrientations) {
	switch (move)
	{
	case 0:
		reflect(cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 1:
	case 2:
	case 3:
		sym(25 + move, cubeletPermutations, cubeletOrientations, faceOrientations);
		reflect(cubeletPermutations, cubeletOrientations, faceOrientations);
	case 4:
	case 5:
	case 6:
		sym(19 + move, cubeletPermutations, cubeletOrientations, faceOrientations);
		reflect(cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		sym(6 + move, cubeletPermutations, cubeletOrientations, faceOrientations);
		reflect(cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 13:
	case 14:
	case 15:
		sym(3 + move, cubeletPermutations, cubeletOrientations, faceOrientations);
		sym(3 + move, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 16:
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 17:
		applyMove(6, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(6, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(6, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 18:
		applyMove(8, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 19:
		applyMove(6, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 20:
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(6, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 21:
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		sym(17, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 22:
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(8, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 23:
		sym(22, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(8, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 24:
		sym(20, cubeletPermutations, cubeletOrientations, faceOrientations);
		applyMove(8, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 25:
		sym(22, cubeletPermutations, cubeletOrientations, faceOrientations);
		sym(17, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 26: 
		sym(21, cubeletPermutations, cubeletOrientations, faceOrientations);
		sym(17, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 27: 
		sym(19, cubeletPermutations, cubeletOrientations, faceOrientations); 
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations);
		break;
	case 28: 
		sym(20, cubeletPermutations, cubeletOrientations, faceOrientations); 
		applyMove(7, cubeletPermutations, cubeletOrientations, faceOrientations); 
		break;
	}
}
