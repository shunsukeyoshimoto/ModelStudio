#include "ms_glhandler.h"

namespace ModelStudio
{

	Window GLHandler::window;
	utilities GLHandler::dt;
	int GLHandler::key;
	Camera* GLHandler::camera;
	int GLHandler::cameraMode;
	int GLHandler::numCamera;
	Light* GLHandler::light;
	int GLHandler::numLight;
	MouseSelection GLHandler::select;
	Object* GLHandler::object;
	Vector3d GLHandler::posClicked;
	bool GLHandler::isRun;
	bool GLHandler::isViewGrid;

	GLHandler::GLHandler()
	{
		this->object=new Object;
		this->light=new Light[MAX_LIGHT_NUM];
		this->camera=new Camera[MAX_CAMERA_NUM];
		this->cameraMode=CAM_PERSP;
		this->numCamera=5;
		this->numLight=3;
		this->isRun=true;
		this->isViewGrid=true;
	}
	GLHandler::~GLHandler()
	{
		delete object;
		delete []camera;
		delete []light;
	}
	void GLHandler::init(int *_argc, char **_argv)
	{
		for(int i=0;i<numCamera;i++){
			camera[i].init(i);
		}
		for(int i=0;i<numLight;i++){
			light[i].init();
			light[i].setID(i);
		}
		camera[4].setTrans(GL_CAMERA_INIPOS);
		camera[5].init(CAM_FRONT);

		light[1].setPosition(Vector4f( 0, 1000.0, 0, 1.0 ));
		light[2].setPosition(Vector4f( 0, 1000.0, -1000.0, 1.0 ));
		glutInit(_argc,_argv);
	}
	void GLHandler::initGL()
	{
		glClearColor(0,0,0,1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_BUFFER_BIT);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
	}

	void GLHandler::startFrame()
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void GLHandler::displayGL()
	{
		if(!isRun)return;
		startFrame();

		camera[cameraMode].attach(window.size);

		glPushMatrix();	
		for(int i=0;i<numLight;i++){
			light[i].enable();
			light[i].set();
		}
		glPopMatrix();

		glPushMatrix();
		if(isViewGrid)
			glutGridGround(20);
		glPopMatrix();

		glPushMatrix();	
		object->render();
		glPopMatrix();

		camera[5].attach(window.size);
		glPushMatrix();
		select.render();
		glPopMatrix();

		camera[4].attach(window.size, false);
		glPushMatrix();
		glutAxsis();
		glPopMatrix();

		endFrame();
	}

	void GLHandler::endFrame()
	{
		dt.setEndTime();
		if(dt.getDeltaTime()>=1.0){
			window.fps/=dt.getDeltaTime();
			sprintf_s(window.title,"Frame buffer rate: %3.1f fps",window.fps);
			dt.setStartTime();
			window.fps=0;
		}
		else{
			window.fps++;
		}
		glutSwapBuffers();
	}

	double GLHandler::returnDepth(int _x, int _y){
		float depth;
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glReadPixels(_x,viewport[3]-_y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
		return depth;
	}
	Vector3d GLHandler::returnCameraCo(Vector3d _object){
		Vector3d V_camera;
		GLdouble mvMatrix[16],pjMatrix[16];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
		gluProject(_object.x,_object.y,_object.z,mvMatrix,pjMatrix,viewport,&V_camera.x,&V_camera.y,&V_camera.z);
		return V_camera;
	}
	Vector3d GLHandler::returnWorldCo(Vector3d V_camera){
		Vector3d V_world;
		GLdouble mvMatrix[16],pjMatrix[16];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
		gluUnProject(V_camera.x,(double)viewport[3]-V_camera.y,V_camera.z,mvMatrix,pjMatrix,viewport,&V_world.x,&V_world.y,&V_world.z);
		return V_world;
	}

	void GLHandler::mousePressEventGL(int _button, int _state, int _x, int _y)
	{
		Vector3d pos_world;
		key = glutGetModifiers();
		//クリックした点の3次元座標を取得
		camera[cameraMode].attach(window.size);
		posClicked=returnWorldCo(Vector3d(_x,_y,returnDepth(_x,_y)));


		if(_state==GLUT_DOWN){
			if(_button==GLUT_MIDDLE_BUTTON||cameraMode==0){
				camera[cameraMode].MouseInput(_button, Vector2f(_x,_y));
				camera[4].setAngle(camera[cameraMode].getAngle());
			}
		}
		else if(_state=GLUT_UP){
			camera[cameraMode].rmlUpdate();
			camera[4].rml[0].Update();
			camera[4].rml[2].Update();
		}
		static double sx, sy;
		if (_button == GLUT_RIGHT_BUTTON && _state == GLUT_UP) {
			select.setIsDrawSelectRegion(true);
			Vector3d selectedCoord[8];
			if (key == GLUT_ACTIVE_ALT) {
				pos_world = returnWorldCo(Vector3d(sx, sy, 0));
				selectedCoord[0] = pos_world;
				pos_world = returnWorldCo(Vector3d(sx, _y, 0));
				selectedCoord[1] = pos_world;
				pos_world = returnWorldCo(Vector3d(_x, _y, 0));
				selectedCoord[2] = pos_world;
				pos_world = returnWorldCo(Vector3d(_x, sy, 0));
				selectedCoord[3] = pos_world;
				pos_world = returnWorldCo(Vector3d(sx, sy, 1));
				selectedCoord[4] = pos_world;
				pos_world = returnWorldCo(Vector3d(sx, _y, 1));
				selectedCoord[5] = pos_world;
				pos_world = returnWorldCo(Vector3d(_x, _y, 1));
				selectedCoord[6] = pos_world;
				pos_world = returnWorldCo(Vector3d(_x, sy, 1));
				selectedCoord[7] = pos_world;
			}
			else if (key == GLUT_ACTIVE_CTRL || key == GLUT_ACTIVE_SHIFT) {
				posClicked = returnWorldCo(Vector3d(_x, _y, returnDepth(_x, _y)));
				selectedCoord[0] = posClicked;
			}
			object->mouse(selectedCoord, key);
		}
		if (_button == GLUT_RIGHT_BUTTON && _state == GLUT_DOWN) {
			select.setIsDrawSelectRegion(true);
			sx = _x;
			sy = _y;
			camera[5].attach(window.size);
			Vector3d pos_camera = Vector3d(_x, _y, returnCameraCo(Vector3d(0, 0, 0)).z);
			pos_world = returnWorldCo(pos_camera);
			select.setLeftTop(Vector2f(pos_world.x, pos_world.y));
			select.setRightBottom(Vector2f(pos_world.x, pos_world.y));
		}
		if (_state == GLUT_UP) {
			select.setIsDrawSelectRegion(false);
		}
	}

	void GLHandler::mouseMoveEventGL(int _x, int _y)
	{
		camera[cameraMode].attach(window.size);
		Vector3d pos_world = returnWorldCo(Vector3d(_x, _y, returnDepth(_x, _y)));

		if (cameraMode == 0) {
			camera[cameraMode].MouseMotion(Vector2f(_x, _y));
			camera[4].setAngle(camera[cameraMode].getAngle());
			camera[4].update();
		}

		camera[5].attach(window.size);
		pos_world = returnWorldCo(Vector3d(_x, _y, returnDepth(_x, _y)));
		select.setRightBottom(Vector2f(pos_world.x, pos_world.y));
	}
	void GLHandler::keyboardEventGL(unsigned char _key, int _x, int _y)
	{
		key = _key;
		if(key==' '){
			cameraMode=(cameraMode+1)%4;
			camera[4].setAngle(camera[cameraMode].getAngle());
			camera[4].update();
		}
	}
	void GLHandler::mouseWheelGL(int _wheel, int _direction, int _x, int _y)
	{
		camera[cameraMode].setZoom(camera[cameraMode].getZoom()+_direction);
		camera[cameraMode].MouseMotion(Vector2f(0,0), false, false, false);
	}
	void GLHandler::idle()
	{
		glutPostRedisplay();
	}
	void GLHandler::resize(int _width, int _height)
	{
		window.size.x=_width;
		window.size.y=_height;
	}
}
