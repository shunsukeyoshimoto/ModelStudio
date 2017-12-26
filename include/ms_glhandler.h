#pragma once
#define MAX_LIGHT_NUM 9
#define MAX_CAMERA_NUM 9
#define GL_REAL_VIRTUAL_SCALE 0.1
#define GL_VIRTUAL_REAL_SCALE 10.0
#define GL_CAMERA_INIPOS Vector3d(35,15,0)

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <ms_utilities.h>
#include <ms_glutilities.h>
#include <ms_transfermatrix.h>
#include <ms_glutilities.h>

namespace ModelStudio{

	class Object{
	public:
		Object(){}
		~Object(){}
		void (*render)();
		void (*mouse)(Vector3d *_coord, int _mode);
		void setObject(void (*_render)()){render=_render;}
		void setMouse(void(*_mouse)(Vector3d *_coord, int _mode)) { mouse = _mouse; }
	};

	class GLHandler
	{
	private:
		static Window window;

		static MouseSelection select;
		static Vector3d posClicked;
		static int key;

		static int cameraMode;
		static Camera *camera;
		static int numCamera;

		static Light *light;
		static int numLight;

		static utilities dt; 
		static bool isRun;
		static bool isViewGrid;

		static double returnDepth(int,int);
		static Vector3d returnCameraCo(Vector3d);
		static Vector3d returnWorldCo(Vector3d);

		static void startFrame();
		static void endFrame();

		static Object *object;
	public:
		GLHandler();
		~GLHandler(void);

		void init(int *_argc, char **_argv);

		void setIsRun(bool _isRun){this->isRun=_isRun;}
		bool getIsRun(){return isRun;}
		void setIsViewGrid(bool _isViewGrid){this->isViewGrid=_isViewGrid;}
		bool getIsViewGrid(){return this->isViewGrid;}

		void setKey(int _key){this->key=_key;}
		int getKey(){return key;}
		Vector3d getPosClicked(){return posClicked;}

		void setNumLight(int _numLight){this->numLight=_numLight;}
		int getNumLight(){return numLight;}
		Light* getLightPointer(){return light;}
		Light* getLightPointerAt(int _index){return &light[_index];}

		void setCameraMode(int _cameraMode){cameraMode=_cameraMode;}
		void setNumCamera(int _numCamera){this->numCamera=_numCamera;}
		int getNumCamera(){return numCamera;}
		Camera* getCameraPointer(){return camera;}
		Camera* getCameraPointerAt(int _index){return &camera[_index];}

		void setWindowSize(Vector2d _size){window.size=_size;}
		Vector2d getWindowSize(){return window.size;}

		void initGL();
		static void displayGL();
		static void mousePressEventGL(int _button, int _state, int _x, int _y);
		static void mouseMoveEventGL(int _x, int _y);
		static void keyboardEventGL(unsigned char _key, int _x, int _y);
		static void idle();
		static void resize(int _width, int _height);
		static void mouseWheelGL(int _wheel, int _direction, int _x, int _y);

		Object* getObject(){return object;}

	};
};
