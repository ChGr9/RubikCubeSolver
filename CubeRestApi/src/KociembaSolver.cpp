#include <KociembaSolver.h>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iostream>

KociembaSolver::KociembaSolver()
	:solutionMoves(std::make_unique<std::array<int, 40>>()),
	solutionAmmount(std::make_unique<std::array<int, 40>>()),
	positions(std::make_unique<std::array<std::array<int, 10>, 40>>()),
	transformationEdgeOrient(std::make_unique<std::array<std::array<int, 6>, 2048>>()),
	transformationCornerOrient(std::make_unique<std::array<std::array<int, 6>, 2187>>()),
	transformationEdgePerm(std::make_unique<std::array<std::array<int, 6>, 40320>>()),
	transformationCornerPerm(std::make_unique<std::array<std::array<int, 6>, 40320>>()),
	transformationSlice(std::make_unique<std::array<std::array<int, 6>, 11880>>()),
	transformationChoice(std::make_unique<std::array<std::array<int, 6>, 2048>>()),
	transformationSlicePerm(std::make_unique<std::array<std::array<int, 6>, 24>>()),
	transformationFace(std::make_unique<std::array<std::array<int, 6>, 4096>>()),
	pruneEdgeOrient(std::make_unique<std::array<char, 2048>>()),
	pruneCornerOrient(std::make_unique<std::array<char, 2187>>()),
	pruneEdgePerm(std::make_unique<std::array<char, 40320>>()),
	pruneCornerPerm(std::make_unique<std::array<char, 40320>>()),
	pruneChoice(std::make_unique<std::array<char, 2048>>()),
	pruneSlicePerm(std::make_unique<std::array<char, 24>>()),
	pruneFace1(std::make_unique<std::array<char, 4096>>()),
	pruneFace2(std::make_unique<std::array<char, 4096>>()),
	phase1Len(0),
	phase2Len(0),
	maxDepth(25),
	solutionLength(0) {
	initializeParameters();
}

KociembaSolver::~KociembaSolver() {
}

void KociembaSolver::reset() {
	initializeParameters();
}

bool KociembaSolver::setCubeState(const CubeState& cubeState) {
	if(!initialized || !cubeState.isValid())
		return false;

	for (auto& row : *positions) {
		row.fill(0);
	}

	(*positions)[0][0] = calculateEdgeOrientation(cubeState);
	std::cout << "Edge orientation: " << (*positions)[0][0] << std::endl;
	(*positions)[0][1] = calculateCornerOrientation(cubeState);
	std::cout << "Corner orientation: " << (*positions)[0][1] << std::endl;
	(*positions)[0][2] = getNFromPermutation(cubeState.getCubeletPermutations(), 8);
	std::cout << "Edge permutation: " << (*positions)[0][2] << std::endl;
	(*positions)[0][3] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 8);
	std::cout << "Corner permutation: " << (*positions)[0][3] << std::endl;
	(*positions)[0][4] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 12);
	std::cout << "Slice permutation: " << (*positions)[0][4] << std::endl;
	(*positions)[0][5] = getNFromPartialPermutation(cubeState.getCubeletPermutations(), 8, 12, 4, 16);
	std::cout << "Face permutation 1: " << (*positions)[0][5] << std::endl;
	(*positions)[0][6] = calculateMiddleSlice(cubeState);
	std::cout << "Middle slice: " << (*positions)[0][6] << std::endl;


	resetConfiguration();

	return true;
}

int KociembaSolver::calculateEdgeOrientation(const CubeState& cubeState) const {
	int edgeOrientation = 0;
	for (int i = 10; i >= 0; i--) {
		edgeOrientation = edgeOrientation * 2 + cubeState.getCubeletOrientations()[8 + i];
	}
	return edgeOrientation;
}

int KociembaSolver::calculateCornerOrientation(const CubeState& cubeState) const {
	int cornerOrientation = 0;
	for (int i = 6; i >= 0; i--) {
		cornerOrientation = cornerOrientation * 3 + cubeState.getCubeletOrientations()[i];
	}
	return cornerOrientation;
}

int KociembaSolver::calculateMiddleSlice(const CubeState& cubeState) const {
	int middleSlice = 0;
	for (int i = 10; i >= 0; i--) {
		middleSlice = middleSlice * 2 + (cubeState.getCubeletPermutations()[8 + i] >= 12 && cubeState.getCubeletPermutations()[8 + i] < 16);
	}
	return middleSlice;
}

void KociembaSolver::resetConfiguration() {
	solutionLength = phase1Len = phase2Len = 0;
	maxDepth = 25;
	solutionMoves->fill(-1);
	solutionAmmount->fill(3);
}

void KociembaSolver::initializeParameters() {
	initializeTransformations();
	initializePruneArrays();
	initialized = true;
}

void KociembaSolver::initializePruneArrays() {
	pruneEdgeOrient->at(0) = 1;
	initializePruneArray(*pruneEdgeOrient, *transformationEdgeOrient);
	pruneCornerOrient->at(0) = 1;
	initializePruneArray(*pruneCornerOrient, *transformationCornerOrient);
	pruneChoice->at(240) = 1;
	initializePruneArray(*pruneChoice, *transformationChoice);
	pruneCornerPerm->at(0) = 1;
	initializePruneCornerPerm(*pruneCornerPerm, *transformationCornerPerm);
	pruneEdgePerm->at(0) = 1;
	initializePruneArray(*pruneEdgePerm, *transformationEdgePerm);
	pruneSlicePerm->at(0) = 1;
	initializePruneArray(*pruneSlicePerm, *transformationSlicePerm);

	for (int i = 0; i < 4096; i++) {
		(*pruneFace1)[i] = 1;
		(*pruneFace1)[i] += ((i & 1) != 0) + ((i & 16) != 0) + ((i & 64) != 0) + ((i & 1024) != 0);

		(*pruneFace2)[i] = 1;
		(*pruneFace2)[i] += ((i & 3) != 0) + ((i & 12) != 0) + ((i & 48) != 0) +
			((i & 192) != 0) + ((i & 768) != 0) + ((i & 3072) != 0);
	}
}

void KociembaSolver::initializePruneCornerPerm(std::span<char> pruneArr, const auto& transArr) {
	int updateCount, l = 1;
	do {
		updateCount = 0;
		for (int i = 0; i < pruneArr.size(); i++)
			if (pruneArr[i] == l)
				for (int j = 0; j < 6; j++) {
					int p = i;
					for (int k = 0; k < 3; k++) {
						p = transArr[p][j];
						if ((j == 1 || j == 4 || k == 1) && pruneArr[p] == 0) {
							pruneArr[p] = static_cast<char>(l + 1);
							updateCount++;
						}
					}
				}
		l++;
	} while (updateCount != 0);
}

void KociembaSolver::initializePruneArray(std::span<char> pruneArr, const auto& transArr ) {
	int updateCount, l = 1;
	do {
		updateCount = 0;
		for (int i = 0; i < pruneArr.size(); i++)
			if (pruneArr[i] == l)
				for (int j = 0; j < 6; j++) {
					int p = i;
					for (int k = 0; k < 3; k++) {
						p = transArr[p][j];
						if (pruneArr[p] == 0) {
							pruneArr[p] = static_cast<char>(l + 1);
							updateCount++;
						}
					}
				}
		l++;
	} while (updateCount != 0);
}

template<typename Func, typename ArrayType>
void applyTransformation(ArrayType& arr, Func func) {
	const int sizeI = arr.size();
	for (int i = 0; i < sizeI; i++) {
		const int sizeJ = arr[i].size();
		for (int j = 0; j < sizeJ; j++)
			arr[i][j] = func(i, j);
	}

}

void KociembaSolver::initializeTransformations() {
	std::generate(transformationFace->begin(), transformationFace->end(), [this, i = 0]() mutable {
		std::array<int, 6> result = {
			((i + (3 << 10)) & ((1 << 12) - 1)),
			((i + (3 << 8)) & ((1 << 10) - 1)) + (i & ((1 << 12) - (1 << 10))),
			((i + (3 << 6)) & ((1 << 8) - 1)) + (i & ((1 << 12) - (1 << 8))),
			((i + (3 << 4)) & ((1 << 6) - 1)) + (i & ((1 << 12) - (1 << 6))),
			((i + (3 << 2)) & ((1 << 4) - 1)) + (i & ((1 << 12) - (1 << 4))),
			((i + 3) & ((1 << 2) - 1)) + (i & ((1 << 12) - (1 << 2)))
		};
		i++;
		return result;
		});

	applyTransformation(*transformationEdgeOrient, [this](int i, int j) {return getTransformationEdgeOrient(i, j); });
	applyTransformation(*transformationCornerOrient, [this](int i, int j) {return getTransformationCornerOrient(i, j); });
	applyTransformation(*transformationEdgePerm, [this](int i, int j) {return getTransformationEdgePerm(i, j); });
	applyTransformation(*transformationCornerPerm, [this](int i, int j) {return getTransformationCornerPerm(i, j); });
	applyTransformation(*transformationSlice, [this](int i, int j) {return getTransformationSlice(i, j); });
	applyTransformation(*transformationChoice, [this](int i, int j) {return getTransformationChoice(i, j); });
	applyTransformation(*transformationSlicePerm, [this](int i, int j) {return getTransformationSlicePerm(i, j); });
}

int KociembaSolver::getTransformationEdgeOrient(int index, int side) {
	std::array<int, 12> edges;
	generateNthCombination(edges, 12, 2, index);

	switch (side) {
	case 0:
		cycle(edges, 2, 7, 10, 6);
		edges[2] ^= 1;
		edges[7] ^= 1;
		edges[10] ^= 1;
		edges[6] ^= 1;
		break;
	case 1:
		cycle(edges, 3, 2, 1, 0);
		break;
	case 2:
		cycle(edges, 1, 6, 9, 5);
		break;
	case 3:
		cycle(edges, 0, 5, 8, 4);
		edges[0] ^= 1;
		edges[5] ^= 1;
		edges[8] ^= 1;
		edges[4] ^= 1;
		break;
	case 4:
		cycle(edges, 8, 9, 10, 11);
		break;
	case 5:
		cycle(edges, 3, 4, 11, 7);
		break;
	}
	return getNFromCombination(edges, 12, 2);
}

int KociembaSolver::getTransformationCornerOrient(int index, int side){
	std::array<int, 8> corners;
	generateNthCombination(corners, 8, 3, index);

	switch (side) {
	case 0:
		cycle(corners, 2, 3, 7, 6);
		corners[2] += 2;
		corners[3] += 1;
		corners[7] += 2;
		corners[6] += 1;
		break;
	case 1:
		cycle(corners, 3, 2, 1, 0);
		break;
	case 2:
		cycle(corners, 1, 2, 6, 5);
		corners[1] += 2;
		corners[2] += 1;
		corners[6] += 2;
		corners[5] += 1;
		break;
	case 3:
		cycle(corners, 0, 1, 5, 4);
		corners[0] += 2;
		corners[1] += 1;
		corners[5] += 2;
		corners[4] += 1;
		break;
	case 4:
		cycle(corners, 4, 5, 6, 7);
		break;
	case 5:
		cycle(corners, 3, 0, 4, 7);
		corners[3] += 2;
		corners[0] += 1;
		corners[4] += 2;
		corners[7] += 1;
		break;
	}
	return getNFromCombination(corners, 8, 3);
}

int KociembaSolver::getTransformationCornerPerm(int index, int side) {
	std::array<int, 8> corners;
	generateNthPermutation(corners, 8, index);

	switch (side) {
	case 0:
		cycle(corners, 2, 3, 7, 6);
		break;
	case 1:
		cycle(corners, 3, 2, 1, 0);
		break;
	case 2:
		cycle(corners, 1, 2, 6, 5);
		break;
	case 3:
		cycle(corners, 0, 1, 5, 4);
		break;
	case 4:
		cycle(corners, 4, 5, 6, 7);
		break;
	case 5:
		cycle(corners, 3, 0, 4, 7);
		break;
	}
	return getNFromPermutation(corners, 8);
}

int KociembaSolver::getTransformationEdgePerm(int index, int side) {
	std::array<int, 8> edges;
	generateNthPermutation(edges, 8, index);

	switch (side) {
	case 0:
		swap(edges, 2, 6);
		break;
	case 1:
		cycle(edges, 3, 2, 1, 0);
		break;
	case 2:
		swap(edges, 1, 5);
		break;
	case 3:
		swap(edges, 0, 4);
		break;
	case 4:
		cycle(edges, 4, 5, 6, 7);
		break;
	case 5:
		swap(edges, 3, 7);
		break;
	}
	return getNFromPermutation(edges, 8);
}

int KociembaSolver::getTransformationSlice(int index, int side) {
	std::array<int, 12> edges{};
	for (int i = 0; i < 4; i++) {
		int value = index % (12 - i);
		index = (index - value) / (12 - i);
		int j;
		for (j = 0; j < 12 && (edges[j] != 0 || value > 0); j++)
			if (edges[j] == 0)
				value--;

		edges[j] = i + 1;
	}
	switch (side)
	{
	case 0:
		cycle(edges, 2, 7, 10, 6);
		break;
	case 1:
		cycle(edges, 3, 2, 1, 0);
		break;
	case 2:
		cycle(edges, 1, 6, 9, 5);
		break;
	case 3:
		cycle(edges, 0, 5, 8, 4);
		break;
	case 4:
		cycle(edges, 8, 9, 10, 11);
		break;
	case 5:
		cycle(edges, 3, 4, 11, 7);
		break;
	}
	index = 0;
	for (int i = 3; i >= 0; i--)
	{
		int k = 0;
		for (int j = 0; j < 12 && edges[j] != i + 1; j++)
			if (edges[j] == 0 || edges[j] > i + 1)
				k++;
		index = index * (12 - i) + k;
	}
	return index;
}

int KociembaSolver::getTransformationSlicePerm(int index, int side) {
	std::array<int, 4> edges;
	generateNthPermutation(edges, 4, index);

	switch (side) {
	case 0:
		swap(edges, 2, 3);
		break;
	case 2:
		swap(edges, 1, 2);
		break;
	case 3:
		swap(edges, 0, 1);
		break;
	case 5:
		swap(edges, 0, 3);
		break;
	}
	return getNFromPermutation(edges, 4);
}

int KociembaSolver::getTransformationChoice(int index, int side) {
	std::array<int, 12> edges;
	generateNthCombination(edges, 12, 2, index);
	switch (side)
	{
	case 0:
		cycle(edges, 2, 7, 10, 6);
		break;
	case 1:
		cycle(edges, 3, 2, 1, 0);
		break;
	case 2:
		cycle(edges, 1, 6, 9, 5);
		break;
	case 3:
		cycle(edges, 0, 5, 8, 4);
		break;
	case 4:
		cycle(edges, 8, 9, 10, 11);
		break;
	case 5:
		cycle(edges, 3, 4, 11, 7);
		break;
	}
	return getNFromCombination(edges, 12, 2);
}

void KociembaSolver::swap(std::span<int> arr, int i, int j) {
	if (i < 0 || j < 0 || i >= arr.size() || j >= arr.size())
		throw std::out_of_range("Index out of range");

	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

void KociembaSolver::cycle(std::span<int> arr, int i, int j, int k, int l) {
	if (i < 0 || j < 0 || k < 0 || l < 0 || i >= arr.size() || j >= arr.size() || k >= arr.size() || l >= arr.size())
		throw std::out_of_range("Index out of range");

	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = arr[k];
	arr[k] = arr[l];
	arr[l] = temp;
}

void KociembaSolver::generateNthPermutation(std::span<int> arr, int len, int n) {
	if (len < 0 || arr.size() < len)
		throw std::out_of_range("Index out of range");

	int numbers[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	for (int i = 0; i < len; i++)
	{
		int value = n % (len - i);
		n = (n - value) / (len - i);
		arr[i] = numbers[value];
		while (++value < len)
			numbers[value - 1] = numbers[value];
	}
}

void KociembaSolver::generateNthPartialPermutation(std::span<int> arr, int len, int permCount, int permOffset, int n) {
	if (len < 0 || arr.size() < len)
		throw std::out_of_range("Index out of range");

	for (int i = 0; i < permCount; i++) {
		int k = n % (len - i);
		n = (n - k) / (len - i);
		for (int j = 0; j < len && ((arr[j] >= permOffset && arr[j] < permOffset + permCount) || k > 0); j++)
			if (arr[j] < permOffset || arr[j] >= permOffset + permCount)
				k--;

		arr[i] = k + permOffset;
	}
}

void KociembaSolver::generateNthCombination(std::span<int> arr, int len, int valueLimit, int n) {
	if (len < 0 || arr.size() < len)
		throw std::out_of_range("Index out of range");

	int j = 0;
	for (int i = 0; i < len - 1; i++)
	{
		int k = n % valueLimit;
		j += valueLimit - k;
		n = (n - k) / valueLimit;
		arr[i] = k;
	}
	arr[len - 1] = j % valueLimit;
}

int KociembaSolver::getNFromCombination(std::span<const int> arr, int len, int valueLimit) {
	int n = 0;
	for (int i = len - 2; i >= 0; i--)
	{
		n = n * valueLimit + (arr[i] % valueLimit);
	}
	return n;
}

int KociembaSolver::getNFromPermutation(std::span<const int> arr, int len) {
	int n = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		int k = 0;
		for (int j = i + 1; j < len; j++)
			if (arr[j] < arr[i])
				k++;

		n = n * (len - i) + k;
	}
	return n;
}

int KociembaSolver::getNFromPartialPermutation(std::span<const int> arr, int start, int len, int permCount, int permOffset) {
	int n = 0;
	for (int i = permCount - 1; i >= 0; i--)
	{
		int k = 0;
		for (int j = 0; j < len && arr[start + j] != permOffset + i; j++)
			if (arr[start + j] < permOffset || arr[start + j] > permOffset + i )
				k++;

		n = n * (len - i) + k;
	}
	return n;
}