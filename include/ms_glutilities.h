#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <ms_vectormatrix.h>
#include <ms_transfermatrix.h>
#include <ms_colormap.h>

#define VC_INIT_ZOOM 20.0
#define VC_DEFAULT_ZOOM 20.0

namespace ModelStudio
{
	enum MState{
		Push,
		Release,
		None
	};
	struct Window{
		char title[256];
		double fps;
		Vector2d size;
	};

	class Material{
	private:
		char name[256];
		Vector4f color;
		Vector4f ambient;
		Vector4f diffuse;
		Vector4f specular;
		Vector4f emission;
		float shininess;
		int illum;
	public:
		Material(char _name[256] = "default"){
			strcpy(name, _name);
			color = Vector4f(0.0, 0.0, 0.0, 1.0);
			ambient = Vector4f(0.0, 0.0, 0.0, 1.0);
			diffuse = Vector4f(1.0, 1.0, 1.0, 1.0);
			specular = Vector4f(0.5, 0.5, 0.5, 1.0);
			emission = Vector4f(0.0, 0.0, 0.0, 1.0);
			shininess = 5.0f;
			illum=4;
		}
		~Material(){}

		Material &operator = (const Material &_mat){ 
			strcpy(name, _mat.name); 
			color=_mat.color; 
			ambient=_mat.ambient;
			diffuse=_mat.diffuse;
			specular=_mat.specular;
			emission=_mat.emission;
			shininess=_mat.shininess;
			illum=_mat.illum;
			return (*this); }
		void setName(const char *_name){strcpy(name,_name);}
		void setColor(Vector4f _color){color = _color;}
		void setAmbient(Vector4f _ambient){ambient = _ambient;}
		void setDiffuse(Vector4f _diffuse){diffuse = _diffuse;}
		void setSpecular(Vector4f _specular){specular = _specular;}
		void setEmission(Vector4f _emission){emission = _emission;}
		void setShininess(float _shininess){shininess = _shininess;}
		void setColor(Vector3f _color, float _alpha=1.0){for(int i=0;i<3;i++){color.X[i] = _color.X[i];}color.w=_alpha;}
		void setAmbient(Vector3f _ambient, float _alpha=1.0){for(int i=0;i<3;i++){ambient.X[i] = _ambient.X[i];}ambient.w=_alpha;}
		void setDiffuse(Vector3f _diffuse, float _alpha=1.0){for(int i=0;i<3;i++){diffuse.X[i] = _diffuse.X[i];}diffuse.w=_alpha;}
		void setSpecular(Vector3f _specular, float _alpha=1.0){for(int i=0;i<3;i++){specular.X[i] = _specular.X[i];}specular.w=_alpha;}
		void setEmission(Vector3f _emission, float _alpha=1.0){for(int i=0;i<3;i++)emission.X[i] = _emission.X[i];emission.w=_alpha;}
		void setColorA(float _a){color.w=_a;}
		void setAmbientA(float _a){ambient.w=_a;}
		void setDiffuseA(float _a){diffuse.w=_a;}
		void setEmissionA(float _a){emission.w=_a;}
		void setSpecularA(float _a){specular.w=_a;}
		void setIllum(int _illum){this->illum=_illum;}

		char* getName(){return name;}
		Vector4f getColor(){return color;}
		Vector4f getAmbient(){return ambient;}
		Vector4f getDiffuse(){return diffuse;}
		Vector4f getEmission(){return emission;}
		Vector4f getSpecular(){return specular;}
		float getShininess(){return shininess;}
		int getIllum(){return illum;}

		void clear();
		void enable();
		void set();
		void disable();
	};

	class Light{
	private:
		Vector4f position;
		Vector4f ambient;
		Vector4f diffuse;
		Vector4f specular;
		int id;
		bool isOn;
	public:
		Light();
		~Light();

		void init();
		void set();
		void enable();
		void disable();

		void setIsOnLight(bool _isOn){this->isOn=_isOn;}
		void setID(int _id){this->id=_id;}
		void setPosition(Vector4f _position){position = _position;}
		void setAmbient(Vector4f _ambient){ambient = _ambient;}
		void setDiffuse(Vector4f _diffuse){diffuse = _diffuse;}
		void setSpecular(Vector4f _specular){specular = _specular;}

		bool getIsOn(){return this->isOn;}
		int getID(){return this->id;}
		Vector4f getAmbient(){return this->ambient;}
		Vector4f getDiffuse(){return this->diffuse;}
		Vector4f getSpecular(){return this->specular;}
		Vector4f getPosition(){return this->position;}
	};

	class MouseButton
	{
	public:
		Vector2f before;
		Vector2f current;
		Vector2f after;
		MState state;
		MouseButton();
		~MouseButton();
		void Reset();
		void Update();
	};
	class MouseSelection
	{
	protected:
		bool isDrawSelectRegion;
		Vector2f leftTop;
		Vector2f rightBottom;
	public:
		MouseSelection();
		~MouseSelection();
		void setIsDrawSelectRegion(bool _isDrawSelectRegion){this->isDrawSelectRegion=_isDrawSelectRegion;}
		void setLeftTop(Vector2f _leftTop){this->leftTop=_leftTop;}
		void setRightBottom(Vector2f _rightBottom){this->rightBottom=_rightBottom;}
		void render();
		bool getIsDrawSelectRegion(){return this->isDrawSelectRegion;}
	};

	enum CAM_MODE{
		CAM_PERSP,
		CAM_FRONT,
		CAM_SIDE,
		CAM_TOP
	};
	class Camera
	{
	private:
		double zoom;
		Vector3d angle;
		Vector3d position;
		Vector3d trans;
		Vector3d target;
		Vector3d upvector;
		transferMatrixd Tproj;
		transferMatrixd Tmodel;
		transferMatrixd Trot;
		transferMatrixd Ttran;

		int cameraModel;

		void setModelViewMatrix(bool _isScale=true);

	public:

		Camera();
		~Camera();
		void init(int _cameraModel=CAM_PERSP);

		void setCameraModel(int _cameraModel=CAM_PERSP){this->cameraModel=_cameraModel;}
		void resizePersp(Vector2d _size);
		void resizeOrtho(Vector2d _size, bool _isScale=true);
		void attachTransMat(bool _isFix=false);
		void attachRotMat(bool _isScale=true);
		void attach(Vector2d _size, bool _isScale=true);

		MouseButton rml[3];
		void rmlReset();
		void rmlUpdate();
		void update();
		void MouseInput(int _button, Vector2f _position, bool ml=true, bool mm=true, bool mr=true);
		void MouseMotion(Vector2f _position, bool ml=true, bool mm=true, bool mr=true);

		void setZoom(double _zoom){zoom=_zoom;}
		void setAngle(Vector3d _angle){angle=_angle;}
		void setPosition(Vector3d _position){position=_position;}
		void setTrans(Vector3d _trans){trans=_trans;}
		void setTarget(Vector3d _target){target=_target;}
		void setUpvector(Vector3d _upvector){upvector=_upvector;}
		void setTrot(transferMatrixd _Trot){this->Trot=_Trot;}
		void setTtran(transferMatrixd _Ttran){this->Ttran=_Ttran;}
		double getZoom(){return zoom;}
		Vector3d getPosition(){return position;}
		Vector3d getTrans(){return trans;}
		Vector3d getTarget(){return target;}
		Vector3d getUpvector(){return upvector;}
		Vector3d getAngle(){return angle;}

		void setTproj(transferMatrixd _Tproj){Tproj=_Tproj;}
		void setTmodel(transferMatrixd _Tmodel){Tmodel=_Tmodel;}
	};
	void glutSolidRectangle(Vector3d _size, bool _isVertShade=false);
	void glutLineArrows(double _scale=10);
	void glutAxsis();
	void glutAxsis(Vector3d _nx, Vector3d _ny, Vector3d _nz);
	void glutGridGround(double _size, int _numGrid=10);
};