#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <ms_pointlineface.h>
#include <ms_vectormatrix.h>
#include <ms_transfermatrix.h>
#include <ms_colormap.h>
#include <ms_glutilities.h>
#include <ms_imageprocessing.h>

#define GAIN_MARGIN 1.2

namespace ModelStudio
{
	#define FREE_COL Vector3d(0,0,0)
	#define SURF_COL Vector3d(0,0,1)
	#define INTR_COL Vector3d(0,1,1)
	#define INVIS_COL Vector3d(1,1,1)

	enum COORDINATE{
		COORD_LOCAL,
		COORD_WORLD
	};
	enum{
		X_AXIS,
		Y_AXIS,
		Z_AXIS
	};

	class VOXMODEL
	{
	private:
		class Voxel
		{
		public:
			int neibor[27];
			bool isSurface;
			bool isFreeSpace;
			int label;
			int labelindex;
			Vector3d color;
			Vector3d center;
			Voxel &operator=(const Voxel &parent){
				label=parent.label;
				labelindex=parent.labelindex;
				color=parent.color;
				center=parent.center;
				isSurface=parent.isSurface;
				isFreeSpace=parent.isFreeSpace;
				for(int i=0;i<27;i++)neibor[i]=parent.neibor[i];
				return(*this);
			}
			Voxel(){
				label=-1;center=Vector3d(0,0,0); color=Vector3d(0,0,0); isSurface=false; isFreeSpace=false;
				labelindex=-1;
				for(int i=0;i<27;i++)neibor[i]=false;
			}
			~Voxel(){}
		}***data;
		COLORMAP colormap;

		Material mat;
		bool isDataLoaded;
		bool isViewData;
		bool isDrawWireFrame;
		bool isViewSlice;
		bool isAffine;
		bool isMilling;
		int curStimMap;
		int numVoxel;
		int mapDepth;
		int width;
		int height;
		int depth;
		int freeLabel;
		double scale;
		Vector3d size;
		Vector3d center;
		double length;
		unsigned char *pixels;
		unsigned char *buffer;
		IMG *slice;
		Vector3d slicePos;
		GLuint texName[3];


		//���f��-�{�N�Z���ϊ��s��̌v�Z
		void calModel2Voxel();
		//�X�P�[���ʒu���������s��̌v�Z
		void calAutoAffine();
		//�{�N�Z�����S�̌v�Z
		void calVoxelCenter();
		//�ߖT�{�N�Z���̐ݒ�
		void calNeiborVoxel();

	public:
		VOXMODEL(void);
		~VOXMODEL(void);

		//���W�n�̍s��
		localObject Tr;

		void initTexture();
		//�X�P�[���͍ő�T�C�Y
		void setScale(double _scale){scale=_scale * GAIN_MARGIN;}
		double getScale(){return scale;}
		//�T�C�Y��3����
		void setSize(Vector3d _size){size=_size;}
		void setSize(double _size){size=Vector3d(_size,_size,_size);}
		void setVoxelNum(int _width, int _height, int _depth);
		void setVoxelSameNum(int _sizeVoxel);
		void setVoxelValue(int x, int y, int z, double _val){
			data[x][y][z].color=Vector3d(0,0,1);
			data[x][y][z].isSurface=true;}
		void setVoxelColor(int x, int y, int z, Vector3d _color){
			data[x][y][z].color=_color;
			data[x][y][z].isSurface=true;}
		void setVoxelValue(int _index, double _val){
			int x,y,z;
			z=_index%depth;
			y=(int)(_index/depth)%height;
			x=(int)(_index/depth)/height;
			data[x][y][z].color=Vector3d(0,0,1);
			data[x][y][z].isSurface=true;}

		Vector3d getSize(){return size;}
		int getWidth(){return width;}
		int getHeight(){return height;}
		int getDepth(){return depth;}
		int getNumVoxel(){return numVoxel;}
		Vector3d getCenter(int x, int y, int z){return data[x][y][z].center;}
		Vector3d getCenter(int _index){
			int x,y,z;
			z=_index%depth;
			y=(int)(_index/depth)%height;
			x=(int)(_index/depth)/height;
			return data[x][y][z].center;
		}

		//���W�ݒ�̌v�Z
		void calCoordinateSetting();

		//�ϊ��s��̐ݒ�y�ю擾

		int getIndexOfVoxelAt(Vector3d _Puni, int _coordinate=COORD_WORLD);
		void deleteVoxel();
		void newVoxel();
		void render(bool _isScaled=true);
		void renderGuid();
		void clear();
		bool getIsLoaded(){return this->isDataLoaded;}
		bool load(const char* _filename);
		bool save(const char* _filename);
		void setCenter(Vector3d _center){this->center=_center;}
		Vector3d getCenter(){return this->center;}
		void setLength(double _length){this->length=_length;}
		double getLength(){return length;}
		void setIsDrawWireFrame(bool _isDrawWireFrame){this->isDrawWireFrame=_isDrawWireFrame;}
		void setIsDataLoaded(bool _isDataLoaded){this->isDataLoaded=_isDataLoaded;}
		//�`�惂�[�h�̐ݒ�
		void setIsViewData(bool _isViewData){this->isViewData=_isViewData;}
		void setIsViewSlice(bool _isViewSlice){this->isViewSlice=_isViewSlice;}
		void setIsAffine(bool _isAffine){this->isAffine=_isAffine;}
		bool getIsAffine(){return this->isAffine;}

		Vector3d getColorAt(int _width, int _height ,int _depth);
		Vector3d getColorAt(int _index);
		int getLabelIndexAt(int _index);
		void setFreeSpaceAt(int _index);
		void setSlicePos(Vector3d _slicePos){this->slicePos=_slicePos;}
		void setSliceSize(int _size);
		void setSlice(bool _invert = false);
		void renderGrid();
		void renderSlicer();
		void renderImages(int _width, int _height);

		bool saveSlice(const char *_filename, int _axis = X_AXIS);

		//�N���[�W���O���X���C�X�摜�ɑ΂��ēK�p����
		void closingSlice(int _times);

		//����̐F�̃{�N�Z���ߖT���C����̐F�̏����ł���Ύw�肵���F�ɕς���(�߂�l�͕ς����{�N�Z���̌�)
		int setNeiborColorWith(Vector3d _tColor, Vector3d _nColor, Vector3d _cColor=Vector3d(0,0,0));
		//�_�ŋ��ޗ̈�i���ӁF_point1�͕��̂̓����ɐݒ肵�Ȃ���΂Ȃ�Ȃ��j
		void setLabelBetween(Vector3d _point1, Vector3d _point2, Vector3d _color, int _label);
		//���x�����O���s��
		void labeling();
		//���x�����Z�b�g����
		int setLabel();
		//�A����Ԃ����̐F�œh��Ԃ�
		void setColorMap(int _num);
		//����̃{�N�Z���̐F�Ɠ����{�N�Z�������ׂĎw�肵���F�œh��ւ���
		void resetColorAt(int _index=0, Vector3d _color=Vector3d(0,0,0), bool _isFreeSpace=true);
		//����̃{�N�Z���̐F�ȊO�̃{�N�Z����Surface�Ɠ����ɂ킯�ēh�肩����
		void resetColorSurfAndInExceptAt(int _index, Vector3d _sColor, Vector3d _iColor);
		//���̂̓����ɃO���f�[�V�����}�b�v�𐶐�����
		void setGradMapWithinSurf();
		//���̂̓�����\�ʂƓ����F�œh��Ԃ�
		void setColorWithinSurf();

		//�����index�ɂ�����Surface�{�N�Z�������ASurface��ݒ肵����
		bool millingSurfaceWith(int _index);
		bool coloringSurfaceWith(int _index, Vector3d _col);
		//����͒P�ɓ���̈ʒu�ɂ�����{�N�Z�������R��Ԃɂ���Method
		void millingVoxels(Vector3d _Pworld);

	};
};
