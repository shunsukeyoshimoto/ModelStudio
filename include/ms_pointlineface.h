#pragma once
#include <ms_vectormatrix.h>
#include <ms_surfMesh.h>

namespace ModelStudio{
	class FACET;

	//�_�Ɩʂ̋������v�Z����Method
	double getFacePointDistance(Vector3d _point, Vector3d _normal, Vector3d _coord);
	//�_��ʂɐ����ɉ��낵����_���v�Z����Method
	Vector3d getFacePointProjection(Vector3d _point, Vector3d _lNormal, Vector3d _sNormal, Vector3d _coord);
	Vector3d getFacePointProjection(Vector3d _point, Vector3d _normal, Vector3d _coord);
	//���Ɩʂ̌�_���v�Z����Method
	Vector3d getIntersectPointFace(Vector3d *_point, Vector3d _normal, Vector3d _coord);
	//2�_�ŗ^�����镽�s�ʂ̊Ԃɓ_�����邩���v�Z����Method
	bool isFacesSandPoint(Vector3d _coord1, Vector3d _coord2, Vector3d _point);
	//2�_���ʂ��͂���ł��邩�ǂ����v�Z����Method
	bool isPointsSandFace(Vector3d *_point, Vector3d _normal, Vector3d _coord);
	//����_��3�p�`�̓����ɂ��邱�Ƃ��v�Z����Method
	bool isPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d *_coord);
	//TomasMoller�̌�������
	bool isIntersectLineTriangle(Vector3d *_point, Vector3d *_coord, Vector3d *_intersection);
	//�ʂ̓������O�����𔻒肷��Method
	bool isPointWithinTriangle(Vector3d _point, Vector3d _normal, Vector3d _coord);
	//�_���O�p�`���ɓ��e����邩�𔻒肷��Method
	bool isProjectedPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d *_coord);
	Vector3d getProjectedPointOnTriangle(Vector3d _point, Vector3d _normal, Vector3d _coord);
	//�_���L���ʏ�ɂ��邩�𔻒肷��Method
	bool isPointOnFiniteFace(Vector3d _point, FACET *_facet);
	bool isProjectedPointOnFiniteFace(Vector3d _point, FACET *_facet);
	//�_�Ɛ��̍ŒZ���������߂�Method
	double getPointLineDistance(Vector3d _point, Vector3d _dir, Vector3d _coord);
};
