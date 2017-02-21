#include "ms_glutilities.h"

namespace ModelStudio
{
	void Material::clear()
	{
		this->color = Vector4f(0, 0, 0, 1.0);
		this->ambient = Vector4f(0, 0, 0, 1.0);
		this->diffuse = Vector4f(0 , 0 , 0 , 1.0);
		this->specular = Vector4f(0 , 0 , 0, 1.0);
		this->emission = Vector4f(0 , 0 , 0, 1.0);
		this->shininess = 0;
		this->illum=4;
	}
	void Material::enable()
	{
		glEnable( GL_LIGHTING );
	}
	void Material::set()
	{
		glColor4fv(color.X);
		if(illum>0)glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.X);
		if(illum>1)glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.X);
		if(illum>2)glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.X);
		if(illum>3)glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
		if(illum>4)glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission.X);
	}
	void Material::disable()
	{
		glDisable( GL_LIGHTING );
		glColor3fv( ambient.X );
	}


	Light::Light()
	{

	}
	Light::~Light()
	{
	}
	void Light::init()
	{
		isOn=true;
		position = Vector4f( 0.0, 1000.0, 1000.0, 1.0 );
		ambient = Vector4f( 0.2, 0.2, 0.2, 1.0 );
		diffuse = Vector4f( 1, 1, 1, 1.0 );
		specular = Vector4f( 0.0, 0.0, 0.0, 1.0 );
	}
	void Light::set()
	{
		glLightfv( GL_LIGHT0+id, GL_POSITION, position.X);
		glLightfv( GL_LIGHT0+id, GL_AMBIENT, ambient.X);
		glLightfv( GL_LIGHT0+id, GL_DIFFUSE, diffuse.X);
		glLightfv( GL_LIGHT0+id, GL_SPECULAR, specular.X);
	}
	void Light::enable()
	{
		if(this->isOn)
			glEnable( GL_LIGHT0+id );
		else
			glDisable( GL_LIGHT0+id );
	}
	void Light::disable()
	{
			glDisable( GL_LIGHT0+id );
	}


	MouseButton::MouseButton()
	{
		Reset();
	}

	MouseButton::~MouseButton()
	{
	}

	void MouseButton::Reset()
	{
		before=Vector2f(0,0);
		current=Vector2f(0,0);
		after=Vector2f(0,0);
		state=None;
	}

	void MouseButton::Update()
	{
		state=Release;
		after=current;
	}

	MouseSelection::MouseSelection()
	{
		this->isDrawSelectRegion=false;
	}

	MouseSelection::~MouseSelection()
	{
	}


	void MouseSelection::render()
	{
		if(!this->isDrawSelectRegion)
			return;
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glColor3d(0.3, 0.3, 0.3);
		glPushMatrix();
		glBegin(GL_LINE_LOOP);
		glVertex3d(this->leftTop.x,this->leftTop.y,0);
		glVertex3d(this->leftTop.x,this->rightBottom.y,0);
		glVertex3d(this->rightBottom.x,this->rightBottom.y,0);
		glVertex3d(this->rightBottom.x,this->leftTop.y,0);
		glEnd();
		glPopMatrix();
		glDisable(GL_LINE_STIPPLE);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH);
	}

	Camera::Camera()
	{
		init();
	}

	Camera::~Camera()
	{
	}

	void Camera::init(int _cameraModel)
	{
		cameraModel=_cameraModel;
		rmlReset();
		if(cameraModel==CAM_PERSP){
			zoom = VC_INIT_ZOOM * 2;
			angle =Vector3d(M_PI/180*45,M_PI/180*45,0);
		}
		else if(cameraModel==CAM_FRONT){
			zoom = VC_INIT_ZOOM * 0.5;
			angle =Vector3d(0,0,0);
		}
		else if(cameraModel==CAM_SIDE){
			zoom = VC_INIT_ZOOM * 0.5;
			angle =Vector3d(0,M_PI/180*90,0);
		}
		else if(cameraModel==CAM_TOP){
			zoom = VC_INIT_ZOOM * 0.5;
			angle =Vector3d(M_PI/180*90,0,0);
		}else{
			zoom = VC_INIT_ZOOM * 0.5;
			angle =Vector3d(0,0,0);
		}
		position.x = sin(angle.x) * cos(angle.y) * zoom;
		position.y = sin(angle.y) * zoom;
		position.z = cos(angle.x) * cos(angle.y) * zoom;
		trans = Vector3d(0,0,0);
		target = Vector3d(0,0,0);
		upvector = Vector3d(0,1,0);
	}

	void Camera::MouseInput(int _button, Vector2f _position, bool ml, bool mm, bool mr)
	{
		if(ml&&_button==GLUT_LEFT_BUTTON){
			rml[2].before = _position;
			rml[2].state = Push;
		}
		else if(mm&&_button==GLUT_MIDDLE_BUTTON){
			rml[1].before = _position;
			rml[1].state = Push;
		}
//		else if(mr&&_button==GLUT_RIGHT_BUTTON){
//			rml[0].before = _position;
//			rml[0].state = Push;
//		}
	}

	void Camera::MouseMotion(Vector2f _position, bool ml, bool mm, bool mr)
	{

		if ( ml&&rml[0].state == Push ){
				rml[0].current = _position -rml[0].before + rml[0].after;
				zoom = VC_INIT_ZOOM+(rml[0].current.y+rml[0].current.x)*0.01;
		}
		if ( ml&&rml[1].state == Push ){
			rml[1].current= _position -rml[1].before + rml[1].after;
			trans.x = rml[1].current.x * 0.01;
			trans.y = -rml[1].current.y * 0.01;
		}
		if ( mr&& rml[2].state == Push ){
			angle.x+=M_PI/180*(rml[2].before.x - _position.x)*0.5;
			angle.y-=M_PI/180*(rml[2].before.y - _position.y)*0.5;
			if ( angle.y >= 2*M_PI )  angle.y -= 2*M_PI;
			else if ( angle.y < 0.0 )  angle.y += 2*M_PI;
			rml[2].before=_position;
		}
		update();
	}
	void Camera::rmlReset()
	{
		rml[0].Reset();
		rml[1].Reset();
		rml[2].Reset();
	}
	void Camera::rmlUpdate()
	{
		rml[0].Update();
		rml[1].Update();
		rml[2].Update();
	}
	void Camera::update()
	{
		position.x = sin(angle.x) * cos(angle.y) * zoom;
		position.y = sin(angle.y) * zoom;
		position.z = cos(angle.x) * cos(angle.y) * zoom;
		if( angle.y > Deg2Rad(90.0)  &&  angle.y <= Deg2Rad(270.0) ) upvector.y = -1.0;
		else upvector.y = 1.0;
	}
	void Camera::attach(Vector2d _size, bool _isScale)
	{
		if(cameraModel==CAM_PERSP){
			this->resizePersp(_size);
			glMatrixMode( GL_MODELVIEW );
			Ttran.setTranslate(Vector3d(trans.x, trans.y, trans.z));
			this->setModelViewMatrix(_isScale);
			glLoadMatrixd((Ttran*Trot).getTr4GL());
		}
		else{
			this->resizeOrtho(_size, _isScale);
			glMatrixMode( GL_MODELVIEW );
			Ttran.setTranslate(Vector3d(trans.x, trans.y, trans.z));
			this->setModelViewMatrix(_isScale);
			glLoadMatrixd((Ttran*Trot).getTr4GL());
		}
	}
	void Camera::attachRotMat(bool _isScale)
	{
		setModelViewMatrix(_isScale);
		glLoadMatrixd(this->Trot.getTr4GL());
	}
	void Camera::attachTransMat(bool _isFix)
	{
		if(!_isFix)
			Ttran.setTranslate(Vector3d(trans.x, trans.y, trans.z));
		glLoadMatrixd(this->Ttran.getTr4GL());
	}

	void Camera::setModelViewMatrix(bool _isScale)
	{
		Vector3d forward;
		Vector3d side;
		Vector3d up;

		forward=target-position;
		forward/=forward.abs();
		up=upvector;
		side=forward%up;
		side/=side.abs();
		up=side%forward;

		transferMatrixd tTrot;
		transferMatrixd tTtran;
		tTrot.setRotMatrix(side,up,-forward);
		if(_isScale)
			tTtran.setTranslate(-position);
		else
			tTtran.setTranslate(-VC_DEFAULT_ZOOM*position/position.abs());

		this->Trot=tTrot*tTtran;
	}

	void Camera::resizeOrtho(Vector2d _size, bool _isScale)
	{
		glViewport( 0, 0, _size.x, _size.y );
		glMatrixMode(GL_PROJECTION );
		glLoadMatrixd(this->Tproj.getTr4GL());
		if(_size.y!=0){
			if(_isScale)
				glOrtho(-zoom*_size.x/_size.y, zoom*_size.x/_size.y, -zoom , zoom, 0.1, 1000);
			else glOrtho(-VC_DEFAULT_ZOOM*_size.x/_size.y, VC_DEFAULT_ZOOM*_size.x/_size.y, -VC_DEFAULT_ZOOM , VC_DEFAULT_ZOOM, 0.1, 1000);
		}
	}
	void Camera::resizePersp(Vector2d _size)
	{
		glViewport( 0, 0, _size.x, _size.y );
		glMatrixMode(GL_PROJECTION );
		glLoadMatrixd(this->Tproj.getTr4GL());
		gluPerspective(50, _size.x/_size.y, 0.01, 1000 );
		glMatrixMode( GL_MODELVIEW );
	}

	void glutSolidRectangle(Vector3d _size, bool _isVertShade)
	{
		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d(0,0, 1).X);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1, 1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1, 1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1, 1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1, 1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0, _size.z/2.0);
		glEnd();

		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d(0,0,-1).X);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1,-1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1,-1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1,-1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1,-1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0,-_size.z/2.0);
		glEnd();

		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d(-1,0,0).X);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1, 1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1,-1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1,-1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1, 1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0, _size.z/2.0);
		glEnd();
		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d( 1,0,0).X);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1, 1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1, 1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1,-1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1,-1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0,-_size.z/2.0);
		glEnd();

		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d(0,-1,0).X);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1, 1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1, 1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1,-1,-1).X);
		glVertex3d(-_size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1,-1,-1).X);
		glVertex3d( _size.x/2.0,-_size.y/2.0,-_size.z/2.0);
		glEnd();
		glBegin(GL_POLYGON);
		glNormal3dv(Vector3d(0, 1,0).X);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1, 1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0, _size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d( 1, 1,-1).X);
		glVertex3d( _size.x/2.0, _size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1,-1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0,-_size.z/2.0);
		if(_isVertShade)glNormal3dv(Vector3d(-1, 1, 1).X);
		glVertex3d(-_size.x/2.0, _size.y/2.0, _size.z/2.0);
		glEnd();

	}

	void glutLineArrows(double _scale)
	{
		glDisable(GL_LIGHTING);
		glLineWidth(3);
		glColor3d(1,0,0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(0,0,0);
		glVertex3d(_scale,0,0);
		glEnd();
		glColor3d(0,1,0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(0,0,0);
		glVertex3d(0,_scale,0);
		glEnd();
		glColor3d(0,0,1);
		glBegin(GL_LINE_STRIP);
		glVertex3d(0,0,0);
		glVertex3d(0,0,_scale);
		glEnd();
		glLineWidth(1);
	}

	void glutAxsis()
	{
		char text[256];
		glDisable(GL_DEPTH);
		glDisable(GL_LIGHTING);
		glLineWidth(4);
		glColor3d(1,0,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(1,0,0);
		glEnd();
		sprintf_s(text,"X");
		glRasterPos3f(1,0,0);
		for(int j=0;text[j];j++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[j]);
		}
		glColor3d(0,1,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,+1,0);
		glEnd();
		sprintf_s(text,"Y");
		glRasterPos3f(0,1,0);
		for(int j=0;text[j];j++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[j]);
		}
		glColor3d(0,0,1);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,0,+1);
		glEnd();
		sprintf_s(text,"Z");
		glRasterPos3f(0,0,1);
		for(int j=0;text[j];j++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[j]);
		}

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH);

	}

	void glutAxsis(Vector3d _nx, Vector3d _ny, Vector3d _nz)
	{
		glLineWidth(1);
		glColor3d(1,0,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3dv(_nx.X);
		glEnd();

		glColor3d(0,1,0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3dv(_ny.X);
		glEnd();

		glColor3d(0,0,1);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3dv(_nz.X);
		glEnd();
	}

	void glutGridGround(double _size, int _numGrid)
	{
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor3d(0.0, 0.0, 0.0);
		for(int i=-_numGrid;i<=_numGrid;i++){
			if(i==0)glLineWidth(3);
			else 	glLineWidth(1);
			glBegin(GL_LINES);
			glVertex3d(-_size, 0, i*_size/_numGrid);
			glVertex3d( _size, 0, i*_size/_numGrid);
			glEnd();
			glBegin(GL_LINES);
			glVertex3d(i*_size/_numGrid, 0, -_size);
			glVertex3d(i*_size/_numGrid, 0, _size);
			glEnd();
		}
		glEnable(GL_LIGHTING);
	}
}