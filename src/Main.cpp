#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif
#include "GraphSolver.h"

int main() {
	GraphSolver graphSolver;
	
	while(graphSolver.IsRunning()) {
		graphSolver.Update();
		graphSolver.Render();
	}
}