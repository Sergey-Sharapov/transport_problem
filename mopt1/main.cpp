#include "Pot.h"

int main()
{
	Transport Problem("input.txt");

	Problem.nwCorner();
	Problem.Print("Output_nwcorner.txt");
	Problem.recount();
	Problem.Print("Output.txt");

	return 0;
}