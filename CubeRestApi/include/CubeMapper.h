#pragma once
#include <CubeState.h>
#include <string>
#include <vector>

class CubeMapper {
private:
	enum CubeSides {
		Front,
		Back,
		Right,
		Left,
		Up,
		Down
	};
	static const inline std::vector<std::vector<int>> coords = {
		{ 38, 9, 20 },
		{ 44, 18, 2 },
		{ 42, 0, 29 },
		{ 36, 27, 11 },
		{ 53, 26, 15 },
		{ 47, 8, 24 },
		{ 45, 35, 6 },
		{ 51, 17, 33 },
		{ 41, 19 },
		{ 43, 1 },
		{ 39, 28 },
		{ 37, 10 },
		{ 23, 12 },
		{ 21, 5 },
		{ 32, 3 },
		{ 30, 14 },
		{ 50, 25 },
		{ 46, 7 },
		{ 48, 34 },
		{ 52, 16 }
	};
	static const inline std::vector<std::vector<int>> pieces = {
		{ CubeSides::Up, CubeSides::Back, CubeSides::Right },
		{ CubeSides::Up, CubeSides::Right, CubeSides::Front },
		{ CubeSides::Up, CubeSides::Front, CubeSides::Left },
		{ CubeSides::Up, CubeSides::Left, CubeSides::Back },
		{ CubeSides::Down, CubeSides::Right, CubeSides::Back },
		{ CubeSides::Down, CubeSides::Front, CubeSides::Right },
		{ CubeSides::Down, CubeSides::Left, CubeSides::Front },
		{ CubeSides::Down, CubeSides::Back, CubeSides::Left },
		{ CubeSides::Up, CubeSides::Right },
		{ CubeSides::Up, CubeSides::Front },
		{ CubeSides::Up, CubeSides::Left },
		{ CubeSides::Up, CubeSides::Back },
		{ CubeSides::Right, CubeSides::Back },
		{ CubeSides::Right, CubeSides::Front },
		{ CubeSides::Left, CubeSides::Front },
		{ CubeSides::Left, CubeSides::Back },
		{ CubeSides::Down, CubeSides::Right },
		{ CubeSides::Down, CubeSides::Front },
		{ CubeSides::Down, CubeSides::Left },
		{ CubeSides::Down, CubeSides::Back }
	};
public:
	static CubeState convertToCubeState(std::string cubeStateStr);
};