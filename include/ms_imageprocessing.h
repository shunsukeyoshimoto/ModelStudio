/****************************************************************************
** IMG
** Copyright (C) 2012 Shunsuke Yoshimoto.
** All rights reserved.
** Contact: Yoshimoto (yoshimotoÅóbpe.es.osaka-u.ac.jp)
****************************************************************************/

#pragma once;


#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <ms_vectormatrix.h>
#include <ms_colormap.h>

typedef unsigned short WORD;
typedef unsigned long DWORD;

namespace ModelStudio{
	enum{
		SOBEL,
		GAUSIAN,
		LAPLACIAN,
		MEDIAN,
		PREWITT_X,
		PREWITT_Y
	};

	class IMG
	{
	private:
		struct BitmapHeader{
			WORD  bfType;
			DWORD bfSize;
			WORD  bfReserved1,
				  bfReserved2;
			DWORD bfOffBits;
			DWORD biSize, 
				  biWidth, biHeight;
			WORD  biPlanes,
				  biBitCount;
			DWORD biCompression, 
				  biSizeImage,
				  biXPelsPerMeter,
				  biYPelsPerMeter,
				  biClrUsed, 
				  biClrImportant;
		}*header;
		unsigned char *data;
		int numPixels;
		int numColors;
		int getXFromIndex(int _index){return (int)(_index%width());}
		int getYFromIndex(int _index){return (int)(_index/width());}
	public:
		IMG();
		~IMG();
		IMG &operator=(const IMG &u){
			if(width()==u.header->biWidth&&height()==u.header->biHeight&&depth()==u.header->biBitCount/8)
			{memcpy(data, u.data, sizeof(unsigned char)*width()*height()*depth());return *this;}
			else return *this;
		}
		int width(){return header->biWidth;}
		int height(){return header->biHeight;}
		int depth(){return header->biBitCount/8;}
		unsigned char* getpData(){return data;}
		void clear();
		bool load(const char* _filename);
		bool save(const char* _filename);
		void setHeader();
		void setSize(int _width, int _height, int _depth=3);
		void setColorAt(int _x, int _y, int *_rgb);
		void setColorAt(int _x, int _y, double *_rgb);
		void setColorAt(int _index, int *_rgb);
		void setColorAt(int _x, int _y, int _luminance);
		int getColorRedAt(int _x, int _y);
		int getColorGreenAt(int _x, int _y);
		int getColorBlueAt(int _x, int _y);
		int getColorLumiAt(int _x, int _y);
		int getColorLumiAt(int _index);
		int getColorAt(int _x, int _y, int _d){return data[depth()*(width()*_y+_x)+_d];}
		int getColorAt(int _index, int _d){return data[depth()*_index+_d];}
		void getInfo();
	};

	bool getPsudeColor(IMG *_dst, IMG *_src);
	bool getFilteredImage(IMG *_dst, IMG *_src, Matrixd _ker);
};
