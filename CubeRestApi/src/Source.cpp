#include <crow.h>
#include <string>
#include <CubeState.h>
#include <CubeMapper.h>
#include <KociembaSolver.h>

std::string solve(CubeState cube) {
	KociembaSolver solver;
	solver.setCubeState(cube);
	return "InProgress";
}

int main() {
	crow::SimpleApp app;

	CROW_ROUTE(app, "/Solve/<string>")([](std::string cubeStr) {
		if (cubeStr.length() != 54) {
			crow::json::wvalue error;
			error["error"] = "Invalid cube state";
			return crow::response(400, error);
		}

		CubeState cube = CubeMapper::convertToCubeState(cubeStr);
		std::string solution = solve(cube);
		if (solution.empty()) {
			crow::json::wvalue error;
			error["error"] = "No solution found";
			return crow::response(404, error);
		}
		else {
			return crow::response(solution);
		}
		});

	app.port(18080).multithreaded().run();
	return 0;
}