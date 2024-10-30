#pragma once
#include <CubeState.h>
#include <span>
#include <string>
#include <memory>

class KociembaSolver {
private:
	bool initialized;
	int phase1Len;
	int phase2Len;
	int maxDepth;
	int solutionLength;
	std::unique_ptr<std::array<int, 40>> solutionMoves;
	std::unique_ptr<std::array<int, 40>> solutionAmmount;
	std::unique_ptr<std::array<std::array<int, 10>, 40>> positions;

	std::unique_ptr<std::array<std::array<int, 6>, 2048>> transformationEdgeOrient;
	std::unique_ptr<std::array<std::array<int, 6>, 2187>> transformationCornerOrient;
	std::unique_ptr<std::array<std::array<int, 6>, 40320>> transformationEdgePerm;
	std::unique_ptr<std::array<std::array<int, 6>, 40320>> transformationCornerPerm;
	std::unique_ptr<std::array<std::array<int, 6>, 11880>> transformationSlice;
	std::unique_ptr<std::array<std::array<int, 6>, 2048>> transformationChoice;
	std::unique_ptr<std::array<std::array<int, 6>, 24>> transformationSlicePerm;
	std::unique_ptr<std::array<std::array<int, 6>, 4096>> transformationFace;

	std::unique_ptr<std::array<char, 2048>> pruneEdgeOrient;
	std::unique_ptr<std::array<char, 2187>> pruneCornerOrient;
	std::unique_ptr<std::array<char, 40320>> pruneEdgePerm;
	std::unique_ptr<std::array<char, 40320>> pruneCornerPerm;
	std::unique_ptr<std::array<char, 2048>> pruneChoice;
	std::unique_ptr<std::array<char, 24>> pruneSlicePerm;
	std::unique_ptr<std::array<char, 4096>> pruneFace1;
	std::unique_ptr<std::array<char, 4096>> pruneFace2;
public:
	KociembaSolver();
	~KociembaSolver();
	void reset();
	bool setCubeState(const CubeState& cubeState);

private:
	void resetConfiguration();
	int calculateEdgeOrientation(const CubeState& cubeState) const;
	int calculateCornerOrientation(const CubeState& cubeState) const;
	int calculateMiddleSlice(const CubeState& cubeState) const;

	void initializeParameters();
	void clearPruneArrays();
	void initializeTransformations();
	void initializePruneArrays();
	void initializePruneCornerPerm(std::span<char> pruneArr, const auto& transArr);
	void initializePruneArray(std::span<char> pruneArr, const auto& transArr);
	int getTransformationEdgeOrient(int index, int side);
	int getTransformationCornerOrient(int index, int side);
	int getTransformationCornerPerm(int index, int side);
	int getTransformationEdgePerm(int index, int side);
	int getTransformationSlice(int index, int side);
	int getTransformationSlicePerm(int index, int side);
	int getTransformationChoice(int index, int side);

	void swap(std::span<int> arr, int i, int j);
	void cycle(std::span<int> arr, int i, int j, int k, int l);
	void generateNthPermutation(std::span<int> arr, int len, int n);
	void generateNthPartialPermutation(std::span<int> arr, int len, int permCount, int permOffset, int n);
	void generateNthCombination(std::span<int> arr, int len, int valueLimit, int n);
	int getNFromCombination(std::span<const int> arr, int len, int valueLimit);
	int getNFromPermutation(std::span<const int> arr, int len);
	int getNFromPartialPermutation(std::span<const int> arr, int start, int len, int permCount, int permOffset);
};