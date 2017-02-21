#pragma once
#include <math.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <ms_vectormatrix.h>

namespace ModelStudio{
	class COLORMAP{
	private:
		Vector3d colormap[256];
		Vector3d currentColor;
		double min,max;
		int offset,rate;
		int rgb[3];
	public:
		COLORMAP();
		~COLORMAP();
		void setParam(double _min, double _max, int _offset, int _rate=200);
		bool load(const char*);
		void setColorMap();
		Vector3d glColorMap(double);
		Vector3d getColorMap(double);
		int* getColorMapip(double);
	};

	Vector3d HSV2RGB(Vector3d HSV);
	void MinMax(Vector3d RGB, double *_min, double *_max);
	Vector3d RGB2HSV(Vector3d RGB);
};