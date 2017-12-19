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


		//モデル-ボクセル変換行列の計算
		void calModel2Voxel();
		//スケール位置自動調整行列の計算
		void calAutoAffine();
		//ボクセル中心の計算
		void calVoxelCenter();
		//近傍ボクセルの設定
		void calNeiborVoxel();

	public:
		VOXMODEL(void);
		~VOXMODEL(void);

		//座標系の行列
		localObject Tr;

		void initTexture();
		//スケールは最大サイズ
		void setScale(double _scale){scale=_scale * GAIN_MARGIN;}
		double getScale(){return scale;}
		//サイズは3次元
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

		//座標設定の計算
		void calCoordinateSetting();

		//変換行列の設定及び取得

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
		//描画モードの設定
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

		//クロージングをスライス画像に対して適用する
		void closingSlice(int _times);

		//特定の色のボクセル近傍を，特定の色の条件であれば指定した色に変える(戻り値は変えたボクセルの個数)
		int setNeiborColorWith(Vector3d _tColor, Vector3d _nColor, Vector3d _cColor=Vector3d(0,0,0));
		//点で挟む領域（注意：_point1は物体の内部に設定しなければならない）
		void setLabelBetween(Vector3d _point1, Vector3d _point2, Vector3d _color, int _label);
		//ラベリングを行う
		void labeling();
		//ラベルをセットする
		int setLabel();
		//連結空間を特定の色で塗りつぶす
		void setColorMap(int _num);
		//特定のボクセルの色と同じボクセルをすべて指定した色で塗り替える
		void resetColorAt(int _index=0, Vector3d _color=Vector3d(0,0,0), bool _isFreeSpace=true);
		//特定のボクセルの色以外のボクセルをSurfaceと内部にわけて塗りかえる
		void resetColorSurfAndInExceptAt(int _index, Vector3d _sColor, Vector3d _iColor);
		//物体の内部にグラデーションマップを生成する
		void setGradMapWithinSurf();
		//物体の内部を表面と同じ色で塗りつぶす
		void setColorWithinSurf();

		//特定のindexにおけるSurfaceボクセルを削り、Surfaceを設定し直す
		bool millingSurfaceWith(int _index);
		bool coloringSurfaceWith(int _index, Vector3d _col);
		//これは単に特定の位置におけるボクセルを自由空間にするMethod
		void millingVoxels(Vector3d _Pworld);

	};
};
