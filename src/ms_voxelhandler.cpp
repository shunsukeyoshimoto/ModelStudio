#include "ms_voxelHandler.h"

namespace ModelStudio
{
	VOXMODEL::VOXMODEL(void):width(2),height(2),depth(2),scale(1),size(1,1,1),isDataLoaded(false),isViewData(true)
	{
		this->isMilling=false;
		this->isAffine=true;
		this->isViewSlice=false;
		this->curStimMap=0;
		this->freeLabel=0;
		this->numVoxel=this->width*this->height*this->depth;
		this->length=1;
		this->newVoxel();
		this->slice=new IMG[3];
		this->setSliceSize(2);
		this->pixels=new unsigned char[3*2000*2000];//ひとまず最大サイズは2000*2000まで
		this->buffer=new unsigned char[3*2000*2000];//ひとまず最大サイズは2000*2000まで
	}
	VOXMODEL::~VOXMODEL(void)
	{
		delete []pixels;
		delete []buffer;
		this->deleteVoxel();
	}
	void VOXMODEL::initTexture()
	{
		glGenTextures(3,&texName[0]);
		//for(int i=0;i<3;i++){
		//	glBindTexture(GL_TEXTURE_2D,texName[i]);
		//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		//}
	}
	void VOXMODEL::setVoxelNum(int _width, int _height, int _depth)
	{
		this->width=_width;
		this->height=_height;
		this->depth=_depth;
		this->numVoxel=this->width*this->height*this->depth;
		this->setSliceSize(width);
	}
	void VOXMODEL::setVoxelSameNum(int _sizeVoxel)
	{
		this->width=_sizeVoxel;
		this->height=_sizeVoxel;
		this->depth=_sizeVoxel;
		this->numVoxel=this->width*this->height*this->depth;
		this->setSliceSize(width);
	}
	void VOXMODEL::deleteVoxel()
	{
		this->clear();
		for(int x=0;x<width;x++){
			for(int y=0;y<height;y++){
				delete []this->data[x][y];
			}
			delete []this->data[x];
		}
		delete []this->data;
	}
	void VOXMODEL::newVoxel()
	{
		this->data = new Voxel**[this->width];
		for(int x=0;x<width;x++){
			this->data[x] = new Voxel*[this->height];
			for(int y=0;y<height;y++){
				this->data[x][y] = new Voxel[this->depth];
			}
		}
	}
	void VOXMODEL::clear()
	{
		this->isDataLoaded=false;
		this->width=0;
		this->height=0;
		this->depth=0;
		this->numVoxel=0;
	}
	void VOXMODEL::calModel2Voxel()
	{
		transferMatrixd Tscale;
		transferMatrixd Tobj;
		transferMatrixd Tvox;
		this->setSize(this->scale/width);
		Tscale.setScale(size.x);
		Tobj.setTranslate(center);
		Tvox.setTranslate(Vector3d(-width/2.0, -height/2.0, -depth/2.0));
		this->Tr.setVoxel2Model(Tobj*Tscale*Tvox);
	}
	void VOXMODEL::calAutoAffine()
	{
		transferMatrixd Tsuni;
		transferMatrixd Ttuni;
		Tsuni.setScale(10*GAIN_MARGIN/this->scale);
		Ttuni.setTranslate(Vector3d(-this->center.x, -this->center.y, -this->center.z));
		Tr.setModel2Uni(Tsuni*Ttuni);
	}
	void VOXMODEL::calVoxelCenter()
	{
		for(int z=0;z<this->depth;z++)
			for(int y=0;y<this->height;y++)
				for(int x=0;x<this->width;x++)
					this->data[x][y][z].center=Tr.getVoxel2Model()*Vector3d(x+0.5, y+0.5, z+0.5);
	}
	void VOXMODEL::calCoordinateSetting()
	{
		this->calModel2Voxel();
		this->calVoxelCenter();
		this->calAutoAffine();
		this->calNeiborVoxel();
	}
	int VOXMODEL::getIndexOfVoxelAt(Vector3d _Puni, int _coordinate)
	{
		Vector3d Pvoxel;
		if(_coordinate==COORD_WORLD){
			if(isAffine)
				Pvoxel=this->Tr.getModel2Voxel()*this->Tr.getModel2World()*this->Tr.getUni2Model()*_Puni;
			else Pvoxel=this->Tr.getModel2Voxel()*this->Tr.getModel2World()*_Puni;
		}
		else if(_coordinate==COORD_LOCAL){
			Pvoxel=this->Tr.getModel2Voxel()*_Puni;
		}
		Vector3d test=this->Tr.getModel2World()*_Puni;
		if(0<=Pvoxel.x&&Pvoxel.x<this->width
			&&0<=Pvoxel.y&&Pvoxel.y<this->height
			&&0<=Pvoxel.z&&Pvoxel.z<this->depth)
		{
			int i,j,k;
			i=(int)Pvoxel.x;
			j=(int)Pvoxel.y;
			k=(int)Pvoxel.z;
			return this->width*(this->height*k+j)+i;
		}
		return -1;
	}
	void VOXMODEL::render(bool _isScaled)
	{
		if(!this->isDataLoaded||!this->isViewData)
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glEnable(GL_ALPHA_TEST);
	//	glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glColor4f(0, 0, 1, 0.1);

		glPushMatrix();
		if(this->isAffine)
			glMultMatrixd(Tr.getModel2Uni().getTr4GL());
		glMultMatrixd(Tr.getWorld2Model().getTr4GL());

		for(int x=0;x<width;x++){
			for(int y=0;y<height;y++){
				for(int z=0;z<depth;z++){
					//アルファブレンドはZデプスソートを行なう必要がある
					if(this->data[x][y][z].color.abs()!=0&&this->data[x][y][z].color!=INVIS_COL){
						mat.disable();
						glColor4f(this->data[x][y][z].color.x, this->data[x][y][z].color.y, this->data[x][y][z].color.z, 0.1);
						glPushMatrix();
						glTranslatef(this->data[x][y][z].center.x,this->data[x][y][z].center.y,this->data[x][y][z].center.z);
						glutSolidRectangle(this->size);
						glPopMatrix();
					}
				}
			}
		}
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_CULL_FACE);
	}
	void VOXMODEL::renderGuid(){
		if(this->isDrawWireFrame){
			glLineWidth(1);
			glDisable(GL_LIGHTING);
			glColor4f(0, 0, 0, 1);
			glPushMatrix();
			glScaled(10.0*GAIN_MARGIN/(this->size.x*width),10.0*GAIN_MARGIN/(this->size.y*width),10.0*GAIN_MARGIN/(this->size.z*width));
			glutWireCube(this->size.x*width*length);
			glPopMatrix();
			glEnable(GL_LIGHTING);
		}
	}
	bool VOXMODEL::load(const char* _filename)
	{
		std::cout <<"Loading .vox data ...";
		std::ifstream file;
		int *pi;
		double *pd;
		int count=0;
		char buf[1024];
		file.open(_filename, std::ios::in||std::ios::binary);
		if ( !file.is_open() ){
			std::cout << "[FAIL]" << std::endl;
			return false;
		}
		this->deleteVoxel();
		file.read(buf, 80);
		pi=(int*)&buf[44];
		pd=(double*)&buf[56];
		this->setVoxelNum(*(pi+0),*(pi+1),*(pi+2));
		this->setSize(Vector3d(*(pd+0),*(pd+1),*(pd+2)));
		this->newVoxel();
		for(int x=0;x<width;x++){
			for(int y=0;y<height;y++){
				for(int z=0;z<depth;z++){
					file.read(buf, 24);
					pd=(double*)&buf[0];
					this->data[x][y][z].color=Vector3d(*(pd+0),*(pd+1),*(pd+2));
				}
			}
		}
		file.close();
		this->calCoordinateSetting();
		this->isDataLoaded=true;
		std::cout << "[OK]" << std::endl;
		return true;
	}
	bool VOXMODEL::save(const char* _filename)
	{
		std::cout <<"Saving .vox data ...";
		std::ofstream file;
		char buf[256];
		file.open(_filename, std::ios::out|std::ios::binary);
		if (!file.is_open()){
			std::cout << "[FAIL]" << std::endl;
			return false;
		}
		file.clear();
		for(int i=0;i<80;i++)buf[i]=NULL;
		sprintf_s(buf,"Object %s was written by binary code\n",_filename);
		file.write(buf,44);
		file.write(( char * ) &this->width, 4);
		file.write(( char * ) &this->height,4);
		file.write(( char * ) &this->depth, 4);
		file.write(( char * ) &this->size.x,8);
		file.write(( char * ) &this->size.y,8);
		file.write(( char * ) &this->size.z,8);
		for(int x=0;x<width;x++){
			for(int y=0;y<height;y++){
				for(int z=0;z<depth;z++){
					file.write(( char * ) &this->data[x][y][z].color.x,8);
					file.write(( char * ) &this->data[x][y][z].color.y,8);
					file.write(( char * ) &this->data[x][y][z].color.z,8);
				}
			}
		}
		file.close();
		std::cout << "[OK]" << std::endl;
		return true;
	}

	Vector3d VOXMODEL::getColorAt(int _width, int _height ,int _depth)
	{
		return data[_width][_height][_depth].color;
	}
	Vector3d VOXMODEL::getColorAt(int _index)
	{
		int x,y,z;
		z=_index%depth;
		y=(int)(_index/depth)%height;
		x=(int)(_index/depth)/height;
		return data[x][y][z].color;
	}
	int VOXMODEL::getLabelIndexAt(int _index)
	{
		int x,y,z;
		z=_index%depth;
		y=(int)(_index/depth)%height;
		x=(int)(_index/depth)/height;
		return data[x][y][z].labelindex;
	}
	//画像のサイズを変更する（voxelizeの際に実行）
	void VOXMODEL::setSliceSize(int _size)
	{
		this->slice[0].setSize(_size,_size);
		this->slice[1].setSize(_size,_size);
		this->slice[2].setSize(_size,_size);
	}
	//スライダを動かすたびに呼び出す
	void VOXMODEL::setSlice(bool _invert)
	{
		if(!this->isDataLoaded)return;
		for(int i=0;i<this->width;i++){
			for(int j=0;j<this->width;j++){
				if(!_invert){
					this->slice[0].setColorAt(i, j, Vector3d(this->getColorAt(this->slicePos.x,i,j)*255.0).X);
					this->slice[1].setColorAt(i, j, Vector3d(this->getColorAt(i,this->slicePos.y,j)*255.0).X);
					this->slice[2].setColorAt(i, j, Vector3d(this->getColorAt(i,j,this->slicePos.z)*255.0).X);
				}
				else{
					Vector3d t_color;
					t_color.x = Vector3d(this->getColorAt(this->slicePos.x,i,j)*255.0).z;
					t_color.y = Vector3d(this->getColorAt(this->slicePos.x,i,j)*255.0).y;
					t_color.z = Vector3d(this->getColorAt(this->slicePos.x,i,j)*255.0).x;
					this->slice[0].setColorAt(i, j, t_color.X);
					t_color.x = Vector3d(this->getColorAt(i,this->slicePos.y,j)*255.0).z;
					t_color.y = Vector3d(this->getColorAt(i,this->slicePos.y,j)*255.0).y;
					t_color.z = Vector3d(this->getColorAt(i,this->slicePos.y,j)*255.0).x;
					this->slice[1].setColorAt(i, j, t_color.X);
					t_color.x = Vector3d(this->getColorAt(i,j,this->slicePos.z)*255.0).z;
					t_color.y = Vector3d(this->getColorAt(i,j,this->slicePos.z)*255.0).y;
					t_color.z = Vector3d(this->getColorAt(i,j,this->slicePos.z)*255.0).x;
					this->slice[2].setColorAt(i, j, t_color.X);
				}
			}
		}
	}
	void VOXMODEL::renderGrid()
	{
		glPushMatrix();
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glColor3d(0,0,0);
		glPushMatrix();
		glScaled(10.0*GAIN_MARGIN/(this->width),10.0*GAIN_MARGIN/(this->height),10.0*GAIN_MARGIN/(this->depth));
		for(int i=-this->depth/2;i<=this->depth/2;i+=8){
			for(int j=-this->height/2;j<=this->height/2;j+=8){
				for(int k=-this->width/2;k<=this->width/2;k+=8){
					glBegin(GL_LINES);
					glVertex3d(depth/2,j,k);
					glVertex3d(-depth/2,j,k);
					glEnd();
					glBegin(GL_LINES);
					glVertex3d(i,height/2,k);
					glVertex3d(i,-height/2,k);
					glEnd();
					glBegin(GL_LINES);
					glVertex3d(i,j,width/2);
					glVertex3d(i,j,-width/2);
					glEnd();
				}
			}
		}
		glPopMatrix();
	}
	void VOXMODEL::renderSlicer()
	{
		if(!this->isDataLoaded)
			return;

		glDisable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		glPushMatrix();
		glScaled(10.0*GAIN_MARGIN/(this->size.x*width),10.0*GAIN_MARGIN/(this->size.y*width),10.0*GAIN_MARGIN/(this->size.z*width));

		//ここにフラグ処理を入れたい
		glLineWidth(2);
		glDisable(GL_TEXTURE_2D);
		glColor3d(1,0,0);
		glBegin(GL_LINE_LOOP);
		glVertex3d(this->size.x*this->length*(slicePos.x-width/2), -this->size.x*width*length/2, -this->size.x*width*length/2);
		glVertex3d(this->size.x*this->length*(slicePos.x-width/2),  this->size.x*width*length/2, -this->size.x*width*length/2);
		glVertex3d(this->size.x*this->length*(slicePos.x-width/2),  this->size.x*width*length/2,  this->size.x*width*length/2);
		glVertex3d(this->size.x*this->length*(slicePos.x-width/2), -this->size.x*width*length/2,  this->size.x*width*length/2);
		glEnd();
		//glBegin(GL_LINES);
		//glVertex3d(this->size.x*this->length*(slicePos.x-width/2+1), this->size.x*this->length*(slicePos.y-width/2+1), -this->size.x*width*length/2);
		//glVertex3d(this->size.x*this->length*(slicePos.x-width/2+1), this->size.x*this->length*(slicePos.y-width/2+1),  this->size.x*width*length/2);
		//glEnd();

		glColor3d(0,1,0);
		glBegin(GL_LINE_LOOP);
		glVertex3d(-this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),  -this->size.x*width*length/2);
		glVertex3d( this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),  -this->size.x*width*length/2);
		glVertex3d( this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),   this->size.x*width*length/2);
		glVertex3d(-this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),   this->size.x*width*length/2);
		glEnd();
		//glBegin(GL_LINES);
		//glVertex3d(-this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2+1),  this->size.x*this->length*(slicePos.z-width/2+1));
		//glVertex3d( this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2+1),  this->size.x*this->length*(slicePos.z-width/2+1));
		//glEnd();

		glColor3d(0,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex3d(-this->size.x*width*length/2, -this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glVertex3d( this->size.x*width*length/2, -this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glVertex3d( this->size.x*width*length/2,  this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glVertex3d(-this->size.x*width*length/2,  this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glEnd();
		//glBegin(GL_LINES);
		//glVertex3d(this->size.x*this->length*(slicePos.x-width/2+1), -this->size.x*width*length/2,  this->size.x*this->length*(slicePos.z-width/2+1));
		//glVertex3d(this->size.x*this->length*(slicePos.x-width/2+1),  this->size.x*width*length/2,  this->size.x*this->length*(slicePos.z-width/2+1));
		//glEnd();

		glColor3d(1,1,1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D , texName[0]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,this->slice[0].getpData());
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3d(this->size.x*this->length*(slicePos.x-width/2), -this->size.x*width*length/2, -this->size.x*width*length/2);
		glTexCoord2f(1.0, 0.0);glVertex3d(this->size.x*this->length*(slicePos.x-width/2),  this->size.x*width*length/2, -this->size.x*width*length/2);
		glTexCoord2f(1.0, 1.0);glVertex3d(this->size.x*this->length*(slicePos.x-width/2),  this->size.x*width*length/2,  this->size.x*width*length/2);
		glTexCoord2f(0.0, 1.0);glVertex3d(this->size.x*this->length*(slicePos.x-width/2), -this->size.x*width*length/2,  this->size.x*width*length/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D , 0);

		glBindTexture(GL_TEXTURE_2D , texName[1]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,this->slice[1].getpData());
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3d(-this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),  -this->size.x*width*length/2);
		glTexCoord2f(1.0, 0.0);glVertex3d( this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),  -this->size.x*width*length/2);
		glTexCoord2f(1.0, 1.0);glVertex3d( this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),   this->size.x*width*length/2);
		glTexCoord2f(0.0, 1.0);glVertex3d(-this->size.x*width*length/2, this->size.x*this->length*(slicePos.y-width/2),   this->size.x*width*length/2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D , 0);

		glBindTexture(GL_TEXTURE_2D , texName[2]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,this->slice[2].getpData());
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3d(-this->size.x*width*length/2, -this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glTexCoord2f(1.0, 0.0);glVertex3d( this->size.x*width*length/2, -this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glTexCoord2f(1.0, 1.0);glVertex3d( this->size.x*width*length/2,  this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glTexCoord2f(0.0, 1.0);glVertex3d(-this->size.x*width*length/2,  this->size.x*width*length/2, this->size.x*this->length*(slicePos.z-width/2));
		glEnd();
		glBindTexture(GL_TEXTURE_2D , 0);


		glPopMatrix();

	}
	//ボクセルのクラスにrenderImagesを導入する
	void VOXMODEL::renderImages(int _width, int _height)
	{

		//ここは後で変換行列に変える
		Vector2d scale;
		int sx=_width-_width%4;
		int sy=_height-_height%4;
		scale=Vector2d((double)(2.0*width)/sx,(double)(2.0*width)/sy);

		if(this->isDataLoaded){
			this->setIsViewSlice(true);
	//		render();
			this->renderSlicer();
			glReadPixels(0,0,sx,sy,GL_RGB,GL_UNSIGNED_BYTE,buffer);
			this->setIsViewSlice(false);
		}

		glClearColor(0.8, 0.8, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(this->isDataLoaded){
			for(int i=0;i<sx/2.0;i++){
				for(int j=0;j<sy/2.0;j++){
					for(int k=0;k<3;k++){
						pixels[3*(sx*(j+sy/2)+i+ 0 )+k]=this->slice[0].getColorAt(scale.x*i, scale.y*j, k);
						pixels[3*(sx*(j+sy/2)+i+sx/2)+k]=this->slice[1].getColorAt(scale.x*i, scale.y*j, k);
						pixels[3*(sx*(j+ 0 )+i+ 0 )+k]=this->slice[2].getColorAt(scale.x*i, scale.y*j, k);
						pixels[3*(sx*(j+ 0 )+i+sx/2)+k]=this->buffer[3*(sx*j*2+i*2)+k];
					}
				}
			}
		}
		for(int i=0;i<sx;i++){
			for(int j=0;j<sy;j++){
				if((sx/2-2<i&&i<sx/2+2)||(sy/2-2<j&&j<sy/2+2)){
					for(int k=0;k<3;k++){
						pixels[3*(sx*j+i)+k]=150;
					}
				}
			}
		}
		glDrawPixels(sx, sy, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		//voxのスライスxyz面を表示する
		//スライス面はuiのスライダhs_sxyzから取得してセットしておく
		//描画サイズはそれぞれwidth/2,height/2->widthとheightを引数にする
		//それぞれの画像座標系に対するウィンドウ座標系の変換行列を算出するのが最初
		//投影の大きさは高さと幅のうち小さい方に合わせる
		//表示はx,y,z,persの順
		//persはオフスクリーンレンダリングを実装
	}
	void VOXMODEL::closingSlice(int _times)
	{
		/*
		for(int i=0;i<_times;i++){
			this->slice[0].getDilationErosion(Vector3d(0,0,255));
			this->slice[1].getDilationErosion(Vector3d(0,0,255));
			this->slice[2].getDilationErosion(Vector3d(0,0,255));
		}
		for(int i=0;i<_times*2;i++){
			this->slice[0].getDilationErosion(Vector3d(0,0,0));
			this->slice[1].getDilationErosion(Vector3d(0,0,0));
			this->slice[2].getDilationErosion(Vector3d(0,0,0));
		}
		for(int i=0;i<_times;i++){
			this->slice[0].getDilationErosion(Vector3d(0,0,255));
			this->slice[1].getDilationErosion(Vector3d(0,0,255));
			this->slice[2].getDilationErosion(Vector3d(0,0,255));
		}
		*/
	}
	bool VOXMODEL::saveSlice(const char *_filename, int _axis)
	{
		std::cout <<"Saving .bmp data ...";
		Vector3d t_pos=this->slicePos;
		char t_filename[256];
		bool ret=true;
		for(int d=0;d<depth;d++){
			if(_axis==X_AXIS)
				this->setSlicePos(Vector3d(d,0,0));
			if(_axis==Y_AXIS)
				this->setSlicePos(Vector3d(0,d,0));
			if(_axis==Z_AXIS)
				this->setSlicePos(Vector3d(0,0,d));
			this->setSlice(true);
			sprintf(t_filename, "%s%03d.bmp", _filename, d);
			ret=ret*this->slice[_axis].save(t_filename);
		}
		this->setSlicePos(t_pos);
		this->setSlice();
		if(ret) std::cout <<"[OK]"<< std::endl;
		else std::cout <<"[FAIL]"<< std::endl;
		return ret;
	}

	void VOXMODEL::calNeiborVoxel()
	{
		int sx,sy,sz;
		for(int i=0;i<width;i++){
			for(int j=0;j<height;j++){
				for(int k=0;k<depth;k++){
					for(int nx=0;nx<3;nx++){
						for(int ny=0;ny<3;ny++){
							for(int nz=0;nz<3;nz++){
								sx=(i+nx-1);
								sy=(j+ny-1);
								sz=(k+nz-1);
								//境界は２つ前の位置にオーバーラップさせる
								if((i==0&&nx==0)||(i==width-1&&nx==2))
									sx=i-nx+1;
								if((j==0&&ny==0)||(j==height-1&&ny==2))
									sy=j-ny+1;
								if((k==0&&nz==0)||(k==depth-1&&nz==2))
									sz=k-nz+1;
								data[i][j][k].neibor[3*(3*nz+ny)+nx]=width*(height*sz+sy)+sx;
							}
						}
					}
				}
			}
		}
	}

	int VOXMODEL::setNeiborColorWith(Vector3d _tColor, Vector3d _nColor, Vector3d _cColor)
	{
		int counter=0;
		int x,y,z;
		int nx,ny,nz;
		for(int i=0;i<this->numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(data[x][y][z].color==_tColor){
				for(int j=0;j<27;j++){
					nz=data[x][y][z].neibor[j]%depth;
					ny=(int)(data[x][y][z].neibor[j]/depth)%height;
					nx=(int)(data[x][y][z].neibor[j]/depth)/height;
					if(j!=13&&data[nx][ny][nz].color==_cColor){
						data[nx][ny][nz].color=_nColor;
						counter++;
					}
				}
			}
		}
		return counter;
	}
	void VOXMODEL::setLabelBetween(Vector3d _point1, Vector3d _point2, Vector3d _color, int _label)
	{
		int *LUT = new int[numVoxel];//ルックアップテーブル
		LUT[0]=0;
		int *newLUT = new int[numVoxel];//ルックアップテーブル
		int *label = new int[numVoxel];//ラベルを用意する
		int tlabel = 1;
		memset(label,-1,sizeof(int)*numVoxel);//ラベルの値を初期化する

		int x,y,z;
		int nx,ny,nz;
		for(int i=0;i<this->numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(this->data[x][y][z].color.abs()!=0&&this->data[x][y][z].color!=INVIS_COL){//条件を満たすボクセルのみ探索する
				if( isFacesSandPoint(_point1, _point2, data[x][y][z].center) ){
					int min = INT_MAX;//ひとまずこれが上限
					for(int j=0;j<27;j++){//26近傍ボクセルに対して
						nz=data[x][y][z].neibor[j]%depth;
						ny=(int)(data[x][y][z].neibor[j]/depth)%height;
						nx=(int)(data[x][y][z].neibor[j]/depth)/height;
						if(isFacesSandPoint(_point1, _point2, data[nx][ny][nz].center)){
							if(j!=13&&data[x][y][z].color==data[nx][ny][nz].color){//関心ボクセルと同じ色のボクセルが近傍に存在した
								if(label[data[x][y][z].neibor[j]]!=-1){//近傍ボクセルにラベルが既にセットされていた
									if(min>label[data[x][y][z].neibor[j]])//最小のラベルの値を求めよう
										min=label[data[x][y][z].neibor[j]];
								}
							}
						}
					}
					if(min!=INT_MAX){
						label[i]=min;//最小の番号を振る
						for(int j=0;j<27;j++){
							nz=data[x][y][z].neibor[j]%depth;
							ny=(int)(data[x][y][z].neibor[j]/depth)%height;
							nx=(int)(data[x][y][z].neibor[j]/depth)/height;
							if(isFacesSandPoint(_point1, _point2, data[nx][ny][nz].center)){
								if(j!=13&&data[x][y][z].color==data[nx][ny][nz].color){//関心ボクセルと同じ色のボクセルが近傍に存在した
									if(label[data[x][y][z].neibor[j]]!=-1){//近傍ボクセルにラベルがセットされていた
										LUT[label[data[x][y][z].neibor[j]]]=min;//LUTを最小の番号で書き換える
										label[data[x][y][z].neibor[j]]=min;
									}
									else{//一致したボクセルの先取り
										label[data[x][y][z].neibor[j]]=min;
									}
								}
							}
						}
					}
					else{//近傍にはまだラベルをつけたボクセルが存在しない
						label[i]=tlabel;
						LUT[tlabel]=tlabel;
						tlabel++;
					}
				}
			}
		}
		//ルックアップテーブルを詰め直す
		int count=0;
		for(int i=0;i<tlabel;i++){
			for(int j=0;j<i;j++){//重複があればカウントを減らしてbreak
				if(LUT[i]==LUT[j]){
					newLUT[i]=newLUT[j];
					count--;
					break;
				}
			}
			newLUT[i]=count;
			count++;
		}

		//ルックアップテーブルを使ってラベルを振り直す
		tlabel=-1;
		std::cout<<this->getIndexOfVoxelAt(_point1)<<std::endl;
		if(this->getIndexOfVoxelAt(_point1)!=-1){
			for(int i=0;i<27;i++){
				z=this->getIndexOfVoxelAt(_point1)%depth;
				y=(int)(this->getIndexOfVoxelAt(_point1)/depth)%height;
				x=(int)(this->getIndexOfVoxelAt(_point1)/depth)/height;
				if(label[data[x][y][z].neibor[i]]!=-1)
					tlabel = label[data[x][y][z].neibor[i]];
			}
		}
		if(tlabel!=-1){
			for(int i=0;i<numVoxel;i++){
				z=i%depth;
				y=(int)(i/depth)%height;
				x=(int)(i/depth)/height;
				if(this->data[x][y][z].color.abs()!=0&&this->data[x][y][z].color!=INVIS_COL){
					if( isFacesSandPoint(_point1, _point2, data[x][y][z].center) ){
						if(newLUT[label[i]]==newLUT[tlabel]){
							this->data[x][y][z].color= _color;
							this->data[x][y][z].labelindex=_label;
						}
					}
				}
			}
		}
		delete []LUT;
		delete []newLUT;
		delete []label;
	}
	void VOXMODEL::labeling()
	{
		int numLabel=this->setLabel();
//		std::cout << numLabel << std::endl;
//		this->setColorMap(numLabel);

		//ここからは別の関数として実装
	//	this->resetColorAt(0,Vector3d(0,0,0));
	//	this->resetColorSurfAndInExceptAt(0, Vector3d(1,0,0), Vector3d(1,0,1));
	}
	int VOXMODEL::setLabel()
	{
		//一番最初のラベル番号はゼロから始める
		int min;
		int label=1;
		int *LUT=new int[numVoxel];//ルックアップテーブル
		int *newLUT=new int[numVoxel];//ルックアップテーブル
		//全てのボクセルに対して処理を行う
		LUT[0]=0;
		int x,y,z;
		int nx,ny,nz;
		for(int i=0;i<this->numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(data[x][y][z].isSurface){//サーフェースはすべてラベル0にする
				data[x][y][z].label=0;
			}
			else{//サーフェースでない領域が見つかった
				min=INT_MAX;//ひとまずこれが上限
				//26近傍ボクセルに対して
				for(int j=0;j<27;j++){
					nz=data[x][y][z].neibor[j]%depth;
					ny=(int)(data[x][y][z].neibor[j]/depth)%height;
					nx=(int)(data[x][y][z].neibor[j]/depth)/height;
					if(j!=13&&data[x][y][z].color==data[nx][ny][nz].color){//関心ボクセルと同じ色のボクセルが近傍に存在した
						if(data[nx][ny][nz].label!=-1){//近傍ボクセルにラベルが既にセットされていた
							//最小のラベルの値を求めよう
							if(min>data[nx][ny][nz].label)
								min=data[nx][ny][nz].label;
						}
					}
				}
				if(min!=INT_MAX){
					data[x][y][z].label=min;//最小の番号を振る
					for(int j=0;j<27;j++){
						nz=data[x][y][z].neibor[j]%depth;
						ny=(int)(data[x][y][z].neibor[j]/depth)%height;
						nx=(int)(data[x][y][z].neibor[j]/depth)/height;
						if(j!=13&&data[x][y][z].color==data[nx][ny][nz].color){//関心ボクセルと同じ色のボクセルが近傍に存在した
							if(data[nx][ny][nz].label!=-1){//近傍ボクセルにラベルがセットされていた
								LUT[data[nx][ny][nz].label]=min;//LUTを最小の番号で書き換える
								data[nx][ny][nz].label=min;
								//問題は連結領域ではない部分も同じ色に変わっていること
							}
							else{//一致したボクセルの先取り
								data[nx][ny][nz].label=min;
							}
						}
					}
				}
				else{//近傍にはまだラベルをつけたボクセルが存在しない
					data[x][y][z].label=label;
					LUT[label]=label;
					label++;
				}
			}
		}
		//ルックアップテーブルを詰め直す
		int count=0;
		for(int i=0;i<label;i++){
			for(int j=0;j<i;j++){//重複があればカウントを減らしてbreak
				if(LUT[i]==LUT[j]){
					newLUT[i]=newLUT[j];
					count--;
					break;
				}
			}
			newLUT[i]=count;
			count++;
		}
		label=count;

		//std::cout<<"Label"<<std::endl;
		//ルックアップテーブルを使ってラベルを振り直す
		for(int i=0;i<numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			data[x][y][z].label=newLUT[data[x][y][z].label];
//			std::cout<<data[x][y][z].label<<std::endl;
		}
		delete []LUT;
		delete []newLUT;
		return label;
	}
	void VOXMODEL::setColorMap(int _num)
	{
		Vector3d HSV;
		Vector3d *RGB=new Vector3d[_num+2];
		//HSVモデルに基づいてラベリングを行う
		for(int i=0;i<_num;i++){
			HSV=Vector3d((i+1)*360.0/(_num+1), 1.0, 1.0);
			RGB[i]=HSV2RGB(HSV);
	//		std::cout<<i<<"::"<<RGB[i]<<std::endl;
		}
		int x,y,z;
		for(int i=0;i<this->numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			data[x][y][z].color=RGB[data[x][y][z].label];
		}
	
		delete []RGB;
	}

	void VOXMODEL::resetColorAt(int _index, Vector3d _color, bool _isFreeSpace)
	{
		int x,y,z;
		z=_index%depth;
		y=(int)(_index/depth)%height;
		x=(int)(_index/depth)/height;
		freeLabel=data[x][y][z].label;
		for(int i=0;i<numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(data[x][y][z].label==freeLabel){
				data[x][y][z].color=_color;
				data[x][y][z].isFreeSpace=_isFreeSpace;
			}
		}
	}
	void VOXMODEL::resetColorSurfAndInExceptAt(int _index, Vector3d _sColor, Vector3d _iColor)
	{
		int x,y,z;
		int ix,iy,iz;
		iz=_index%depth;
		iy=(int)(_index/depth)%height;
		ix=(int)(_index/depth)/height;
		for(int i=0;i<numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(data[x][y][z].label!=data[ix][iy][iz].label){
				data[x][y][z].isFreeSpace=false;
				if(data[x][y][z].isSurface)
					data[x][y][z].color=_sColor;
				else
					data[x][y][z].color=_iColor;
			}
			else{
				data[x][y][z].isFreeSpace=true;
			}
		}
	}
	void VOXMODEL::setGradMapWithinSurf()
	{
		//自由空間を黒で埋める
		this->resetColorAt();
		//表面を白,物体内部を赤で埋める
		this->resetColorSurfAndInExceptAt(0, SURF_COL, INTR_COL);

		int numRerigion;
		int numRep=0;
		Vector3d target=SURF_COL;
		do{
			numRerigion=this->setNeiborColorWith(target, target+SURF_COL, INTR_COL);
			numRep++;
			target+=SURF_COL;
		}while(numRerigion>0);	//赤い領域がなくなるまで続ける
		//色をスケーリングする
		colormap.setParam(1,numRep,50);
		int x,y,z;
		for(int i=0;i<numVoxel;i++){
			z=i%depth;
			y=(int)(i/depth)%height;
			x=(int)(i/depth)/height;
			if(data[x][y][z].color!=FREE_COL&&data[x][y][z].color!=SURF_COL)
				data[x][y][z].color=colormap.getColorMap(data[x][y][z].color.z);
		}
		//刺激の階層がいくらか
		this->mapDepth=numRep;
	}

	void VOXMODEL::setColorWithinSurf()
	{
		//自由空間を黒で埋める
		this->resetColorAt();
		//表面の色を取得,物体内部をその色で埋める（1色物体を仮定）
		int x, y, z;
		Vector3d color_t;
		for (int i = 0; i < numVoxel; i++) {
			z = i%depth;
			y = (int)(i / depth) % height;
			x = (int)(i / depth) / height;
			if (data[x][y][z].isSurface) {
				color_t = data[x][y][z].color;
				break;
			}
		}
		std::cout << color_t << std::endl;
//		this->resetColorSurfAndInExceptAt(0, SURF_COL, INTR_COL);
		this->resetColorSurfAndInExceptAt(0, color_t, color_t);
	}


	bool VOXMODEL::millingSurfaceWith(int _index)
	{
		int ix,iy,iz;
		int nx,ny,nz;
		iz=_index%depth;
		iy=(int)(_index/depth)%height;
		ix=(int)(_index/depth)/height;
		bool isMilled=false;
		if(!this->isDataLoaded)return isMilled;
		if(_index>=0){
			if(!data[ix][iy][iz].isFreeSpace){
				data[ix][iy][iz].color=FREE_COL;
				data[ix][iy][iz].isFreeSpace=true;
				data[ix][iy][iz].isSurface=false;
				data[ix][iy][iz].label=freeLabel;
				for(int i=0;i<27;i++){
					nz=data[ix][iy][iz].neibor[i]%depth;
					ny=(int)(data[ix][iy][iz].neibor[i]/depth)%height;
					nx=(int)(data[ix][iy][iz].neibor[i]/depth)/height;
					if(i!=13&&!data[nx][ny][nz].isFreeSpace){
						data[nx][ny][nz].isSurface=true;
						data[nx][ny][nz].color=SURF_COL;
						isMilled=true;
					}
				}
			}
		}
		return isMilled;
	}
	bool VOXMODEL::coloringSurfaceWith(int _index, Vector3d _col)
	{
		int ix,iy,iz;
		iz=_index%depth;
		iy=(int)(_index/depth)%height;
		ix=(int)(_index/depth)/height;
		bool isMilled=false;
		if(!this->isDataLoaded)return isMilled;
		if(_index>=0){
			if(!data[ix][iy][iz].isFreeSpace){
				data[ix][iy][iz].color=_col;
			}
		}
		return isMilled;
	}
	void VOXMODEL::setFreeSpaceAt(int _index)
	{
		int ix,iy,iz;
		int nx,ny,nz;
		iz=_index%depth;
		iy=(int)(_index/depth)%height;
		ix=(int)(_index/depth)/height;
		data[ix][iy][iz].color=FREE_COL;
		data[ix][iy][iz].isFreeSpace=true;
		data[ix][iy][iz].isSurface=false;
		data[ix][iy][iz].label=freeLabel;
		for(int i=0;i<27;i++){
			nz=data[ix][iy][iz].neibor[i]%depth;
			ny=(int)(data[ix][iy][iz].neibor[i]/depth)%height;
			nx=(int)(data[ix][iy][iz].neibor[i]/depth)/height;
			if(i!=13&&!data[nx][ny][nz].isFreeSpace){
				data[nx][ny][nz].isSurface=true;
				data[nx][ny][nz].color=SURF_COL;
			}
		}
	}
}