#include "GraphSolver.h"

int main() {
	GraphSolver graphSolver;
	
	while(graphSolver.IsRunning()) {
		graphSolver.Update();
		graphSolver.Render();
	}
}