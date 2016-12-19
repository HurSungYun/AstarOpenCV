#include "ASTAR.H"
int UNIT::GetX(){
	return X;
}
int UNIT::GetY(){
	return Y;
}
double UNIT::GetTheta(){
	return (double)atanl(VY/VX);
}
double UNIT::GetVX(){
	return VX;
}
double UNIT::GetVY(){
	return VY;
}
void UNIT::PutX(int x){
	X=x;
}
void UNIT::PutY(int y){
	Y=y;
}
void UNIT::PutVx(int vx){
	VX=vx;
}
void UNIT::PutVy(int vy){
	VY=vy;
}
void UNIT::PutXY(int x, int y){
	X=x;
	Y=y;
}
void UNIT::PutV(int vx, int vy){
	VX=vx;
	VY=vy;
}
void UNIT::Rotating(double theta, double v=-1){
	double Theta;
	double Velocity;

	Theta = (double)atanl(VY/VX);
	Theta += theta;

	if(v<0)	Velocity = sqrt((double)VX*VX+VY*VY);
	else	Velocity = v;

	VX = (double)Velocity * cosl(Theta);
	VY = (double)Velocity * sinl(Theta);
}

void UNIT::Move(){
	X+=VX;
	Y+=VY;
}