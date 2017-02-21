#ifndef TriBoxTest_H
#define TriBoxTest_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ms_vectormatrix.h>


namespace ModelStudio{
	enum{
		XX,YY,ZZ
	};


#define FINDMINMAX(x0,x1,x2,min,max) \
	 min = max = x0;   \
	if(x1<min) min=x1;\
	if(x1>max) max=x1;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v[0].y - b*v[0].z;			       	   \
	p2 = a*v[2].y - b*v[2].z;			       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * voxel.size.y + fb * voxel.size.z;   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v[0].y - b*v[0].z;			           \
	p1 = a*v[1].y - b*v[1].z;			       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * voxel.size.y + fb * voxel.size.z;   \
	if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v[0].x + b*v[0].z;		      	   \
	p2 = -a*v[2].x + b*v[2].z;	       	       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * voxel.size.x + fb * voxel.size.z;   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v[0].x + b*v[0].z;		      	   \
	p1 = -a*v[1].x + b*v[1].z;	     	       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * voxel.size.x + fb * voxel.size.z;   \
	if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/
#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v[1].x - b*v[1].y;			           \
	p2 = a*v[2].x - b*v[2].y;			       	   \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * voxel.size.x + fb * voxel.size.y;   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v[0].x - b*v[0].y;				   \
	p1 = a*v[1].x - b*v[1].y;			           \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * voxel.size.x + fb * voxel.size.y;   \
	if(min>rad || max<-rad) return 0;




class AABBtest
{
private:
	int i,j,k;
	Vector3d v[3];
	double min,max,d,p0,p1,p2,rad,fex,fey,fez;  
	Vector3d e[3];
public:

	struct TRIANGLE{
		Vector3d vertex[3];
		Vector3d normal;
	}triangle;

	struct VOXEL{
		Vector3d center;
		Vector3d size;
	}voxel;

	void setData(double _boxSize, double *_boxCenter, double *_triangleVertex);
	int planeBoxOverlap(double d);
	int triBoxOverlap();
	bool isBoxIncludeTri(Vector3d _begin, Vector3d _end, Vector3d *_vertex);
	//–ß‚è’l‚Í“à•”‚É‚ ‚é“_‚ÌŒÂ”DÅŒã‚Ìˆø”‚Ìæ“ª‚©‚ç“à•”‚Ì“_‚ªŠi”[‚³‚ê‚é
	int calFaceDivideTri(Vector3d _normal, Vector3d _center, Vector3d *_vertex);
};


#endif

};