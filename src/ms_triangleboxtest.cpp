#include"ms_triangleboxtest.h"

namespace ModelStudio
{

	void AABBtest::setData(double _boxSize, double *_boxCenter, double *_triangleVertex){
		voxel.size=Vector3d(_boxSize,_boxSize,_boxSize);
		voxel.center=Vector3d(_boxCenter);
		for(i=0;i<3;i++)triangle.vertex[i]=Vector3d(&_triangleVertex[i*3]);
	}


	int AABBtest::planeBoxOverlap(double d)
	{
		Vector3d vmin,vmax;
		for(i=0;i<3;i++)
		{
			if(triangle.normal.X[i]>0.0f)
			{
				vmin.X[i]=-voxel.size.X[i];
				vmax.X[i]=voxel.size.X[i];
			}
			else
			{
				vmin.X[i]=voxel.size.X[i];
				vmax.X[i]=-voxel.size.X[i];
			}
		}
		if(triangle.normal*vmin+d>0.0f) return 0;
		if(triangle.normal*vmax+d>=0.0f) return 1;
	  return 0;
	}

	int AABBtest::triBoxOverlap()
	{

		/*    use separating axis theorem to test overlap between triangle and box */
		/*    need to test for overlap in these directions: */
		/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
		/*       we do not even need to test these) */
		/*    2) normal of the triangle */
		/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
		/*       this gives 3x3=9 more tests */


		/* This is the fastest branch on Sun */
		/* move everything so that the boxcenter is in (0,0,0) */
		for(i=0;i<3;i++) 
		   v[i]=triangle.vertex[i]-voxel.center;

		/* compute triangle edges */
		for(i=0;i<3;i++) 
			e[i]=v[(i+1)%3]-v[i];

		/* Bullet 3:  */
		/*  test the 9 tests first (this was faster) */
		fex = fabs(e[0].x);
		fey = fabs(e[0].y);
		fez = fabs(e[0].z);
		AXISTEST_X01(e[0].z, e[0].y, fez, fey);
		AXISTEST_Y02(e[0].z, e[0].x, fez, fex);
		AXISTEST_Z12(e[0].y, e[0].x, fey, fex);

		fex = fabs(e[1].x);
		fey = fabs(e[1].y);
		fez = fabs(e[1].z);
		AXISTEST_X01(e[1].z, e[1].y, fez, fey);
		AXISTEST_Y02(e[1].z, e[1].x, fez, fex);
		AXISTEST_Z0(e[1].y, e[1].x, fey, fex);

		fex = fabs(e[2].x);
		fey = fabs(e[2].y);
		fez = fabs(e[2].z);
		AXISTEST_X2(e[2].z, e[2].y, fez, fey);
		AXISTEST_Y1(e[2].z, e[2].x, fez, fex);
		AXISTEST_Z12(e[2].y, e[2].x, fey, fex);

		/* Bullet 1: */
		/*  first test overlap in the {x,y,z}-directions */
		/*  find min, max of the triangle each direction, and test for overlap in */
		/*  that direction -- this is equivalent to testing a minimal AABB around */
		/*  the triangle against the AABB */

		/* test in X-direction */
		for(i=0;i<3;i++)
		{
			FINDMINMAX(v[0].X[i],v[1].X[i],v[2].X[i],min,max);
			if(min>voxel.size.X[i] || max<-voxel.size.X[i]) return 0;
		}

		/* Bullet 2: */
		/*  test if the box intersects the plane of the triangle */
		/*  compute plane equation of triangle: normal*x+d=0 */
		triangle.normal=e[0]%e[1];
		d=-(triangle.normal*v[0]);  /* plane eq: normal.x+d=0 */
		if(!planeBoxOverlap(d)) return 0;

	   return 1;   /* box and triangle overlaps */
	}

	bool AABBtest::isBoxIncludeTri(Vector3d _begin, Vector3d _end, Vector3d *_vertex)
	{
		if(_begin.x<_vertex[0].x&&_begin.x<_vertex[1].x&&_begin.x<_vertex[2].x)
			if(_begin.y<_vertex[0].y&&_begin.y<_vertex[1].y&&_begin.y<_vertex[2].y)
				if(_begin.z<_vertex[0].z&&_begin.z<_vertex[1].z&&_begin.z<_vertex[2].z)
					if(_end.x>_vertex[0].x&&_end.x>_vertex[1].x&&_end.x>_vertex[2].x)
						if(_end.y>_vertex[0].y&&_end.y>_vertex[1].y&&_end.y>_vertex[2].y)
							if(_end.z>_vertex[0].z&&_end.z>_vertex[1].z&&_end.z>_vertex[2].z)
								return true;
		return false;
	}

	int AABBtest::calFaceDivideTri(Vector3d _normal, Vector3d _center, Vector3d *_vertex)
	{
		int inNum=0;
		int outNum=0;
		Vector3d in[3];
		Vector3d out[3];
		for(int i=0;i<3;i++){
			if(_normal*(_vertex[i]-_center)<0){
				in[inNum]=_vertex[i];
				inNum++;
			}
			else{
				out[outNum]=_vertex[i];
				outNum++;
			}
		}
		for(int i=0;i<inNum;i++)
			_vertex[i]=in[i];
		for(int i=0;i<outNum;i++)
			_vertex[inNum+i]=out[i];
		return inNum;
	}
}