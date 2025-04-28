
#include <iostream>
#include <vector>
#include <map>
#include "acequia_manager.h"

int main(){
	//Initialize the Acequia Manger with Random Parameters
	AcequiaManager manager;
	manager.initializeRandomParameters();

	//Display initial state
//	manager.displayState();

 	//Students will implement this function to solve the problems
 	solveProblems(manager);
 	manager.displayState();
 	//Evaluate the solution and display results
 	manager.evaluateSolution();
 	manager.displayLeaderboard();
	

	return 0;

}