#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <math.h>
#include <ms_vectormatrix.h>
#include <ms_transfermatrix.h>
#include <ms_glutilities.h>
#include <ms_pointlineface.h>

#define NUM_VERTEX_MAX 6

namespace ModelStudio{

	enum SELECT_MESH{
		SELECT_NONE,
		SELECT_FACET,
		SELECT_LINE,
		SELECT_POINT
	};
	enum NORMAL_TYPE{
		NORMAL_NONE,
		NORMAL_FACET,
		NORMAL_POINT
	};

	class LINE
	{
	public:
		LINE(){isSelected=false;}
		~LINE(){;}
		bool isSelected;
		Vector3d vertex[2];
		int vertexIndex[2];
		LINE &operator = (const LINE &_line){
			memcpy(vertex,_line.vertex,sizeof(Vector3d)*2);
			memcpy(vertexIndex,_line.vertexIndex,sizeof(int)*2);
			isSelected=_line.isSelected;
			return (*this);
		}
		void render(){
			glBegin(GL_LINE_STRIP);
			glVertex3dv(this->vertex[0].X);	
			glVertex3dv(this->vertex[1].X);	
			glEnd();		
		}
		double getLength(){return (vertex[0]-vertex[1]).abs();}
	};

	class FACET
	{
	public:
		GLenum type;

		LINE line[NUM_VERTEX_MAX];
		int vertexIndex[NUM_VERTEX_MAX];
		int normalIndex[NUM_VERTEX_MAX];
		Vector3d vertex[NUM_VERTEX_MAX];
		Vector3d normal[NUM_VERTEX_MAX];

		bool isSelected;
		bool isEnabled;
		int facetIndex;
		int materialIndex;
		int numVertex;
		int numNormal;
		int numLine;
		int desIndex;
		double desAngle;

		Vector3d min;
		Vector3d max;

		int normalType;
		FACET &operator = (const FACET &_facet){
			type=_facet.type;
			this->isSelected=_facet.isSelected;
			this->isEnabled=_facet.isEnabled;
			facetIndex=_facet.facetIndex;
			materialIndex=_facet.materialIndex;
			numVertex=_facet.numVertex;
			numLine=_facet.numLine;
			numNormal=_facet.numNormal;
			normalType = _facet.normalType;
			desIndex=_facet.desIndex;
			desAngle=_facet.desAngle;
			memcpy(vertexIndex,_facet.vertexIndex,sizeof(int)*numVertex);
			memcpy(vertex,_facet.vertex,sizeof(Vector3d)*numVertex);
			memcpy(normalIndex,_facet.normalIndex,sizeof(int)*numNormal);
			memcpy(normal,_facet.normal,sizeof(Vector3d)*numNormal);
			memcpy(line,_facet.line,sizeof(LINE)*numLine);
			return (*this);
		}
		FACET(GLenum _type=GL_TRIANGLES, int _numVertex=0,	int _normalType=NORMAL_POINT) 
			: type(_type), numVertex(_numVertex), numNormal(_numVertex), normalType(_normalType),isSelected(false)
			,isEnabled(true),materialIndex(0),numLine(0),desIndex(0),desAngle(0){
		}
		~FACET(){
		}
		void setFacetTypeAsTriangle(){
			type=GL_TRIANGLES;
			numVertex=3;
			numLine=3;
			numNormal=3;
			materialIndex=0;
		}
		void setFacetTypeAsPolygon(int _num=4){
			if(_num<NUM_VERTEX_MAX){
				type=GL_POLYGON;
				numVertex=_num;
				numLine=_num;
				numNormal=_num;
				materialIndex=0;
			}
		}
		Vector3d getMinVertex(){
			min=Vector3d(INT_MAX,INT_MAX,INT_MAX);
			for(int i=0;i<numVertex;i++){
				if(min.x>vertex[i].x)min.x=vertex[i].x;
				if(min.y>vertex[i].y)min.y=vertex[i].y;
				if(min.z>vertex[i].z)min.z=vertex[i].z;
			}
			return min;
		}
		Vector3d getMaxVertex(){
			max=Vector3d(-INT_MAX,-INT_MAX,-INT_MAX);
			for(int i=0;i<numVertex;i++){
				if(max.x<vertex[i].x)max.x=vertex[i].x;
				if(max.y<vertex[i].y)max.y=vertex[i].y;
				if(max.z<vertex[i].z)max.z=vertex[i].z;
			}
			return max;
		}
		bool getIsSelected(){return this->isSelected;}
		void setIsSelected(bool _isSelected){isSelected=_isSelected;}
	};

	class surfMesh
	{
	protected:
		int numVertex;
		int numNormal;
		int numFacet;
		int numLine;
		int numMaterial;
		FACET *facet;
		LINE *line;
		Material *material;
		Material sMat;
		Material cMat;
		Vector3d *normal;
		Vector3d *vertex;
		Vector3f *color;
		int *labelIndex;
		transferMatrixd T_human2traj;
		bool *isSelected;
		Vector3d center;
		Vector3d size;
		Vector3d selectedVertex;
		int selectedIndex;
		double scale;

		GLuint id;
		bool isUseDisplayList;
		bool isListed;
		bool isLoaded;
		bool isView;
		bool isLine;
		bool isPoint;
		bool isFacet;
		bool isAffine;
		bool isTri;
		bool isIdentity;
		bool isSelect;
		bool isViewLabel;
		bool isAlphaBlend;
		bool isLineCalculated;
		bool isVertexColorEnabled;

		void calFacetNormal();
		void calFacetVertex();
		void calLine();
		void calVertex();
		void calCenter();
		void calScale();
		void calTransform();

		void addVertex(Vector3d &_vertex);
		void addNormal(Vector3d &_normal);
		void addFacet(FACET &_facet);
		void addMaterial(Material &_material);
	public:
		surfMesh(void);
		~surfMesh(void);

		localObject Tr;

		bool load(const char *filename);
		bool save(const char *filename);

		void newMesh();
		void newMaterial();
		void deleteMesh();
		void deleteMaterial();
		void clearMesh();
		void clearView();

		bool getIsTri(){return this->isTri;}
		bool getIsLoaded(){return this->isLoaded;}
		bool getIsView(){return this->isView;}
		bool getIsAffine(){return this->isAffine;}
		bool getIsIdentity(){return this->isIdentity;}
		bool getIsSelect(){return this->isSelect;}
		bool getIsPoint(){return this->isPoint;}
		bool getIsLine(){return this->isLine;}
		bool getIsFacet(){return this->isFacet;}
		bool getIsUseDisplayList(){return this->isUseDisplayList;}
		bool getIsVertexColorEnabled(){return this->isVertexColorEnabled;}
		bool getIsViewLabel(){return this->isViewLabel;}

		int getNumVertex() { return numVertex; }
		int getNumNormal() { return numNormal; }
		int getNumFacet() { return numFacet; }
		int getNumMaterial() { return numMaterial; }
		int getNumLine() {return numLine;}

		Vector3d getSize(){return size;}
		double getScale(){return scale;}
		Vector3d getCenter(){return center;}
		Vector3d getVertex(int _index){return vertex[_index];}
		Vector3d* getVertexPointer(){return vertex;}
		Vector3d getNormal(int _index){return normal[_index];}
		LINE getLine(int _index){return this->line[_index];}
		Vector3f getColor(int _index){return color[_index];}
		void setVertex(int _index, Vector3d _vertex){vertex[_index]=_vertex;}
		FACET getFacet(int _index){return facet[_index];}
		FACET* getFacetPointer(int _index){return &facet[_index];}
		void setFacet(int _index, FACET _facet){facet[_index]=_facet;}
		void setNormal(int _index, Vector3d _normal){normal[_index]=_normal;}
		void setLine(int _index, LINE _line){line[_index]=_line;}
		Material getMaterial(int _index){return this->material[_index];}
		Material* getMaterialPointerAt(int _index){return &this->material[_index];}
		int getLabelIndex(int _index){return this->labelIndex[_index];}
		transferMatrixd getThuman2traj(){return this->T_human2traj;}

		std::ostream& getInfo(std::ostream &stream);
		void render();
		void renderList();
		GLuint makeDisplayList(int);

		void setColorAt(int _index, Vector3f _color){this->color[_index]=_color;}
		void setLabelIndexAt(int _index, int _label){this->labelIndex[_index] = _label;}
		void setNumVertex(int _numVertex){this->numVertex=_numVertex;}
		void setNumNormal(int _numNormal){this->numNormal=_numNormal;}
		void setNumFacet(int _numFacet){this->numFacet=_numFacet;}
		void setNumMaterial(int _numMaterial){this->numMaterial=_numMaterial;}
		void setNumLine(int _numLine){this->numLine=_numLine;}
		void setIsView(bool _isView){this->isView=_isView;}
		void setIsViewLabel(bool _isViewLabel){this->isViewLabel=_isViewLabel;}
		void setIsLine(bool _isLine){this->isLine=_isLine;}
		void setIsFacet(bool _isFacet){this->isFacet=_isFacet;}
		void setIsPoint(bool _isPoint){this->isPoint=_isPoint;}
		void setIsAffine(bool _isAffine){this->isAffine=_isAffine;}
		void setIsTri(bool _isTri){this->isTri=_isTri;}
		void setIsIdentity(bool _isIdentity){this->isIdentity=_isIdentity;}
		void setIsSelect(bool _isSelect){this->isSelect=_isSelect;}
		void setIsAlphaBlend(bool _isAlphaBlend){this->isAlphaBlend=_isAlphaBlend;}
		void setIsVertexColorEnabled(bool _isVertexColorEnabled){this->isVertexColorEnabled=_isVertexColorEnabled;}
		void setIsUseDisplayList(bool _isUseDisplayList){this->isUseDisplayList=_isUseDisplayList;}

		//•`‰æÀ•W‚ÉÅ‚à‹ß‚¢’¸“_”Ô†‚ðŽæ“¾‚·‚é
		int getVertexIndexNear(Vector3d _pos);
		int getFacetIndexNear(Vector3d _pos);
		Vector3d getSelectedVertex(){return this->selectedVertex;}
		void setSelectedVertex(int _index){this->selectedVertex=this->vertex[_index];}
		void setSelectedIndex(int _selectedIndex){this->selectedIndex=_selectedIndex;}
		void clearSelection();

		void quadrization();
	};

	bool isSharedLine(LINE _new, LINE *_old, int _num);
	bool convertTriangle2Quad(FACET *_tri1, FACET *_tri2);
	bool isCrossFacet(FACET *_facet1, FACET *_facet2, Vector3d *_Xend);
}