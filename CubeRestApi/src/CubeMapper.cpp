#include <CubeMapper.h>
#include <VectorUtils.h>
#include <set>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <array>

CubeState CubeMapper::convertToCubeState(std::string cubeStr) {
	std::map<char, int> colorCounter;
	std::map<char, int> colorMap;
	for (int i = 0; i < cubeStr.length(); i++) {
		char c = cubeStr[i];
		colorCounter[c]++;

		if (i % 9 == 4) {
			if (colorMap.contains(c))
				throw std::invalid_argument("Cube has duplicate centers");
			else
				colorMap.insert({ c, i / 9 });
		}
	}

	if (colorCounter.size() != 6)
		throw std::invalid_argument("Cube has too many colors");

	std::ostringstream errorStream;
	for (const auto& [color, count] : colorCounter) {
		if (count != 9) {
			if (errorStream.str().empty())
				errorStream << "Cube has incorrect number of colors: ";
			errorStream << count << " '" << color << "', ";
		}
	}
	std::string error = errorStream.str();
	if (!error.empty()) {
		error.erase(error.length() - 2);
		throw std::invalid_argument(errorStream.str());
	}

	std::array<int, 26> cubeletPermutations;
	std::array<int, 26> cubeletOrientations;
	std::array<int, 6> faceOrientations;

	for (int i = 0; i < coords.size(); i++) {
		std::vector<int> currentCubelet;
		int firstFace = colorMap[cubeStr[coords[i][0]]];
		for (auto coord : coords[i]) {
			currentCubelet.push_back(colorMap[cubeStr[coord]]);
		}
	std:sort(currentCubelet.begin(), currentCubelet.end());
		bool found = false;
		for (int j = 0; j < pieces.size(); j++) {
			std::vector<int> piece = pieces[j];
			std::sort(piece.begin(), piece.end());

			if (currentCubelet == piece) {
				cubeletPermutations[i] = j;
				cubeletOrientations[i] = VectorUtils::indexOf(pieces[j], firstFace);
				found = true;
				break;
			}
		}
		if (!found)
			throw std::invalid_argument("Invalid cube state: cubelet permutation not found");
	}


	for (int i = 0; i < 6; i++) {
		cubeletPermutations[20 + i] = i + 20;
		cubeletOrientations[20 + i] = 0;
		faceOrientations[i] = 0;
	}

	return CubeState(cubeletPermutations, cubeletOrientations, faceOrientations);
}
