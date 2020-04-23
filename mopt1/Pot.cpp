#include "Pot.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

Transport::Transport(const char *fileName)
{
	int i, j;
	ifstream input;
	input.open(fileName);

	input >> numSource;
	input >> numReceiver;
	sourceValues   = new double[numSource];
	receiverValues = new double[numReceiver];

	for (i = 0; i < numReceiver; i++)
		input >> receiverValues[i];
	
	cost  = new double*[numSource];
	goods = new double*[numSource];
	for (i = 0; i < numSource; i++)
	{
		input >> sourceValues[i];

		cost [i] = new double[numReceiver];
		goods[i] = new double[numReceiver];

		for (j = 0; j < numReceiver; j++)
		{
			input >> cost[i][j];
			goods[i][j] = -1.0;
		}
	}

	cycle = new struct cell[numSource + numReceiver];
	for (i = 0; i < numReceiver + numSource; i++)
	{
		cycle[i].i = -1;
		cycle[i].j = -1;
	}
}

Transport::~Transport()
{
	int i;
	delete[] sourceValues;
	delete[] receiverValues;

	for (i = 0; i < numSource; i++)
	{
		delete[] cost[i];
		delete[] goods[i];
	}
	delete[] cost;
	delete[] goods;
	delete[] cycle;
}

void Transport::Print(const char *fileName)
{
	int i, j;
	ofstream output;
	output.open(fileName);

	double result = 0.0;
	for (i = 0; i < numSource; i++)
		for (j = 0; j < numReceiver; j++)
			if (goods[i][j] > -1.0)
				result += goods[i][j] * cost[i][j];
	output << "Answer: " << result << endl;
	for (i = 0; i < numSource; i++)
	{
		for (j = 0; j < numReceiver; j++)
			if (goods[i][j] < 0.0)
				output <<" "<< 0.0;
			else
				output << " " << goods[i][j];
		output << endl;
	}
}

void Transport::PrintMat(const char *fileName)
{
	int i, j;
	ofstream output;
	output.open(fileName);

	for (i = 0; i < numReceiver; i++)
	{
		output << receiverValues[i] << " ";
	}
	output << endl;

	for (i = 0; i < numSource; i++)
	{
		output << sourceValues[i] << "  ";

		for (j = 0; j < numReceiver; j++)
		{
			output << cost[i][j] << " ";
		}
		output << endl;
	}
}

void Transport::nwCorner()
{
	int i = 0, j = 0;
	double minValue = 0;
	while ((i < numSource) && (j < numReceiver))
	{
		minValue = min(sourceValues[i], receiverValues[j]);
		if (minValue == receiverValues[j])
		{
			goods[i][j] = receiverValues[j];
			sourceValues[i] -= receiverValues[j];
			j++;
		}
		else {
			goods[i][j] = sourceValues[i];
			receiverValues[j] -= sourceValues[i];
			i++;
		}
	}
}

bool Transport::PotentialMet()
{
	int i, j, iFreeCell = 0, ind, flag = 1;
	struct cell * cells = new struct cell[numSource + numReceiver - 1];
	double ** u = new double*[numSource], ** v = new double*[numReceiver];
	bool isOpt = true;

	//...[0] - values of potential
	//...[1] - 1/0: if the value valid
	for(i = 0; i < numSource; i++)
		u[i] = new double[2];
	for (i = 0; i < numReceiver; i++)
		v[i] = new double[2];
	cout << "memory checked"<<endl;

	//initialization
	for (i = 0; i < numSource; i++)
		u[i][1] = 0;
	for (i = 0; i < numReceiver; i++)
		v[i][1] = 0;
	cout << "init checked" << endl;

	//find ordered cells
	for (i = 0; i < numSource; i++)
	{
		for (j = 0; j < numReceiver; j++)
		{
			if (goods[i][j] != -1)
			{
				cells[iFreeCell].i = i;
				cells[iFreeCell].j = j;
				iFreeCell++;
			}
		}
	}
	cout << "find checked" << endl;
	u[0][0] = 0.0;
	u[0][1] = 1.0;

	//count potentials
	while (flag)
	{
		flag = 0;
		for (ind = 0; ind < numSource + numReceiver - 1; ind++)
		{
			if (u[cells[ind].i][1] == 1.0 && v[cells[ind].j][1] == 1.0)
			{
				continue;
			}
			else
			{
				if (u[cells[ind].i][1] == 1.0 && v[cells[ind].j][1] != 1.0)
				{
					v[cells[ind].j][1] = 1.0;
					v[cells[ind].j][0] = cost[cells[ind].i][cells[ind].j] + u[cells[ind].i][0];
					flag = 1;
				
				}
				else
				{
					if (u[cells[ind].i][1] != 1.0 && v[cells[ind].j][1] == 1.0)
					{
						u[cells[ind].i][1] = 1.0;
						u[cells[ind].i][0] = v[cells[ind].j][0] - cost[cells[ind].i][cells[ind].j];
						flag = 1;
					}
					else
						continue;
				}
			}
		}
	}

	cout << "V:"<<endl;
	for (i = 0; i < numReceiver; i++)
		cout << v[i][0] << endl;

	cout << "U:" << endl;
	for (i = 0; i < numSource; i++)
		cout << u[i][0] << endl;

	//check optimal conditions
	for (i = 0; i < numSource; i++)
	{
		for (j = 0; j < numReceiver; j++)
		{
			if (goods[i][j] == -1)
			{
				if (v[j][0] - u[i][0] > cost[i][j])
				{
					isOpt = false;
					iBegCycle = i;
					jBegCycle = j;
					break;
				}
			}
		}
		if (isOpt == false)
			break;
	}
	cout << "checked optimal conditions"<<endl;


	for (i = 0; i < numSource; i++)
		delete[] u[i];
	for (i = 0; i < numReceiver; i++)
		delete[] v[i];

	delete[] v;
	delete[] u;
	delete[] cells;

	return isOpt;
}

void Transport::recount()
{
	bool isOpt = false, isFound = false;
	int i;

	while (!isOpt)
	{
		isOpt = PotentialMet();

		if (!isOpt)
		{
			printGoods();

			isFound = findCycle(iBegCycle, jBegCycle);
			if (!isFound)
				cout << "Cycle not found." << endl;

			modify();

			isFound = false;
		}
	}

	printGoods();
	for(i = 0; !(cycle[i].i == cycle[0].i && cycle[i].j == cycle[0].j && i != 0); i++)
		cout << cycle[i].i << "," << cycle[i].j << "|";
}

bool Transport::findCycle(int iFirstCell, int jFirstCell)
{
	bool isFound = 0;
	int i, j, ind = 0;

	cycle[ind].i = iFirstCell;
	cycle[ind].j = jFirstCell;

	for (j = cycle[ind].j - 1; j >= 0; j--)// turn to the left
	{
		if (goods[cycle[ind].i][j] != -1)
		{
			cycle[ind + 1].i = cycle[ind].i;
			cycle[ind + 1].j = j;

			isFound = find(ind + 1);
			if (isFound == true)
				return true;
			else
			{
				cycle[ind + 1].i = -1;
				cycle[ind + 1].j = -1;
			}
		}
	}
	for (j = cycle[ind].j + 1; j <= numReceiver; j++)// turn to the right
	{
		if (goods[cycle[ind].i][j] != -1)
		{
			cycle[ind + 1].i = cycle[ind].i;
			cycle[ind + 1].j = j;

			isFound = find(ind + 1);
			if (isFound == true)
				return true;
			else
			{
				cycle[ind + 1].i = -1;
				cycle[ind + 1].j = -1;
			}
		}
	}


	for (i = cycle[ind].i - 1; i >= 0; i--)// turn up
	{
		if (goods[i][cycle[ind].j] != -1)
		{
			cycle[ind + 1].i = i;
			cycle[ind + 1].j = cycle[ind].j;

			isFound = find(ind + 1);
			if (isFound == true)
				return true;
			else
			{
				cycle[ind + 1].i = -1;
				cycle[ind + 1].j = -1;
			}
		}
	}
	for (i = cycle[ind].i + 1; i <= numSource; i++)// turn down
	{
		if (goods[i][cycle[ind].j] != -1)
		{
			cycle[ind + 1].i = i;
			cycle[ind + 1].j = cycle[ind].j;

			isFound = find(ind + 1);
			if (isFound == true)
				return true;
			else
			{
				cycle[ind + 1].i = -1;
				cycle[ind + 1].j = -1;
			}
		}
	}

	return false;
}

// ind - index of cycle[] array
bool Transport::find(int ind)
{
	bool isFound = 0;
	int i, j;

	// check for begin element
	if (cycle[ind].j == cycle[0].j && cycle[ind].i == cycle[0].i)
		return true;

	// check for element already added to cycle
	for (i = 1; i < ind; i++)
	{
		if (cycle[ind].j == cycle[i].j && cycle[ind].i == cycle[i].i)
			return false;
	}

	if (cycle[ind].j == cycle[ind - 1].j) // last cell on vertical line
	{
		
		for (j = cycle[ind].j - 1; j >= 0; j--)// turn to the left
		{
			if (goods[cycle[ind].i][j] != -1 || (cycle[ind].i == iBegCycle && j == jBegCycle))//if cell is empty or begin
			{
				cycle[ind + 1].i = cycle[ind].i;
				cycle[ind + 1].j = j;

				isFound = find(ind + 1);
				if (isFound == true)
					return true;
				else
				{
					cycle[ind + 1].i = -1;
					cycle[ind + 1].j = -1;
				}
			}
		}
		for (j = cycle[ind].j + 1; j < numReceiver; j++)// turn to the right
		{
			if (goods[cycle[ind].i][j] != -1 || (cycle[ind].i == iBegCycle && j == jBegCycle))//if cell is empty or begin
			{
				cycle[ind + 1].i = cycle[ind].i;
				cycle[ind + 1].j = j;

				isFound = find(ind + 1);
				if (isFound == true)
					return true;
				else
				{
					cycle[ind + 1].i = -1;
					cycle[ind + 1].j = -1;
				}
			}
		}
	}

	if (cycle[ind].i == cycle[ind - 1].i) // last cell on horizontal line
	{

		for (i = cycle[ind].i - 1; i >= 0; i--)// turn up
		{
			if (goods[i][cycle[ind].j] != -1 || (i == iBegCycle && cycle[ind].j == jBegCycle))//if cell is empty or begin
			{
				cycle[ind + 1].i = i;
				cycle[ind + 1].j = cycle[ind].j;

				isFound = find(ind + 1);
				if (isFound == true)
					return true;
				else
				{
					cycle[ind + 1].i = -1;
					cycle[ind + 1].j = -1;
				}
			}
		}
		for (i = cycle[ind].i + 1; i < numSource; i++)// turn down
		{
			if (goods[i][cycle[ind].j] != -1 || (i == iBegCycle && cycle[ind].j == jBegCycle))//if cell is empty or begin
			{
				cycle[ind + 1].i = i;
				cycle[ind + 1].j = cycle[ind].j;

				isFound = find(ind + 1);
				if (isFound == true)
					return true;
				else
				{
					cycle[ind + 1].i = -1;
					cycle[ind + 1].j = -1;
				}
			}
		}
	}

	return false;
}

void Transport::modify()
{
	int ind = 1, imin = cycle[1].i, jmin = cycle[1].j;
	double min = goods[imin][jmin];

	//find minimun in cycle
	while (!(cycle[ind].i == cycle[0].i && cycle[ind].j == cycle[0].j))
	{
		if (goods[cycle[ind].i][cycle[ind].j] < min && ind % 2 == 1)
		{
			imin = cycle[ind].i;
			jmin = cycle[ind].j;
			min = goods[cycle[ind].i][cycle[ind].j];
		}
		cout << cycle[ind].i << "," << cycle[ind].j << "|";
		ind++;
	}

	//recount values in goods
	goods[cycle[0].i][cycle[0].j] = 0;

	ind = 0;
	while (!(cycle[ind].i == cycle[0].i && cycle[ind].j == cycle[0].j && ind != 0))
	{
		if (ind % 2 == 0)// plus
			goods[cycle[ind].i][cycle[ind].j] += min;
		else// minu
			goods[cycle[ind].i][cycle[ind].j] -= min;

		ind++;
	}

	// mark cell with minimum value as empty
	goods[imin][jmin] = -1;

}

void Transport::printGoods()
{
	int i, j;

	for (i = 0; i < numSource; i++)
	{
		for (j = 0; j < numReceiver; j++)
		{
			if (goods[i][j] == -1)
				cout << "__ ";
			else
				cout << goods[i][j] << " ";
		}
		cout << endl;
	}
}
