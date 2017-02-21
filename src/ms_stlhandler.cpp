#include "ms_stlhandler.h"

namespace ModelStudio
{

	bool STLMESH::load(const char *_filename)
	{
		this->isLoaded = false;
		std::cout <<"Loading .stl data ...";
		std::ifstream file;
		char buf[256];
		int count=0;
		if(_filename==NULL){
			std::cout << "[FAIL]" << std::endl;
			return false;
		}
		file.open(_filename, std::ios::in);
		if (!file.is_open()){
			std::cout << "[FAIL]" << std::endl;
			return false;
		}
		while(count<10){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "facet")){
				file.close();
				this->isLoaded=loadASCII(_filename);
			}
			else count++;
		}
		this->isLoaded=loadBinary(_filename);
		if(isLoaded)
			std::cout <<"[OK]"<< std::endl;
		else std::cout <<"[FAIL]"<< std::endl;
		return isLoaded;
	}


	bool STLMESH::loadASCII(const char* _filename)
	{
		int temp_countFacet=0;
		std::ifstream file;
		FACET sample;
		char buf[256];
		char dummy[256];
		char dummy2[256];
		file.open(_filename, std::ios::in);
		if (!file.is_open())return false;
		this->numVertex=0;
		this->numFacet=0;
		while (!file.eof()){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "vertex"))this->numVertex++;
			else if(strstr(buf, "normal"))this->numFacet++;
		}
		this->numMaterial=1;
		this->deleteMesh();
		this->deleteMaterial();
		this->newMesh();
		this->newMaterial();
		sample.setFacetTypeAsTriangle();
		file.clear();
		file.seekg(0, std::fstream::beg);
		while (!file.eof()){
			file.getline(buf, sizeof(buf));
			if(strstr(buf, "normal"))
				sscanf(buf, "%s %s %lf %lf %lf", dummy, dummy2, &sample.normal[0].x,&sample.normal[0].y,&sample.normal[0].z);
			else if(strstr(buf, "vertex")){
					sscanf(buf, "%s %lf %lf %lf", dummy, &sample.vertex[0].x,&sample.vertex[0].y,&sample.vertex[0].z);
					file.getline(buf, sizeof(buf));
					sscanf(buf, "%s %lf %lf %lf", dummy, &sample.vertex[1].x,&sample.vertex[1].y,&sample.vertex[1].z);
					file.getline(buf, sizeof(buf));
					sscanf(buf, "%s %lf %lf %lf", dummy, &sample.vertex[2].x,&sample.vertex[2].y,&sample.vertex[2].z);
					sample.facetIndex=temp_countFacet;
					sample.normalType=NORMAL_FACET;
					this->facet[temp_countFacet]=sample;
					sample.normalIndex[0]=temp_countFacet;
					sample.normalIndex[0]=temp_countFacet;
					sample.normalIndex[0]=temp_countFacet;
					normal[temp_countFacet]=sample.normal[0];
					temp_countFacet++;
			}
		}
		file.close();
		this->calVertex();
		this->calFacetNormal();
		this->calCenter();
		this->calScale();
		this->calTransform();
		this->calLine();
		return true;
	}
	bool STLMESH::loadBinary(const char* _filename)
	{
		int count=0;
		float *pf;
		int *pd;
		std::ifstream file;
		FACET sample;
		char buf[256];
		file.open(_filename, std::ios::in|std::ios::binary);
		if (!file.is_open())return false;
		file.read(buf,84);
		pd = (int *)&buf[80];
		this->numFacet=*pd;
		this->numNormal=this->numFacet;
		this->numVertex=3*this->numFacet;
		this->deleteMesh();//TODO:２回目で落ちるので要チェック
		this->newMesh();
		sample.setFacetTypeAsTriangle();
		while (file.read(buf,50)){
				pf = (float *)buf;
				sample.normal[0].x=*(pf+ 0);
				sample.normal[0].y=*(pf+ 1);
				sample.normal[0].z=*(pf+ 2);
				sample.vertex[0].x=*(pf+ 3);
				sample.vertex[0].y=*(pf+ 4);
				sample.vertex[0].z=*(pf+ 5);
				sample.vertex[1].x=*(pf+ 6);
				sample.vertex[1].y=*(pf+ 7);
				sample.vertex[1].z=*(pf+ 8);
				sample.vertex[2].x=*(pf+ 9);
				sample.vertex[2].y=*(pf+10);
				sample.vertex[2].z=*(pf+11);
				sample.facetIndex=count;
				sample.normalIndex[0]=count;
				sample.normalIndex[0]=count;
				sample.normalIndex[0]=count;
				normal[count]=sample.normal[0];
				facet[count]=sample;
				count++;
		}

		file.close();
		this->setup();
		return true;
	}
	bool STLMESH::save(const char *_filename, int _type)
	{
		bool ret =false;
		std::cout <<"Saving .stl data ...";
		if(_type==STL_ASCII)
			ret = saveASCII(_filename);
		if(_type==STL_BINARY)
			ret = saveBinary(_filename);

		if(ret)
			std::cout <<"[OK]"<< std::endl;
		else std::cout <<"[FAIL]"<< std::endl;
		return ret;
	}

	bool STLMESH::saveASCII(const char* _filename)
	{
		std::ofstream file;
		file.open(_filename, std::ios::out);
		if (!file.is_open())return false;
		file<<"solid "<<_filename<<" was written by ascii code."<<std::endl;
		for(int i=0;i<this->numFacet;i++){
			file<<"	facet normal "<<this->facet[i].normal[0].x<<" "<<this->facet[i].normal[0].y<<" "<<this->facet[i].normal[0].z<<std::endl;
			file<<"		outer loop"<<std::endl;		
			for(int j=0;j<3;j++)
				file<<"			vertex "<<this->facet[i].vertex[j].x<<" "<<this->facet[i].vertex[j].y<<" "<<this->facet[i].vertex[j].z<<std::endl;
			file<<"		endloop"<<std::endl;		
			file<<"	endfacet"<<std::endl;		
		}
		file<<"endsolid"<<std::endl;
		file.close();
		this->isLoaded=true;
		return true;
	}

	bool STLMESH::saveBinary(const char* _filename)
	{
		char *buf=new char[256];
		int *pd;
		float temp_data;
		std::ofstream file;
		file.open(_filename, std::ios::out|std::ios::binary);
		if (!file.is_open())return false;
		for(int i=0;i<80;i++)buf[i]=NULL;
		sprintf(buf,"Solid %s was written by binary code\n",_filename);
		file.write(buf,80);
		file.write((char*)&this->numFacet,4);
		for(int i=0;i<this->numFacet;i++){
			for(int j=0;j<3;j++){
				temp_data=this->facet[i].normal[0].X[j];
				file.write((char*)&temp_data,4);
			}
			for(int j=0;j<3;j++){
				for(int k=0;k<3;k++){
					temp_data=this->facet[i].vertex[j].X[k];
					file.write((char*)&temp_data,4);
				}
			}
			file.write("  ",2);
		}
		file.close();
		return true;
	}
	void STLMESH::setup()
	{
		this->calVertex();
		this->calFacetNormal();
		this->calCenter();
		this->calScale();
		this->calTransform();
		this->calLine();
		this->isLoaded = true;
	}
};
