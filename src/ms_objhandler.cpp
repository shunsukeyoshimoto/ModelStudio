#include "ms_objhandler.h"

namespace ModelStudio
{

	char * GetDirectoryName(const char *filename, char *dest)
	{
		char *strings = NULL;
		char *dir;
		dir = new char [strlen(filename)+1];
		strcpy(dir, filename);
		if ( strings = strrchr(dir, '/' ) ) strings[1] = '\0';
		else if ( strings = strrchr(dir, '\\') ) strings[1] ='\0';
		else dir[0] = '\0';
		strcpy(dest, dir);
		return dest;
	}
	bool OBJMESH::loadOBJFile(const char *filename)
	{
		std::ifstream file;
		int cvi = -1;
		int cni = -1;
		int cfi = -1;
		int cmi =  0;
		float tmp_float;
		char tmp_char[256];
		char buf[256];
		char *pbuf;
		Vector3d tp;
		strcpy(objFileName, filename);
		file.open(filename, std::ios::in);
		if ( !file.is_open() ){
	//		std::cout << "[FAIL]" << std::endl;
			return false;
		}
		this->clearMesh();
		this->deleteMesh();
		file.getline(buf, sizeof(buf));
		while( !file.eof() ){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "of "));
			else if(strstr(buf, "v "))this->numVertex++;
			else if(strstr(buf, "vn "))this->numNormal++;
			else if(strstr(buf, "f "))this->numFacet++;
			else if(strstr(buf, "mtllib ")){
				sscanf(buf, "mtllib %s", &tmp_char);
				sprintf_s(buf,"%s%s", directoryName, tmp_char);
				if ( !loadMTLFile(buf))
					return false;
			}
		}
		this->numLine=numFacet*4;
		this->newMesh();
		file.clear();
		file.seekg(0, std::fstream::beg);
		while( !file.eof() ){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "v ")){
				cvi++;
				if ( sscanf_s(buf, "v %lf %lf %lf",&tp.x, &tp.y, &tp.z) == 3 ){
					if ( sscanf_s(buf, "v %lf %lf %lf %f %f %f",&tp.x, &tp.y, &tp.z, &color[cvi].x, &color[cvi].y, &color[cvi].z) == 6 ){
						this->isVertexColorEnabled=true;
					}
					this->vertex[cvi]=tp;
				}
				else {
//					std::cout << "[FAIL]"<<std::endl;
					return false;
				}
			}
			else if(strstr(buf, "vn")){
				cni++;
				if ( sscanf_s(buf, "vn %lf %lf %lf", &this->normal[cni].x, &this->normal[cni].y, &this->normal[cni].z) != 3){
//					std::cout << "[FAIL]"<<std::endl;
					return false;
				}
			}
			else if(strstr(buf, "f ")){
				cfi++;
				pbuf = buf;
				while ( *pbuf ){
					if ( *pbuf == ' ' ) this->facet[cfi].numVertex++;
					pbuf++;
				}
				if ( this->facet[cfi].numVertex < 3 ){
//					std::cout << "[FAIL]" << std::endl;
					return false;
				}
				else if ( this->facet[cfi].numVertex ==3){
					this->facet[cfi].setFacetTypeAsTriangle();
				}
				else if ( this->facet[cfi].numVertex >3){
					this->facet[cfi].setFacetTypeAsPolygon(this->facet[cfi].numVertex);
				}
				else{
//					std::cout << "[FAIL]" << std::endl;
					return false;
				}
				pbuf = buf;
				for ( int i=0; i<this->facet[cfi].numVertex; i++ ){
					pbuf = strchr(pbuf, ' ');
					pbuf++;
					if ( sscanf_s(pbuf, "%d/%d/%d", &this->facet[cfi].vertexIndex[i], &tmp_float, &this->facet[cfi].normalIndex[i] ) != 3 ){
						if ( sscanf_s(pbuf, "%d//%d", &this->facet[cfi].vertexIndex[i], &this->facet[cfi].normalIndex[i] ) != 2 ){
							if ( sscanf_s(pbuf, "%d/%d", &this->facet[cfi].vertexIndex[i], &tmp_float ) != 2 ){
								sscanf_s(pbuf, "%d", &this->facet[cfi].vertexIndex[i]);
								this->facet[cfi].normalType = NORMAL_NONE;
							}
							else{
								this->facet[cfi].normalType = NORMAL_NONE;
							}
						}
						else{
							this->facet[cfi].normalType = NORMAL_FACET;
						}
					}
					else{
						this->facet[cfi].normalType = NORMAL_POINT;
					}
					this->facet[cfi].vertexIndex[i]--;
					this->facet[cfi].facetIndex = cfi;
					if ( this->facet[cfi].normalType!=NORMAL_NONE )
						this->facet[cfi].normalIndex[i]--;
				}
				this->facet[cfi].materialIndex = cmi;
			}
			if(strstr(buf, "usemtl ")){
				sscanf(buf, "usemtl %s", &tmp_char);
				for ( int i=0; i<numMaterial; i++ ){
					if ( strcmpi(material[i].getName(), tmp_char) == 0 ) cmi = i;
				}
			}
		}
		file.close();
		return true;
	}

	bool OBJMESH::loadMTLFile(const char *filename)
	{
		std::ifstream file;
		int cmi = -1;
		char buf[256];
		int ti=4;
		float tmp_float=0.0f;
		Vector3f temp;
		Material _mat;
		strcpy(mtlFileName, filename); 
		file.open(filename, std::ios::in);
		if ( !file.is_open() )
		{
//			std::cout << "Error 0: File open error!" << std::endl;
			return false;
		}
		this->numMaterial=0;
		this->deleteMaterial();
		while( !file.eof() ){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "newmtl "))this->numMaterial++;
		}
		this->newMaterial();
		file.clear();
		file.seekg(0, std::fstream::beg);
		while ( !file.eof() )
		{
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "newmtl ")){
				cmi++;
				sscanf(buf, "newmtl %s", this->material[cmi].getName());
			}
			else if(strstr(buf, "Ka ")){
				sscanf(buf, "Ka %f %f %f", &temp.x, &temp.y, &temp.z);
				this->material[cmi].setAmbient(temp);
			}
			else if(strstr(buf, "Kd ")){
				sscanf(buf, "Kd %f %f %f", &temp.x, &temp.y, &temp.z);
				this->material[cmi].setColor(temp);
				this->material[cmi].setDiffuse(temp);
			}
			else if(strstr(buf, "Ks ")){
				sscanf(buf, "Ks %f %f %f", &temp.x, &temp.y, &temp.z);
				this->material[cmi].setSpecular(temp);
			}
			else if(strstr(buf, "Tr ")){
				if(sscanf(buf, "Tr %f %f %f", &temp.x, &temp.y, &temp.z)==3)
					this->material[cmi].setEmission(temp);
			}
			else if(strstr(buf, "Tf ")){
				if(sscanf(buf, "Tf %f %f %f", &temp.x, &temp.y, &temp.z)==3)
					this->material[cmi].setEmission(temp);
			}
			else if(strstr(buf, "Ni ")){
				if(sscanf(buf, "Ni %f", &temp.x))
					this->material[cmi].setShininess(temp.x);
			}
			else if(strstr(buf, "Ns ")){
				if(sscanf(buf, "Ns %f", &temp.x))
					this->material[cmi].setShininess(temp.x);
			}
			else if(strstr(buf, "illum ")){
				if(sscanf(buf, "illum %d", &ti))
					this->material[cmi].setIllum(ti);
			}
			else if(strstr(buf, "d ")){
				if(sscanf(buf, "d %f", &tmp_float)){
					this->material[cmi].setIllum(5);
					this->material[cmi].setColorA(tmp_float);
					this->material[cmi].setAmbientA(tmp_float);
					this->material[cmi].setDiffuseA(tmp_float);
					this->material[cmi].setSpecularA(tmp_float);
					this->material[cmi].setEmissionA(tmp_float);
				}
			}
		}
		file.close();
		return true;
	}

	bool OBJMESH::load(const char *_filename)
	{
		std::cout <<"Loading .obj data ...";
		GetDirectoryName(_filename, directoryName);
		if ( !loadOBJFile(_filename) ){
			std::cout <<"[FAIL]"<< std::endl;
			return false;
		}
		this->setup();
		std::cout <<"[OK]"<< std::endl;
		return true;
	}
	void OBJMESH::setup()
	{
		this->calFacetVertex();
		this->calFacetIndex();
		this->calFacetNormal();
		this->calCenter();
		this->calScale();
		this->calTransform();
		this->calLine();
		this->isLoaded=true;
	}
}