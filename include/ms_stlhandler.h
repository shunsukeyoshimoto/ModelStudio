#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <GL/glut.h>
#include <ms_vectormatrix.h>
#include <ms_glutilities.h>
#include <ms_surfmesh.h>

namespace ModelStudio
{
	enum{
		STL_ASCII,
		STL_BINARY
	};
	class STLMESH: public surfMesh
	{
	public:
		STLMESH(surfMesh *parent=0){
			this->numMaterial=1;this->newMaterial();
		}
		~STLMESH(){this->deleteMaterial();}
		bool load(const char *_filename);
		bool loadASCII(const char *_filename);
		bool loadBinary(const char *_filename);
		bool save(const char *_filename, int _type=STL_BINARY);
		bool saveASCII(const char *_filename);
		bool saveBinary(const char *_filename);
		void setup();
	};
};