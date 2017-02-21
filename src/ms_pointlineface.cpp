#include "ms_pointlineface.h"

namespace ModelStudio
{

	double getFacePointDistance(Vector3d _point, Vector3d _normal, Vector3d _coord)
	{
		double dst;
		dst=_normal*(_point-_coord)/_normal.abs();
		return dst;
	}
	double getPointLineDistance(Vector3d _point, Vector3d _dir, Vector3d _coord)
	{
		double dst;
		dst=_dir*(_point-_coord)/_dir.abs();
		return dst;
	}

	Vector3d getFacePointProjection(Vector3d _point,  Vector3d _lNormal, Vector3d _sNormal, Vector3d _coord)
	{
		Vector3d dst;
		double t;
		t=-_sNormal*(_point-_coord)/(_sNormal*_lNormal);
		dst=_point+_lNormal*t;
		return dst;
	}
	Vector3d getFacePointProjection(Vector3d _point,  Vector3d _normal, Vector3d _coord)
	{
		return getFacePointProjection(_point,_normal,_normal,_coord);
	}
	Vector3d getIntersectPointFace(Vector3d *_point, Vector3d _normal, Vector3d _coord)
	{
		Vector3d dst;
		double t;
		Vector3d dir;
		dir=(_point[1]-_point[0])/(_point[1]-_point[0]).abs();
		t=-_normal*(_point[0]-_coord)/(_normal*dir);
		dst=_point[0]+dir*t;
		return dst;
	}
	bool isPointWithinTriangle(Vector3d _point, Vector3d _normal, Vector3d _coord)
	{
		double dst;
		dst=getFacePointDistance(_point,_normal,_coord);
		if(dst<0)return true;
		return false;
	}
	bool isPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d *_coord)
	{
		Vector3d c1,c2,c3;
		Vector3d u, v, w;
		c1=_coord[0]-_point;
		c2=_coord[1]-_point;
		c3=_coord[2]-_point;
		u=c2%c3;
		v=c3%c1;
		w=c1%c2;
		if(u*v<0.0)return false;
		if(u*w<0.0)return false;
		return true;
	}
	bool isPointsSandFace(Vector3d *_point, Vector3d _normal, Vector3d _coord)
	{
		double dst1;
		double dst2;
		dst1=getFacePointDistance(_point[0],_normal,_coord);
		dst2=getFacePointDistance(_point[1],_normal,_coord);
		if(dst1*dst2>0)return false;
		return true;
	}
	bool isFacesSandPoint(Vector3d _coord1, Vector3d _coord2, Vector3d _point)
	{
		Vector3d tnormal = (_coord1-_coord2)/(_coord1-_coord2).abs();
		if(tnormal*(_coord1-_point)>=0 && tnormal*(_coord2-_point)<=0)
			return true;
		return false;
	}
	bool isIntersectLineTriangle(Vector3d *_point, Vector3d *_coord, Vector3d *_intersection)
	{
		double det;
		double t, u, v;
		double invDet;
		Vector3d e1, e2, dir, pvec, tvec, qvec;
		e1=_coord[1]-_coord[0];
		e2=_coord[2]-_coord[0];
		dir=(_point[1]-_point[0])/(_point[1]-_point[0]).abs();
		pvec=dir%e2;
		det=e1*pvec;
		if (det > (1e-3))
		{
			tvec=_point[0]-_coord[0];
			u=tvec*pvec;
			if (u < 0.0f || u > det) return false;
			qvec=tvec%e1;
			v=dir*qvec;
			if (v < 0.0 || u + v > det) return false;
		}
		else if(det < -(1e-3))
		{
			tvec=_point[0]-_coord[0];
			u=tvec*pvec;
			if (u > 0.0f || u < det) return false;
			qvec=tvec%e1;
			v=dir*qvec;
			if (v > 0.0 || u + v < det) return false;
		}
		else 
		{
			return false;
		}
		invDet = 1.0/det;
		t = e2*qvec;
		t *= invDet;
		u *= invDet;
		v *= invDet;
		_intersection->x=t;
		_intersection->y=u;
		_intersection->z=v;
		return true;
	}
	bool isProjectedPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d *_coord)
	{
		double t=getFacePointDistance(_point,_normal,_coord[0]);
		return isPointOnTriangle(_point-t*_normal/_normal.abs(),_normal,_coord);
	}

	Vector3d getProjectedPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d _coord)
	{
		double t=getFacePointDistance(_point,_normal,_coord);
		return _point-t*_normal/_normal.abs();
	}
	bool isPointOnFiniteFace(Vector3d _point, FACET *_facet)
	{
		double mLength;
		Vector3d tCoord[3];
		for(int j=0;j<_facet->numVertex-2;j++){
			tCoord[0]=_facet->vertex[0];
			tCoord[1]=_facet->vertex[j+1];
			tCoord[2]=_facet->vertex[j+2];
			if(isPointOnTriangle(_point, _facet->normal[0], tCoord))
				return true;
		}
		return false;
	}
	bool isProjectedPointOnFiniteFace(Vector3d _point, FACET *_facet)
	{
		Vector3d pPoint;
		pPoint=getFacePointProjection(_point, _facet->normal[0], _facet->normal[0], _facet->vertex[0]);	
		return isPointOnFiniteFace(pPoint,_facet);
	}
}