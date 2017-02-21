#pragma once

#include <fstream>
#include <iostream>
#include <GL/glut.h>
#include <ms_vectormatrix.h>
#include <ms_surfmesh.h>

namespace ModelStudio
{
	class OBJMESH: public surfMesh
	{
	protected:
		bool loadOBJFile(const char *filename);
		bool loadMTLFile(const char *filename);
		char objFileName[256];
		char mtlFileName[256];
		char directoryName[256];
	public:
		OBJMESH(surfMesh *parent = 0){}
		~OBJMESH(){}
		char* getName(){return this->objFileName;}
		bool load(const char *_filename);
		bool save(const char *_filename);
		void setup();
	};
};