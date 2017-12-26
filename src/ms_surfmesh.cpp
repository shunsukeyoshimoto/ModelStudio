#include "ms_surfmesh.h"


namespace ModelStudio{
	surfMesh::surfMesh(void)
		:numVertex(0),numNormal(0),numFacet(0),numLine(0),numMaterial(0),id(0)
		,isView(true),isLine(false),isPoint(false),isFacet(true)
		,isAffine(true),isLoaded(false),isTri(false),isIdentity(false)
		,isSelect(false),isAlphaBlend(false),isLineCalculated(false)
		,isListed(true),isUseDisplayList(false),isVertexColorEnabled(false),
		isViewLabel(false)
	{
		this->newMesh();
		this->newMaterial();
		sMat.setDiffuse(Vector4f(1,1,0,0.5));
		sMat.setAmbient(Vector4f(1,1,0,0.5));
		cMat.setAmbient(Vector4f(0.0,0.0,0.0,1));
		cMat.setSpecular(Vector4f(1.0,1.0,1.0,1));
		cMat.setShininess(0);
		cMat.setIllum(4);
	}


	surfMesh::~surfMesh(void)
	{
		this->deleteMesh();
		this->deleteMaterial();
	}

	void surfMesh::deleteMesh()
	{
		this->isLoaded=false;
		delete []vertex;
		delete []facet;
		delete []normal;
		delete []labelIndex;
		delete []color;
		delete []line;
		delete []isSelected;
	}

	void surfMesh::newMesh()
	{
		this->vertex=new Vector3d[this->numVertex+2];
		this->facet_index.resize(this->numVertex+2);
		this->color=new Vector3f[this->numVertex+2];
		this->labelIndex=new int[this->numVertex+2];
		this->facet=new FACET[this->numFacet+2];
		this->normal=new Vector3d[this->numNormal+2];
		this->isSelected=new bool[this->numVertex+2];
		this->line=new LINE[this->numLine+2];

		for(int i=0;i<numVertex+2;i++){
			isSelected[i]=false;
			labelIndex[i]=-1;
			this->facet_index[i].clear();
		}
	}

	void surfMesh::deleteMaterial()
	{
		delete []material;
	}
	void surfMesh::newMaterial()
	{
		this->material=new Material[this->numMaterial+2];
		this->material[0].setDiffuse(Vector4f(0.5,0.5,0.5,0.5));
		this->material[0].setAmbient(Vector4f(0.0,0.0,0.0,0.5));
	}

	void surfMesh::clearMesh()
	{
		this->isListed=false;
		this->isLoaded=false;
		this->isLineCalculated=false;
		this->isVertexColorEnabled=false;
		this->isUseDisplayList=false;
		numVertex = 0;
		numNormal = 0;
		numFacet = 0;
		numMaterial = 0;
		numLine = 0;
		this->scale=0;
		this->size=Vector3d(1,1,1);
		this->center=Vector3d(0,0,0);
	}
	void surfMesh::clearView()
	{
		this->isView=true;
		this->isFacet=true;
		this->isLine=false;
		this->isPoint=false;
		this->isViewLabel=true;
	}
	void surfMesh::addVertex(Vector3d &_vertex)
	{
		numVertex++;
		vertex = (Vector3d*)realloc(vertex, numVertex*sizeof(Vector3d));
		vertex[numVertex-1] = _vertex;
		isSelected = (bool*)realloc(isSelected, numVertex*sizeof(bool));
		isSelected[numVertex-1]=false;
	}
	void surfMesh::addNormal(Vector3d &_normal)
	{
		numNormal++;
		normal = (Vector3d*)realloc(normal, numNormal*sizeof(Vector3d));
		normal[numNormal-1] = _normal;
	}
	void surfMesh::addFacet(FACET &_facet)
	{
		numFacet++;
		facet = (FACET*)realloc(facet, numFacet*sizeof(FACET));
		facet[numFacet-1] = _facet;
	}
	void surfMesh::addMaterial(Material &_material)
	{
		numMaterial++;
		material = (Material*)realloc(material, numMaterial*sizeof(Material));
		material[numMaterial-1] = _material;
	}
	std::ostream& surfMesh::getInfo(std::ostream &stream)
	{
		stream<<"Number of vertex：" << numVertex << std::endl;
		stream<<"Number of normal：" << numNormal << std::endl;
		stream<<"Number of surface：" << numFacet << std::endl;
		stream<<"Number of material：" << numMaterial << std::endl;
		return stream;
	}

	void surfMesh::calFacetNormal()
	{
		Vector3d temp[2];
	//	if(this->facet[0].normalType==NORMAL_NONE){
			delete []normal;
			this->normal=new Vector3d[this->numFacet];
			for(int i=0;i<numFacet;i++){
				this->facet[i].normalType=NORMAL_FACET;
				this->facet[i].numNormal=1;
				for(int j=1;j<3;j++)
					temp[j-1]=vertex[facet[i].vertexIndex[j]]-vertex[facet[i].vertexIndex[0]];
				for(int j=0;j<3;j++){
					this->normal[i].X[j] =temp[1].X[(j+1)%3]*temp[0].X[(j+2)%3]
							-temp[1].X[(j+2)%3]*temp[0].X[(j+1)%3];
				}
				if(this->normal[i].abs()!=0)this->normal[i]/=this->normal[i].abs();
				this->facet[i].normalIndex[0]=i;
				this->facet[i].normal[0]=this->normal[i];
			}
	//	}
	/*
		if(this->facet[0].normalType==NORMAL_POINT){
			for(int i=0;i<numFacet;i++)
				for(int j=0;j<facet[i].numNormal;j++)
					this->facet[i].normal[0]=this->normal[facet[i].normalIndex[j]];
		}
		*/
	}
	void surfMesh::calFacetVertex()
	{
		for(int i=0;i<this->numFacet;i++){
			for(int j=0;j<this->facet[i].numVertex;j++){
				this->facet[i].vertex[j]=vertex[this->facet[i].vertexIndex[j]];
			}
		}
	}
	void surfMesh::calFacetIndex()
	{
		for (int i = 0; i<this->numFacet; i++) {
			for (int j = 0; j<this->facet[i].numVertex; j++) {
				this->facet_index[this->facet[i].vertexIndex[j]].push_back(i);
			}
		}
	}
	void surfMesh::calLine()
	{
		if(this->numLine>1)
			delete []this->line;
		this->numLine=0;
		this->line= new LINE[this->numFacet*4];//四角形までしか想定していない

		for(int i=0;i<this->numFacet;i++){
			for(int j=0;j<facet[i].numVertex;j++){
				facet[i].line[j].vertexIndex[0]=facet[i].vertexIndex[j];
				facet[i].line[j].vertexIndex[1]=facet[i].vertexIndex[(j+1)%facet[i].numVertex];
				facet[i].line[j].vertex[0]=facet[i].vertex[j];
				facet[i].line[j].vertex[1]=facet[i].vertex[(j+1)%facet[i].numVertex];
				if(!isSharedLine(facet[i].line[j],line,numLine)){
					this->line[this->numLine]=facet[i].line[j];
					this->numLine++;
				}
			}
		}
		this->isLineCalculated=true;
	}

	void surfMesh::calVertex()
	{
		int count=1;
		this->vertex[0]=this->facet[0].vertex[0];
		for(int i=0;i<this->numFacet;i++){
			for(int j=0;j<3;j++){
				for(int k=0;k<count;k++){
					if(this->facet[i].vertex[j]==this->vertex[k]){
						this->facet[i].vertexIndex[j]=k;
						k=count;
					}
					else if(k==count-1){
						this->vertex[count]=this->facet[i].vertex[j];
						this->facet[i].vertexIndex[j]=count;
						count++;
					}
				}
			}
		}
		this->numVertex=count;
		Vector3d *tempVertex=new Vector3d[this->numVertex];
		memcpy(tempVertex,vertex,sizeof(Vector3d)*this->numVertex);
		delete []vertex;
		this->vertex=tempVertex;
	}
	void surfMesh::calCenter()
	{
		Vector3d min(INT_MAX,INT_MAX,INT_MAX);
		Vector3d max(-INT_MAX,-INT_MAX,-INT_MAX);
		for(int i=0;i<this->numVertex;i++){
			for(int k=0;k<3;k++){
				if(min.X[k]>this->vertex[i].X[k])
					min.X[k]=this->vertex[i].X[k];
				if(max.X[k]<this->vertex[i].X[k])
					max.X[k]=this->vertex[i].X[k];
			}
		}
		this->size.x=fabs(max.x-min.x);
		this->size.y=fabs(max.y-min.y);
		this->size.z=fabs(max.z-min.z);
		this->center=(max+min)/2.0;
	}
	void surfMesh::calScale()
	{
		double max=-INT_MAX;
		for(int i=0;i<3;i++)
			if(max<size.X[i])max=size.X[i];
		scale=max;
	}
	void surfMesh::calTransform()
	{
		transferMatrixd Tscale;
		transferMatrixd Ttrans;
		Tscale.setScale(10/this->scale);
		Ttrans.setTranslate(Vector3d(-this->center.x, -this->center.y, -this->center.z));
		Tr.setModel2Uni(Tscale*Ttrans);
	}

	bool surfMesh::load(const char *_filename)
	{
		return true;
	}
	bool surfMesh::save(const char *_filename)
	{
		return true;
	}


	void surfMesh::render()
	{
		if(!this->isLoaded||!this->isView)
			return;
		glDisable(GL_CULL_FACE); 
		if(this->isAlphaBlend){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glPushMatrix();
		if(!isIdentity){
			if(isAffine){
				glMultMatrixd(Tr.getModel2Uni().getTr4GL());
			}
		}
		if(this->isUseDisplayList){
			if(this->isListed)
				glCallList(id);
			else this->makeDisplayList(0);
		}else renderList();

		glPopMatrix();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_LIGHTING);
	}

	void surfMesh::renderList()
	{
		Vector3d p1,p2;
		int pre_mat = -1, cur_mat = 0;
		if(this->isFacet){
			glEnable(GL_LIGHTING);
			if(this->isViewLabel)
				glDisable(GL_LIGHTING);
			for ( int i=0; i<numFacet; i++ ){
				if(facet[i].isEnabled){
					cur_mat = facet[i].materialIndex;
					if ( pre_mat != cur_mat ){
						material[cur_mat].set();
						pre_mat = cur_mat;
					}
					if(facet[i].isSelected){
						sMat.set();
						pre_mat=-1;
					}
					if(this->facet[i].normalType==NORMAL_FACET)
						glNormal3dv(this->normal[this->facet[i].normalIndex[0]].X);
					glBegin(facet[i].type);
					for ( int j=0; j<facet[i].numVertex; j++ ){
						if(this->facet[i].normalType==NORMAL_POINT)
							glNormal3dv(this->normal[this->facet[i].normalIndex[j]].X);
						glColor3fv(color[facet[i].vertexIndex[j]].X);
						p1=Tr.getModel2World()*vertex[facet[i].vertexIndex[j]];
						glVertex3dv(p1.X);
					}
					glEnd();
				}
			}
		}
		if(this->isLine){
			glDisable(GL_LIGHTING);
			glColor3d(0.1, 0.1, 0.1);
			glLineWidth(0.5);
			if(this->isLineCalculated){
				for(int i=0;i<numLine;i++)
					this->line[i].render();
			}else{
				for(int i=0;i<numFacet;i++){
					glBegin(GL_LINE_LOOP);
					for ( int j=0; j<facet[i].numVertex; j++ )
						glVertex3dv(vertex[facet[i].vertexIndex[j]].X);
					glEnd();
				}
			}
			glEnable(GL_LIGHTING);
		}
		if(this->isPoint){
			glDisable(GL_LIGHTING);
			glPointSize(3);
			for(int i=0;i<numVertex;i++){
				glPushMatrix();
				glMultMatrixd(Tr.getModel2World().getTr4GL());
				if(this->isSelect&&this->isSelected[i])
					glColor3d(1,0,1);
				if(this->isViewLabel){
					glColor3fv(color[i].X);
				}
				else glColor3d(0,1,1);
				glBegin(GL_POINTS);
				glVertex3dv(vertex[i].X);
				glEnd();
				glPopMatrix();
			}
			glEnable(GL_LIGHTING);
		}
	}

	GLuint surfMesh::makeDisplayList(int _window)
	{
		id = glGenLists(1);
		glNewList(id, GL_COMPILE);
		renderList();
		glEndList();
		this->isListed=true;
		return id;
	}

	int surfMesh::getVertexIndexNear(Vector3d _pos)
	{
		Vector3d error;
		Vector3d minError=Vector3d(1000,1000,1000);
		int minErrorIndex=-1;
		for(int i=0;i<this->numVertex;i++){
			if(!this->isIdentity){
				if(isAffine)
					error=(Tr.getModel2Uni()*Tr.getWorld2Model()*this->vertex[i]-_pos);
				else
					error=(Tr.getWorld2Model()*this->vertex[i]-_pos);
			}
			else error=this->vertex[i]-_pos;
			if(error.abs()<minError.abs()&&error.abs()<10){
				minError=error;
				minErrorIndex=i;
			}
			isSelected[i]=false;
		}
		if(minErrorIndex!=-1){
			isSelected[minErrorIndex]=true;
			this->selectedVertex=this->vertex[minErrorIndex];
			this->selectedIndex=minErrorIndex;
			this->isSelect=true;
		}
		else this->isSelect=false;
		return minErrorIndex;
	}
	int surfMesh::getFacetIndexNear(Vector3d _pos)
	{
		double dist;
		double mDist=10;//クリックした点から面までの距離が最大でも10
		int mDistIndex=-1;
		Vector3d Pc_o;
		Vector3d Pf_o;
		if(!this->isIdentity){
			if(isAffine)
				Pc_o=Tr.getModel2World()*Tr.getUni2Model()*_pos;
			else
				Pc_o=Tr.getModel2World()*_pos;
		}
		else Pc_o=_pos;
		for(int i=0;i<this->numFacet;i++){
			if(facet[i].isEnabled){
				facet[i].setIsSelected(false);
				if(isProjectedPointOnFiniteFace(Pc_o, &facet[i])){
					dist=getFacePointDistance(Pc_o, facet[i].normal[0], facet[i].vertex[0]);
					if(mDist>fabs(dist)){
						mDist=fabs(dist);
						mDistIndex=i;
					}
				}
			}
		}
		if(mDistIndex!=-1)
			facet[mDistIndex].setIsSelected(true);
		return mDistIndex;
	}
	void surfMesh::clearSelection()
	{
		for(int i=0;i<numFacet;i++){
			facet[i].isSelected=false;
			this->isSelected[i]=false;
		}
	}

	void surfMesh::quadrization()
	{
		std::cout<<"Quadrizing surface...";
		for(int i=0;i<this->numFacet;i++)
			if(this->facet[i].isEnabled)
				for(int j=0;j<this->numFacet;j++)
					if(i!=j)
						convertTriangle2Quad(&this->facet[i],&this->facet[j]);
		this->calLine();
		std::cout<<"[OK]"<<std::endl;
	}

	void surfMesh::pushLabelList()
	{
		std::vector<int> t_index;
		LABEL t_label;
		for (int i = 0; i < this->numFacet; i++) {
			if (this->facet[i].isSelected) {
				t_index.push_back(i);
				this->facet[i].isSelected = false;
			}
		}
		t_label.index = t_index;
		t_label.label = this->label_list.size();
		this->label_list.push_back(t_label);
	}

	bool surfMesh::saveLabelList(const char *_filename) 
	{
		std::ofstream file;
		file.open(_filename, std::ios::out);
		if (!file.is_open())return false;
		for (int i = 0; i < this->label_list.size(); i++) {
			for (int j = 0; j < this->label_list[i].index.size(); j++) {
				file << this->label_list[i].index[j] << ",8," << this->label_list[i].label;
				if(i!= this->label_list.size() - 1 || j != this->label_list[i].index.size() -1)
					file<< std::endl;
			}
		}
		file.close();
		return true;
	}

	bool isSharedLine(LINE _new, LINE *_old, int _num)
	{
		for(int i=0;i<_num;i++){
			if(_old[i].vertexIndex[0]==_new.vertexIndex[0]&&_old[i].vertexIndex[1]==_new.vertexIndex[1])
				return true;
			else if(_old[i].vertexIndex[0]==_new.vertexIndex[1]&&_old[i].vertexIndex[1]==_new.vertexIndex[0])
				return true;
		}
		return false;
	}
	bool convertTriangle2Quad(FACET *_tri1, FACET *_tri2)
	{
		if(_tri1->type!=GL_TRIANGLES||_tri2->type!=GL_TRIANGLES)
			return false;
		if(fabs(_tri1->normal[0]*_tri2->normal[0])<0.999)
			return false;

		Vector3d temp_vertex[4];
		int temp_vertexIndex[4];
		int temp_sharedIndex[2];
		Vector3d temp_sharedVertex[2];
		int count=0;
		FACET temp_facet;
		temp_facet=*_tri1;

		//共有されている2点を抽出する
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				if(_tri1->vertexIndex[i]==_tri2->vertexIndex[j]){
					temp_sharedIndex[count]=_tri1->vertexIndex[i];
					temp_sharedVertex[count]=_tri1->vertex[i];
					count++;
				}
			}
		}
		if(count!=2)return false;
		//共有されていない2点を抽出する
		count=0;
		for(int i=0;i<3;i++){
			count=0;
			for(int j=0;j<2;j++){
				if(_tri1->vertexIndex[i]!=temp_sharedIndex[j])
					count++;
				if(count==2){
					temp_vertexIndex[0]=_tri1->vertexIndex[i];
					temp_vertex[0]=_tri1->vertex[i];
				}
			}
		}
		count=0;
		for(int i=0;i<3;i++){
			count=0;
			for(int j=0;j<2;j++){
				if(_tri2->vertexIndex[i]!=temp_sharedIndex[j])
					count++;
				if(count==2){
					temp_vertexIndex[2]=_tri2->vertexIndex[i];
					temp_vertex[2]=_tri2->vertex[i];
				}
			}
		}
		//順番を考慮して一時変数に格納する
		temp_vertexIndex[1]=temp_sharedIndex[0];
		temp_vertexIndex[3]=temp_sharedIndex[1];
		temp_vertex[1]=temp_sharedVertex[0];
		temp_vertex[3]=temp_sharedVertex[1];

		//ポリゴン種類の変更
		_tri1->setFacetTypeAsPolygon();
		_tri2->setFacetTypeAsPolygon();	

		//値の継承
		_tri1->facetIndex=temp_facet.facetIndex;
		_tri1->normalType=temp_facet.normalType;
		_tri1->materialIndex=temp_facet.materialIndex;
		memcpy(_tri1->normal,temp_facet.normal,sizeof(Vector3d)*4);
		memcpy(_tri1->normalIndex,temp_facet.normalIndex,sizeof(int)*4);
		memcpy(_tri1->vertex,temp_vertex,sizeof(Vector3d)*4);
		memcpy(_tri1->vertexIndex,temp_vertexIndex,sizeof(int)*4);

		_tri2->facetIndex=temp_facet.facetIndex;
		_tri2->normalType=temp_facet.normalType;
		_tri2->materialIndex=temp_facet.materialIndex;
		memcpy(_tri2->normal,temp_facet.normal,sizeof(Vector3d)*4);
		memcpy(_tri2->normalIndex,temp_facet.normalIndex,sizeof(int)*4);
		_tri2->isEnabled=false;
		memcpy(_tri2->vertex,temp_vertex,sizeof(Vector3d)*4);
		memcpy(_tri2->vertexIndex,temp_vertexIndex,sizeof(int)*4);

		return true;
	}
}