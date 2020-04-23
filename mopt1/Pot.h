#pragma once

struct cell {
	int i, j;
};

class Transport
{
private:
	int numSource, numReceiver;
	int iBegCycle, jBegCycle;

	double *sourceValues, *receiverValues;

	cell * cycle;

	double **cost;
	double **goods;

public:
	Transport(const char *a_FileName);
	~Transport();
	void Print(const char *a_FileName);

	void nwCorner();
	bool PotentialMet();
	void recount();
	bool findCycle(int i, int j);
	bool find(int i);
	void modify();

	void PrintMat(const char *a_FileName);
	void printGoods();
};