#include "ms_modelhandler.h"

namespace ModelStudio
{

	bool convertSTL2VOX(STLMESH *_stl, VOXMODEL *_vox, int _num)
	{
		std::cout<<"Converting STL data to VOX data ...";

		_vox->setIsDataLoaded(false);
		_vox->deleteVoxel();
		_vox->setVoxelSameNum(_num);
		_vox->newVoxel();
		_vox->setScale(_stl->getScale());
		_vox->setCenter(_stl->getCenter());
		_vox->calCoordinateSetting();
		_vox->setIsDataLoaded(true);

		for(int i=0;i<_stl->getNumFacet();i++){
			testIntersection(_stl->getFacetPointer(i), _vox, 255);
		}

		std::cout<<"[OK]"<<std::endl;
		return true;
	}
	bool convertOBJ2VOX(OBJMESH *_obj, VOXMODEL *_vox, int _num)
	{
		std::cout<<"Converting OBJ data to VOX data ...";

		_vox->setIsDataLoaded(false);
		_vox->deleteVoxel();
		_vox->setVoxelSameNum(_num);
		_vox->newVoxel();
		_vox->setScale(_obj->getScale());
		_vox->setCenter(_obj->getCenter());
		_vox->calCoordinateSetting();
		_vox->setIsDataLoaded(true);

		for(int i=0;i<_obj->getNumFacet();i++){
			Vector3d t_color;
			t_color.x=_obj->getMaterial(_obj->getFacet(i).materialIndex).getDiffuse().x;
			t_color.y=_obj->getMaterial(_obj->getFacet(i).materialIndex).getDiffuse().y;
			t_color.z=_obj->getMaterial(_obj->getFacet(i).materialIndex).getDiffuse().z;
			testIntersection(_obj->getFacetPointer(i),_vox,t_color);
		}

		std::cout<<"[OK]"<<std::endl;
		return true;
	}
	bool convertOBJ2STL(OBJMESH *_obj, STLMESH *_stl)
	{
		std::cout<<"Converting OBJ data to STL data ...";

		_stl->deleteMesh();
		_stl->deleteMaterial();
		_stl->setNumVertex(_obj->getNumVertex());
		_stl->setNumFacet(_obj->getNumFacet());
		_stl->setNumNormal(_obj->getNumNormal());
		_stl->setNumMaterial(_obj->getNumMaterial());
		_stl->setNumLine(_obj->getNumLine());
		_stl->newMesh();
		_stl->newMaterial();
		for(int i = 0; i < _obj->getNumFacet(); i++){
			_stl->setFacet(i, _obj->getFacet(i));
		}
		for(int i = 0; i < _obj->getNumNormal(); i++){
			_stl->setNormal(i, _obj->getNormal(i));
		}
		for(int i = 0; i < _obj->getNumVertex(); i++){
			_stl->setVertex(i, _obj->getVertex(i));
		}
		_stl->setup();

		std::cout<<"[OK]"<<std::endl;
		return true;
	}
	bool convertSTL2OBJ(STLMESH *_stl, OBJMESH *_obj)
	{
		std::cout<<"Converting STL data to OBJ data ...";

		_obj->deleteMesh();
		_obj->deleteMaterial();
		_obj->setNumVertex(_stl->getNumVertex());
		_obj->setNumFacet(_stl->getNumFacet());
		_obj->setNumNormal(_stl->getNumNormal());
		_obj->setNumMaterial(_stl->getNumMaterial());
		_obj->setNumLine(_stl->getNumLine());
		_obj->newMesh();
		_obj->newMaterial();
		for(int i = 0; i < _stl->getNumFacet(); i++){
			_obj->setFacet(i, _stl->getFacet(i));
			_obj->setNormal(i, _stl->getNormal(i));
		}
		for(int i = 0; i < _stl->getNumVertex(); i++){
			_obj->setVertex(i, _stl->getVertex(i));
		}
		_obj->setup();

		std::cout<<"[OK]"<<std::endl;
		return true;
	}
	void testIntersection(FACET *_facet, VOXMODEL *_vox, int _map, int _thickness)
	{
		AABBtest isIntersectFaceBox;
		double vp[9];
		int j,k,l;
		Vector3d Wmin,Wmax;
		Vector3d Vmin,Vmax;
		for(int ivp=0;ivp<3;ivp++)
			for(int dim=0;dim<3;dim++)
				vp[3*ivp+dim]=_facet->vertex[ivp].X[dim];
		Wmin=_facet->getMinVertex();
		Wmax=_facet->getMaxVertex();
		Vmin=_vox->Tr.getModel2Voxel()*Wmin;
		Vmax=_vox->Tr.getModel2Voxel()*Wmax;
		for(j=Vmin.x-_thickness;j<Vmax.x+_thickness;j++){
			if(0<=j&&j<_vox->getWidth()){
				for(k=Vmin.y-_thickness;k<Vmax.y+_thickness;k++){
					if(0<=k&&k<_vox->getHeight()){
						for(l=Vmin.z-_thickness;l<Vmax.z+_thickness;l++){
							if(0<=l&&l<_vox->getDepth()){
								isIntersectFaceBox.setData(_vox->getSize().x/2.0,_vox->getCenter(j,k,l).X,vp);
								if(isIntersectFaceBox.triBoxOverlap()){
									_vox->setVoxelValue(j,k,l,_map);
								}
							}
						}
					}
				}
			}
		}
	}
	void testIntersection(FACET *_facet, VOXMODEL *_vox, Vector3d _color, int _thickness)
	{
		AABBtest isIntersectFaceBox;
		double vp[9];
		int j,k,l;
		Vector3d Wmin,Wmax;
		Vector3d Vmin,Vmax;
		for(int ivp=0;ivp<3;ivp++)
			for(int dim=0;dim<3;dim++)
				vp[3*ivp+dim]=_facet->vertex[ivp].X[dim];
		Wmin=_facet->getMinVertex();
		Wmax=_facet->getMaxVertex();
		Vmin=_vox->Tr.getModel2Voxel()*Wmin;
		Vmax=_vox->Tr.getModel2Voxel()*Wmax;
		for(j=Vmin.x-_thickness;j<Vmax.x+_thickness;j++){
			if(0<=j&&j<_vox->getWidth()){
				for(k=Vmin.y-_thickness;k<Vmax.y+_thickness;k++){
					if(0<=k&&k<_vox->getHeight()){
						for(l=Vmin.z-_thickness;l<Vmax.z+_thickness;l++){
							if(0<=l&&l<_vox->getDepth()){
								isIntersectFaceBox.setData(_vox->getSize().x/2.0,_vox->getCenter(j,k,l).X,vp);
								if(isIntersectFaceBox.triBoxOverlap()){
									_vox->setVoxelColor(j,k,l,_color);
								}
							}
						}
					}
				}
			}
		}
	}
	bool calOBJandVOXcollision(OBJMESH *_obj, VOXMODEL *_vox, int *_index)
	{
		int num = _obj->getNumVertex();
		//ボクセルのフラグを開放する
		int index;
		double max=0;
		bool isCollide=false;
		*_index=-1;
		if(_vox->getIsLoaded()){
			for(int i=0;i<num;i++){
				//まず座標を変換、次にボクセルを参照、ボクセルにフラグを立てる
				index=_vox->getIndexOfVoxelAt(_obj->Tr.getWorld2Model()*_obj->getVertex(i));
				if(index!=-1){
					if(_vox->getColorAt(index).abs()!=0){
						isCollide=true;
					}
				}
			}
		}
		return isCollide;
	}
	/*
	double calBLADEandVOXcollision(transferMatrixd _Tw2o, BLADE *_blade, VOXMODEL *_vox, int *_index)
	{
		int num = _blade->getNumPc();
		int index;
		double max=0;
		bool isCollide=false;
		*_index=-1;
		if(_vox->getIsLoaded()){
			for(int i=0;i<num;i++){
				index=_vox->getIndexOfVoxelAt(_Tw2o*_blade->getPcAt(i));
				if(index!=-1){
					if(_vox->getColorAt(index).abs()!=0){
						isCollide=true;
						if(max<_vox->getMapAt(index,true,_Tw2o*_blade->getPcAt(i))){
							max=_vox->getMapAt(index,true,_Tw2o*_blade->getPcAt(i));
							*_index=i;
						}
					}
				}
			}
		}
		return max;
	}
	bool calBLADEandVOXcollision(transferMatrixd _Tw2o, BLADE *_blade, VOXMODEL *_vox, int *_index, Vector3d *_points, int *_numPoints)
	{
		*_numPoints=0;
		int numPoints=0;
		int num = _blade->getNumPc();
		int index=-1;
		bool isCollide=false;
		double max=0;
		*_index=-1;
		if(_vox->getIsLoaded()){
			for(int i=0;i<num;i++){
				index=_vox->getIndexOfVoxelAt(_Tw2o*_blade->getPcAt(i));
				if(index!=-1){
					if(_vox->getColorAt(index).abs()!=0){
						isCollide=true;
						if(_vox->getColorAt(index)!=INVIS_COL){
							_points[numPoints]=_vox->Tr.getModel2World()*_Tw2o*_blade->getPcAt(i);
							numPoints++;
						}
						if(max<_vox->getMapAt(index,true,_Tw2o*_blade->getPcAt(i))){
							max=_vox->getMapAt(index,true,_Tw2o*_blade->getPcAt(i));
							*_index=i;
						}
					}
				}
			}
		}
		*_numPoints=numPoints;
		return isCollide;
	}*/
	/*
	double calOBJandVOXdistance(OBJMESH *_obj, VOXMODEL *_vox, int *_index)
	{
		double min=INT_MAX;
		double dist=0;
		if(_vox->getIsLoaded()){
			for(int i=0;i<_vox->getNumVoxel();i++){
				if(_vox->getColorAt(i)==SURF_COL){
					dist=(_obj->Tr.getWorld2Model()*_obj->getTri()->getCenterCoord()-_vox->Tr.getWorld2Model()*_vox->getCenter(i)).abs();
					if(min>dist)
						min=dist;
				}
			}
		}
		return min;
	}
	*/
	void millingVOXbyOBJ(OBJMESH *_obj, VOXMODEL *_vox)
	{
		int num = _obj->getNumVertex();
		//ボクセルのフラグを開放する
		int index;
		int max=0;
		bool isCollide=false;
		if(_vox->getIsLoaded()){
			for(int i=0;i<num;i++){
				//まず座標を変換、次にボクセルを参照、ボクセルにフラグを立てる
				index=_vox->getIndexOfVoxelAt(_obj->Tr.getWorld2Model()*_obj->getVertex(i));
				if(index!=-1)
					_vox->millingSurfaceWith(index);
			}
		}
	}
	void millingVOXbyPOINTS(VOXMODEL *_vox, Vector3d *_points, double *_depth, int _numPoints)
	{
		int index;
		if(_vox->getIsLoaded()){
			for(int i=0;i<_numPoints;i++){
				index=_vox->getIndexOfVoxelAt(_points[i],COORD_LOCAL);
				if(index!=-1){
					if(_vox->getColorAt(index)!=INVIS_COL){
						_vox->coloringSurfaceWith(index, INVIS_COL);
					}
					else _depth[i]=0;
				}
				else _depth[i]=0;
			}
		}
	}

	void spanLabelVOX2OBJ(VOXMODEL *_vox, OBJMESH *_obj)
	{
		Vector3f tcolor;
		for(int i=0;i<_obj->getNumVertex();i++){
			tcolor.x=_vox->getColorAt(_vox->getIndexOfVoxelAt(_obj->getVertex(i))).x;
			tcolor.y=_vox->getColorAt(_vox->getIndexOfVoxelAt(_obj->getVertex(i))).y;
			tcolor.z=_vox->getColorAt(_vox->getIndexOfVoxelAt(_obj->getVertex(i))).z;
			_obj->setColorAt(i,tcolor);
			_obj->setLabelIndexAt(i,_vox->getLabelIndexAt(_vox->getIndexOfVoxelAt(_obj->getVertex(i))));
		}
	}
	void setLabelOBJNearest(OBJMESH *_obj)
	{
		double mdist;
		for(int i=0;i<_obj->getNumVertex();i++){
			mdist=1000;
			if(_obj->getLabelIndex(i)==-1){
				for(int j=0;j<_obj->getNumVertex();j++){
					if(_obj->getLabelIndex(j)!=-1){
						if(mdist>(_obj->getVertex(i)-_obj->getVertex(j)).abs()){
							mdist=(_obj->getVertex(i)-_obj->getVertex(j)).abs();
							_obj->setLabelIndexAt(i,_obj->getLabelIndex(j));
							_obj->setColorAt(i,_obj->getColor(j));
						}
					}
				}
			}
		}
	}
	void selectSTL(STLMESH *_stl, Vector3d *_selected_coord)
	{
		Vector3d select_normal;
		bool is_po_wi_tr = false;
		for (int i = 0; i<_stl->getNumFacet(); i++) {
			_stl->getFacetPointer(i)->isSelected = false;
		}

		for (int i = 0; i<_stl->getNumVertex(); i++) {
			for (int j = 0; j<4; j++) {
				select_normal = (_selected_coord[(j + 1) % 4] - _selected_coord[j])
					% (_selected_coord[j + 4] - _selected_coord[j]);
				select_normal /= select_normal.abs();

				is_po_wi_tr = ModelStudio::isPointWithinTriangle(_stl->Tr.getModel2Uni()*_stl->getVertex(i), select_normal, _selected_coord[j]);

				if (!is_po_wi_tr) {
					if (j == 3) {
						for (int k = 0; k<_stl->getFacetIndexAt(i).size(); k++) {
							_stl->getFacetPointer(_stl->getFacetIndexAt(i)[k])->isSelected = true;
						}
					}
				}
				else {
					break;
				}
			}
		}
	}
	void pickupFacet(STLMESH *_stl, Vector3d _coord, int _mode)
	{
		Vector3d select_normal;
		bool is_on_facet = false;
		if(_mode == GLUT_ACTIVE_SHIFT){}
		else {
			for (int i = 0; i<_stl->getNumFacet(); i++) {
				_stl->getFacetPointer(i)->isSelected = false;
			}
		}

		for (int i = 0; i<_stl->getNumFacet(); i++) {
			is_on_facet = ModelStudio::isPointOnTriangle( _stl->Tr.getUni2Model()*_coord, _stl->getFacetPointer(i)->normal[0], _stl->getFacetPointer(i)->vertex );
			if (is_on_facet) {
				_stl->getFacetPointer(i)->isSelected = !_stl->getFacetPointer(i)->isSelected;
				break;
			}
		}
	}

	/*
	void clippingVOXbyPLANE(VOXMODEL *_vox, PLANE *_plane, int _numPlane)
	{
		bool isInside=true;
		for(int i=0;i<_vox->getNumVoxel();i++){
			if(_vox->getColorAt(i)==SURF_COL){
				isInside=true;
				for(int j=0;j<_numPlane;j++)
					if(_plane[j].getNormal()*(_vox->getCenter(i)-_plane->getCenter())>=0)
						isInside=false;
				if(isInside)
					_vox->coloringSurfaceWith(i, INVIS_COL);
				//				_vox->setFreeSpaceAt(i);
			}
		}
	}
	*/
	transferMatrixd ICP::getOptimalRegistration(OBJMESH *_objA, OBJMESH *_objB, int _mode, double minError)
	{
		int numPoint=_objA->getNumVertex();
		transferMatrixd Ta2b;
		Vector3d *pointA=new Vector3d[numPoint];
		Vector3d *pointB=new Vector3d[numPoint];
		double *error=new double[numPoint];
		double dE=1000;
		double pError=0;
		double cError=0;


		//TODO:LOOP
		while(dE>minError){

			//位置合わせを行う点群を設定
			for(int i=0;i<numPoint;i++)
				pointA[i]=_objA->Tr.getWorld2Model()*_objA->getVertex(i);

			//最近傍探索点を取得
			ICP::matching(numPoint, pointA, pointB, _objB, _mode);
			//誤差の取得
			pError=cError;
			cError=ICP::errorMetric(error, pointB, _objA);
			dE=fabs(cError-pError);
			//最適化を行うための変換行列を取得
			Ta2b=ICP::minimizing(numPoint, pointA, pointB);
			//変換行列の更新
			_objA->Tr.setWorld2Model(Ta2b*_objA->Tr.getWorld2Model());

			std::cout<<"dE:"<<dE<<std::endl;
		}
		std::cout<<"Mean Square Error:"<<cError<<std::endl;
		std::cout<<"Transfer Matrix:"<<std::endl;
		std::cout<<_objA->Tr.getWorld2Model()<<std::endl;

		delete []pointA;
		delete []pointB;
		delete []error;
		return _objA->Tr.getWorld2Model();
	}
	//最近傍点の取得
	void  ICP::matching(int _numPoint, Vector3d *_pointA, Vector3d *_pointB, OBJMESH *_objB, int _mode)
	{
		double dist=0;
		double mDist=INT_MAX;
		Vector3d normal;
		Vector3d vertex;
		for(int i=0;i<_numPoint;i++){
			if(_mode==ICP2P){//点と点の場合
				for(int j=0;j<_objB->getNumVertex();j++){
				}
			}
			if(_mode==ICP2S){//点と面の場合
				mDist=INT_MAX;
				for(int j=0;j<_objB->getNumFacet();j++){
					normal=_objB->getFacetPointer(j)->normal[0];
					vertex=_objB->getFacetPointer(j)->vertex[0];
					dist=getFacePointDistance(_pointA[i],normal,vertex);
					if(mDist>dist){
						mDist=dist;
						_pointB[i]=getFacePointProjection(_pointA[i], normal, vertex);
					}
				}
			}
		}
	}
	double  ICP::errorMetric(double *_error, Vector3d *_pointB, OBJMESH *_objA)
	{
		double error=0;
		double max=0;
		double min=INT_MAX;
		for(int i=0;i<_objA->getNumVertex();i++){
			_error[i]=(_pointB[i]-_objA->Tr.getWorld2Model()*_objA->getVertex(i)).abs();
			if(max<_error[i])
				max=_error[i];
			if(min>_error[i])
				min=_error[i];
			error+=_error[i]*_error[i];
		}
		for(int i=0;i<_objA->getNumVertex();i++){
			if(max!=0){
				Vector3d color=HSV2RGB(Vector3d(240.0-240.0*(_error[i]-min)/(max-min),1,1));
				_objA->setColorAt(i,Vector3f(color.x, color.y, color.z));
			}
		}
		return sqrt(error)/_objA->getNumVertex();
	}
	Vector3d  ICP::centering(int _numPoint, Vector3d *_point)
	{
		Vector3d center;
		for(int i=0;i<_numPoint;i++)
			center+=_point[i]/(double)_numPoint;
		return center;
	}
	Vector3d  ICP::translating(int _numPoint, Vector3d *_pointA, Vector3d *_pointB){
		Vector3d centerA=ICP::centering(_numPoint,_pointA);
		Vector3d centerB=ICP::centering(_numPoint,_pointB);
		return centerB-centerA;
	}
	transferMatrixd ICP::rotating(int _numPoint, Vector3d *_pointA, Vector3d *_pointB)
	{
		transferMatrixd Trot;
		Matrixd Xa = Matrixd(_numPoint, 3);
		Matrixd Xb = Matrixd(_numPoint, 3);
		Matrixd R = Matrixd(3, 3);
		for(int i=0;i<_numPoint;i++){
			Xa.X[_numPoint*0+i]=_pointA[i].x;
			Xa.X[_numPoint*1+i]=_pointA[i].y;
			Xa.X[_numPoint*2+i]=_pointA[i].z;

			Xb.X[_numPoint*0+i]=_pointB[i].x;
			Xb.X[_numPoint*1+i]=_pointB[i].y;
			Xb.X[_numPoint*2+i]=_pointB[i].z;
		}
		if((Xa*Xa.trn()).det()!=0){
			R=Xb*Xa.trn()*(Xa*Xa.trn()).inv();
			Vector3d nx=Vector3d(R.X[0],R.X[1],R.X[2]);
			Vector3d ny=Vector3d(R.X[3],R.X[4],R.X[5]);
			Vector3d nz=Vector3d(R.X[6],R.X[7],R.X[8]);
			Trot.setRotMatrix(nx/nx.abs(), ny/ny.abs(), nz/nz.abs());
		}
		Xa.free();
		Xb.free();
		R.free();
		return Trot;
	}
	transferMatrixd ICP::minimizing(int _numPoint, Vector3d *_pointA, Vector3d *_pointB)
	{
		transferMatrixd Ttran;
		transferMatrixd Trot;
		transferMatrixd TcenterA;
		transferMatrixd TcenterB;
		Vector3d *pointA=new Vector3d[_numPoint];
		Vector3d *pointB=new Vector3d[_numPoint];
		Ttran.setTranslate(ICP::translating(_numPoint, _pointA, _pointB));
		TcenterA.setTranslate(ICP::centering(_numPoint,_pointA));
		TcenterB.setTranslate(ICP::centering(_numPoint,_pointB));

		for(int i=0;i<_numPoint;i++){
			pointA[i]=TcenterA.inv()*_pointA[i];
			pointB[i]=TcenterB.inv()*_pointB[i];
		}
		Trot=ICP::rotating(_numPoint, pointA, pointB);

		delete []pointA;
		delete []pointB;

		return Ttran*TcenterA*Trot*TcenterA.inv();
	}

	transferMatrixd calTransMatBetween3(Vector3d *_p1, Vector3d *_p2)
	{
		Matrixd Pp(4,4);
		Matrixd Pn(4,4);
		Matrixd tTm2w;
		transferMatrixd Tdst;
		Vector3d temp1=(_p1[1]-_p1[0])%(_p1[2]-_p1[0]);
		Vector3d temp2=(_p2[1]-_p2[0])%(_p2[2]-_p2[0]);
		if(temp1.abs()!=0)temp1/=temp1.abs();
		if(temp2.abs()!=0)temp2/=temp2.abs();
		for(int j=0;j<4;j++){
			Pp.X[4*3+j]=1;
			Pn.X[4*3+j]=1;
		}
		for(int j=0;j<3;j++){
			for(int k=0;k<3;k++){
				Pp.X[4*k+j]=_p1[j].X[k];
				Pn.X[4*k+j]=_p2[j].X[k];
			}
		}
		for(int k=0;k<3;k++){
			Pp.X[4*k+3]=temp1.X[k];
			Pn.X[4*k+3]=temp2.X[k];
		}
		tTm2w=Pn*Pp.trn()*(Pp*Pp.trn()).inv();
		memcpy(Tdst.X,tTm2w.X,sizeof(double)*4*4);
		return Tdst;
	}

	double calLongAxisFromPointCloud(int _num, Vector3d *_point, int *_index)
	{
		double dst=0;
		for(int i=0;i<_num;i++){
			for(int j=_num-1;j>=0;j--){
				if(i!=j){
					if(dst<(_point[i]-_point[j]).abs()){
						dst=(_point[i]-_point[j]).abs();
						_index[0]=i;
						_index[1]=j;
					}
				}
			}
		}
		return dst;
	}
};

/*

bool INTERACTION::intractINSTandVOX(Instrument *_inst, voxelHandler *_vox)
{
	bool isInteracted=false;
	if(!_inst->getTool()->getIsLoaded()||!_vox->getIsLoaded())return isInteracted;
	int num=_inst->getTool()->num_vertex;
	Vector3d Pworld;
	int max=0;
	for(int i=0; i<num; i++){
		Pworld=_inst->getTw2v()*_inst->getTi2w()*_inst->getTool()->vertex[i]*2;//VRスペースでは座標を2倍にする
		if(_vox->millingSurfaceWith(Pworld))
			isInteracted=true;
	}
	return isInteracted;
}


int INTERACTION::navigateINSTwithVOX(Instrument *_inst, voxelHandler *_vox)
{
	if(!_inst->getTool()->getIsLoaded()||!_vox->getIsLoaded())return 0;
	int num=_inst->getTool()->num_vertex;
	Vector3d Pworld;
	int max=0;
	int val;
	for(int i=0; i<num; i++){
//		Pworld=_inst->getTw2v()*_inst->getTi2w()*_inst->getTool()->vertex[i];//オリジナル
		Pworld=_inst->getTi2w()*_inst->getTool()->vertex[i];//仮に２倍にしている
		val=_vox->navigateWith(Pworld);
		if(max<val)
			max=val;
	}
	return max;
}



*/