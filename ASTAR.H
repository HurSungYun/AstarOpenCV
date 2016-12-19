#ifndef ASTAR_H
#define ASTAR_H

#include<windows.h>
#include<math.h>
#include<stdio.h>
#include<queue>
using namespace std;

class UNIT{
private:
	int VX, VY;
	int X, Y;
public:
	int GetX();
	int GetY();
	double GetTheta();
	double GetVX();
	double GetVY();
	void PutX(int x);
	void PutY(int y);
	void PutVx(int vx);
	void PutVy(int vy);
	void PutXY(int x, int y);
	void PutV(int vx, int vy);
	void Rotating(double theta, double v);
	void Move();

	UNIT(int x=0, int y=0, int vx=0, int vy=0){
		X=x;
		Y=y;
		VX=vx;
		VY=vy;
	}
	~UNIT(){}
};

#endif