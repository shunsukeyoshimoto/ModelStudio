#pragma once
#include <ms_pointlineface.h>
#include <ms_triangleboxtest.h>
#include <ms_voxelhandler.h>
#include <ms_stlhandler.h>
#include <ms_objhandler.h>

namespace ModelStudio
{
	bool convertSTL2VOX(STLMESH *_stl, VOXMODEL *_vox, int _num=64);
	bool convertOBJ2VOX(OBJMESH *_obj, VOXMODEL *_vox, int _num=64);
	bool convertSTL2OBJ(STLMESH *_stl, OBJMESH *_obj);
	bool convertOBJ2STL(OBJMESH *_obj, STLMESH *_stl);
	void testIntersection(FACET *_mesh, VOXMODEL *_vox, int _map=255, int _thickness=3);
	void testIntersection(FACET *_mesh, VOXMODEL *_vox, Vector3d _color=Vector3d(0,0,1), int _thickness=3);
	bool calOBJandVOXcollision(OBJMESH *_obj, VOXMODEL *_vox, int *_index); 
//	double calBLADEandVOXcollision(transferMatrixd _Tw2o, BLADE *_blade, VOXMODEL *_vox, int *_index); 
//	bool calBLADEandVOXcollision(transferMatrixd _Tw2o, BLADE *_blade, VOXMODEL *_vox, int *_index, Vector3d *_points, int *_numPoints); 
	double calOBJandVOXdistance(OBJMESH *_obj, VOXMODEL *_vox, int *_index); 
	void millingVOXbyOBJ(OBJMESH *_obj, VOXMODEL *_vox);
	void millingVOXbyPOINTS(VOXMODEL *_vox, Vector3d *_points, double *depth, int _numPoints=1);
//	void clippingVOXbyPLANE(VOXMODEL *_vox, PLANE *_plane, int _numPlane=1);

	void spanLabelVOX2OBJ(VOXMODEL *_vox, OBJMESH *_obj);
	void setLabelOBJNearest(OBJMESH *_obj);
	void selectSTL(STLMESH *_stl, Vector3d *_selected_coord);
	void pickupFacet(STLMESH *_stl, Vector3d _coord, int _mode);

	namespace ICP{
		enum MatchingType{
			ICP2P,ICP2S
		};
		transferMatrixd getOptimalRegistration(OBJMESH *_objA, OBJMESH *_objB, int _mode=ICP2P, double minError=0.00001);
		void matching(int _numPoint, Vector3d *_pointA, Vector3d *_pointB, OBJMESH *_objB, int _mode);
		double errorMetric(double *_error, Vector3d *_pointB, OBJMESH *_objA);
		Vector3d centering(int _numPoint, Vector3d *_point);
		Vector3d translating(int _numPoint, Vector3d *_pointA, Vector3d *_pointB);
		transferMatrixd rotating(int _numPoint, Vector3d *_pointA, Vector3d *_pointB);
		transferMatrixd minimizing(int _numPoint, Vector3d *_pointA, Vector3d *_pointB);
	};
	transferMatrixd calTransMatBetween3(Vector3d *_p1, Vector3d *_p2);
	double calLongAxisFromPointCloud(int _num, Vector3d *_point, int *_index);
};
