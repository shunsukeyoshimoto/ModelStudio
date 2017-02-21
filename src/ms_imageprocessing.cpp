#include "ms_imageprocessing.h"

namespace ModelStudio
{

	IMG::IMG()
	{
		header=new BitmapHeader;
		data=new unsigned char[256];
		setHeader();
		header->biWidth=0;
		header->biHeight=0;
	}
	IMG::~IMG()
	{
		delete header;
		delete []data;
	}

	void IMG::setHeader()
	{
		header->bfType='B' | ('M' << 8);
		header->bfReserved1=0;
		header->bfReserved2=0;
		header->bfOffBits=54;
		header->biSize=40;
		header->biCompression=0;
		header->biPlanes=1;
		header->biClrUsed=0;
		header->biClrImportant=0;
		header->biSizeImage=0;
		header->biXPelsPerMeter=11771;
		header->biYPelsPerMeter=11771;
	}
	void IMG::setSize(int _width, int _height, int _depth)
	{
		header->biWidth=_width;
		header->biHeight=_height;
		header->biBitCount=_depth*8;
		header->bfSize=_width*_height*_depth+header->bfOffBits;
		numColors=_width*_height;
		numPixels=_width*_height*_depth;
		delete []data;
		data=new unsigned char[numPixels];
	}

	bool IMG::load(const char* _filename){ 
		FILE *fp;
		if((fp = fopen(_filename, "rb"))==NULL)
			return false;
		fread(&header->bfType, sizeof(header->bfType), 1, fp);
		fread(&header->bfSize, sizeof(header->bfSize), 1, fp);
		fread(&header->bfReserved1, sizeof(header->bfReserved1), 1, fp);
		fread(&header->bfReserved2, sizeof(header->bfReserved2), 1, fp);
		fread(&header->bfOffBits, sizeof(header->bfOffBits), 1, fp);
		fread(&header->biSize, sizeof(header->biSize), 1, fp);
		fread(&header->biWidth, sizeof(header->biWidth), 1, fp);
		fread(&header->biHeight, sizeof(header->biHeight), 1, fp);
		fread(&header->biPlanes, sizeof(header->biPlanes), 1, fp);
		fread(&header->biBitCount, sizeof(header->biBitCount), 1, fp);
		fread(&header->biCompression, sizeof(header->biCompression), 1, fp);
		fread(&header->biSizeImage, sizeof(header->biSizeImage), 1, fp);
		fread(&header->biXPelsPerMeter, sizeof(header->biXPelsPerMeter), 1, fp);
		fread(&header->biYPelsPerMeter, sizeof(header->biYPelsPerMeter), 1, fp);
		fread(&header->biClrUsed, sizeof(header->biClrUsed), 1, fp);
		fread(&header->biClrImportant, sizeof(header->biClrImportant), 1, fp);
		delete []data;
		data=new unsigned char[this->width()*this->height()*this->depth()];
		fseek(fp, header->bfOffBits, SEEK_SET );
		fread(data,header->bfSize-header->bfOffBits, 1,fp);
		fclose(fp);
		numColors=width()*height();
		numPixels=width()*height()*depth();
		return true;
	}
	bool IMG::save(const char* _filename){ 
		FILE *fp;
		if((fp = fopen(_filename, "wb"))==NULL)
			return false;
		fwrite(&header->bfType, sizeof(header->bfType), 1, fp);
		fwrite(&header->bfSize, sizeof(header->bfSize), 1, fp);
		fwrite(&header->bfReserved1, sizeof(header->bfReserved1), 1, fp);
		fwrite(&header->bfReserved2, sizeof(header->bfReserved2), 1, fp);
		fwrite(&header->bfOffBits, sizeof(header->bfOffBits), 1, fp);
		fwrite(&header->biSize, sizeof(header->biSize), 1, fp);
		fwrite(&header->biWidth, sizeof(header->biWidth), 1, fp);
		fwrite(&header->biHeight, sizeof(header->biHeight), 1, fp);
		fwrite(&header->biPlanes, sizeof(header->biPlanes), 1, fp);
		fwrite(&header->biBitCount, sizeof(header->biBitCount), 1, fp);
		fwrite(&header->biCompression, sizeof(header->biCompression), 1, fp);
		fwrite(&header->biSizeImage, sizeof(header->biSizeImage), 1, fp);
		fwrite(&header->biXPelsPerMeter, sizeof(header->biXPelsPerMeter), 1, fp);
		fwrite(&header->biYPelsPerMeter, sizeof(header->biYPelsPerMeter), 1, fp);
		fwrite(&header->biClrUsed, sizeof(header->biClrUsed), 1, fp);
		fwrite(&header->biClrImportant, sizeof(header->biClrImportant), 1, fp);
		fwrite(data,header->bfSize-header->bfOffBits,1,fp);
		fclose(fp);
		return true;
	}

	void IMG::clear()
	{
		for(int i=0;i<this->numPixels;i++)
			data[i]=0;
	}

	void IMG::setColorAt(int _x, int _y, int *_rgb)
	{
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			for(int i=0;i<depth();i++)
				data[depth()*(width()*_y+_x)+i]=_rgb[i];
	}
	void IMG::setColorAt(int _x, int _y, double *_rgb)
	{
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			for(int i=0;i<depth();i++)
				data[depth()*(width()*_y+_x)+i]=_rgb[i];
	}
	void IMG::setColorAt(int _index, int *_rgb)
	{
		if(0<=_index&&_index<width()*height())
			for(int i=0;i<depth();i++)
				data[depth()*_index+i]=_rgb[i];
	}
	void IMG::setColorAt(int _x, int _y, int _luminance)
	{
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			for(int i=0;i<depth();i++)
				data[depth()*(width()*_y+_x)+i]=_luminance;
	}

	int IMG::getColorRedAt(int _x, int _y)
	{
		int red=0; 
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			red=data[depth()*(width()*_y+_x)+0];
		return red;
	}

	int IMG::getColorGreenAt(int _x, int _y)
	{
		int green=0; 
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			green=data[depth()*(width()*_y+_x)+1];
		return green;
	}

	int IMG::getColorBlueAt(int _x, int _y)
	{
		int blue=0; 
		if(0<=_x&&_x<width()&&0<=_y&&_y<height())
			blue=data[depth()*(width()*_y+_x)+2];
		return blue;
	}
	int IMG::getColorLumiAt(int _x, int _y){
		int lumi=0;
		lumi=getColorRedAt(_x,_y)+getColorGreenAt(_x,_y)+getColorBlueAt(_x,_y);
		return lumi;
	}
	int IMG::getColorLumiAt(int _index){
		int x=this->getXFromIndex(_index);
		int y=this->getYFromIndex(_index);
		return getColorLumiAt(x,y);
	}


	void IMG::getInfo(){
		std::cout<<"Type>>"<<header->bfType<<std::endl;
		std::cout<<"bfSize>>"<<header->bfSize<<std::endl;
		std::cout<<"Reserved1>>"<<header->bfReserved1<<std::endl;
		std::cout<<"Reserved2>>"<<header->bfReserved2<<std::endl;
		std::cout<<"OffBits>>"<<header->bfOffBits<<std::endl;
		std::cout<<"biSize>>"<<header->biSize<<std::endl;
		std::cout<<"Width>>"<<header->biWidth<<std::endl;
		std::cout<<"Height>>"<<header->biHeight<<std::endl;
		std::cout<<"Planes>>"<<header->biPlanes<<std::endl;
		std::cout<<"Count>>"<<header->biBitCount<<std::endl;
		std::cout<<"Compression>>"<<header->biCompression<<std::endl;
		std::cout<<"SizeImage>>"<<header->biSizeImage<<std::endl;
		std::cout<<"XPelsPerMeter>>"<<header->biXPelsPerMeter<<std::endl;
		std::cout<<"YPelsPerMeter>>"<<header->biYPelsPerMeter<<std::endl;
		std::cout<<"ClrUsed>>"<<header->biClrUsed<<std::endl;
		std::cout<<"ClrImportant>>"<<header->biClrImportant<<std::endl;
	}


	bool getPsudeColor(IMG *_dst, IMG *_src){
		COLORMAP map;
		map.setParam(0,255,0,200);
		if(_dst->width()!=_src->width()||_dst->height()!=_src->height())
			return false;
		for(int i=0;i<_src->width()*_src->height();i++)
			_dst->setColorAt(i,map.getColorMapip(_src->getColorLumiAt(i)));
		return true;
	}
	bool getFilteredImage(IMG *_dst, IMG *_src, Matrixd _ker){
		double rgbd[3]={0,0,0}; 
		int rgbi[3];
		if(_dst->width()!=_src->width()||_dst->height()!=_src->height())
			return false;
		for(int i=0;i<_src->height();i++){
			for(int j=0;j<_src->width();j++){
				for(int k=0;k<_src->depth();k++){
					for(int m=-(_ker.m-1)/2;m<_ker.m-(_ker.m-1)/2;m++){
						for(int n=-(_ker.n-1)/2;n<_ker.n-(_ker.n-1)/2;n++){
							if(0<=j+n&&j+n<_src->width()&&0<=i+m&&i+m<_src->height())
								rgbd[k]+=_ker(n+(_ker.n-1)/2,m+(_ker.m-1)/2)*_src->getColorAt(j+n,i+m,k);
						}
					}
					rgbi[k]=abs(rgbd[k]);
					rgbd[k]=0;
				}
				_dst->setColorAt(j,i,rgbi);
			}
		}
		return true;
	}
};