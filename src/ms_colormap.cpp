#include "ms_colormap.h"

namespace ModelStudio{

	COLORMAP::COLORMAP()
	{
		min=0;
		max=0;
		offset=0;
		rate=0;
		this->setColorMap();
	}

	COLORMAP::~COLORMAP()
	{
	}

	void COLORMAP::setParam(double _min, double _max, int _offset, int _rate)
	{
		min=_min;
		max=_max;
		offset=_offset;
		rate=_rate;
	}

	bool COLORMAP::load(const char *_filename)
	{
		char buf[256];
		std::ifstream file;
		std::cout<<"# Load color map.........";
		file.open(_filename, std::ios::in);
		if ( !file.is_open() ){
			std::cout << "[FAIL]" << std::endl;
			return false;
		}
		for(int i=0;i<255;i++){
			file >> buf;
			sscanf(buf,"%lf,%lf,%lf",&colormap[i].x,&colormap[i].y,&colormap[i].z);
		}
		file.close();
		std::cout<<"[OK]"<<std::endl;
		return true;
	}

	Vector3d COLORMAP::glColorMap(double _value)
	{
		int index=offset+(int)((double)rate*_value/(max-min));
		if(index>250)index=250;
		if(index<0)index=0;
		currentColor=colormap[index];
		glColor3dv(currentColor.X);
		return colormap[index];
	}
	Vector3d COLORMAP::getColorMap(double _value)
	{
		int index=offset+(int)((double)rate*_value/(max-min));
		if(index>250)index=250;
		if(index<0)index=0;
		currentColor=colormap[index];
		return colormap[index];
	}
	int* COLORMAP::getColorMapip(double _value){
		int index=offset+(int)((double)rate*_value/(max-min));
		if(index>250)index=250;
		if(index<0)index=0;
		currentColor=colormap[index];
		rgb[0]=currentColor.x*255;
		rgb[1]=currentColor.y*255;
		rgb[2]=currentColor.z*255;
		return rgb;
	}
	void COLORMAP::setColorMap()
	{
		for(int i = 0; i < 42; i++){
			colormap[i].x = 0;
			colormap[i].y = 0;
			colormap[i].z = 199;
		}
		colormap[42].x = 0;colormap[42].y= 0;colormap[42].z= 199;
		colormap[43].x = 0;colormap[43].y= 0;colormap[43].z= 200;
		colormap[44].x = 0;colormap[44].y= 0;colormap[44].z= 201;
		colormap[45].x = 0;colormap[45].y= 0;colormap[45].z= 203;
		colormap[46].x = 0;colormap[46].y= 0;colormap[46].z= 204;
		colormap[47].x = 0;colormap[47].y= 0;colormap[47].z= 206;
		colormap[48].x = 0;colormap[48].y= 0;colormap[48].z= 207;
		colormap[49].x = 0;colormap[49].y= 0;colormap[49].z= 209;
		colormap[50].x = 0;colormap[50].y= 0;colormap[50].z= 210;
		colormap[51].x = 0;colormap[51].y= 0;colormap[51].z= 211;
		colormap[52].x = 0;colormap[52].y= 0;colormap[52].z= 212;
		colormap[53].x = 0;colormap[53].y= 1;colormap[53].z= 213;
		colormap[54].x = 0;colormap[54].y= 5;colormap[54].z= 215;
		colormap[55].x = 0;colormap[55].y= 8;colormap[55].z= 216;
		colormap[56].x = 0;colormap[56].y= 13;colormap[56].z= 218;
		colormap[57].x = 0;colormap[57].y= 17;colormap[57].z= 219;
		colormap[58].x = 0;colormap[58].y= 21;colormap[58].z= 220;
		colormap[59].x = 0;colormap[59].y= 24;colormap[59].z= 222;
		colormap[60].x = 0;colormap[60].y= 28;colormap[60].z= 223;
		colormap[61].x = 0;colormap[61].y= 31;colormap[61].z= 224;
		colormap[62].x= 0;colormap[62].y= 35;colormap[62].z= 225;
		colormap[63].x= 0;colormap[63].y= 40;colormap[63].z= 227;
		colormap[64].x= 0;colormap[64].y= 44;colormap[64].z= 228;
		colormap[65].x= 0;colormap[65].y= 47;colormap[65].z= 229;
		colormap[66].x= 0;colormap[66].y= 51;colormap[66].z= 231;
		colormap[67].x= 0;colormap[67].y= 55;colormap[67].z= 232;
		colormap[68].x= 0;colormap[68].y= 59;colormap[68].z= 233;
		colormap[69].x= 0;colormap[69].y= 63;colormap[69].z= 234;
		colormap[70].x= 0;colormap[70].y= 66;colormap[70].z= 236;
		colormap[71].x= 0;colormap[71].y= 70;colormap[71].z= 237;
		colormap[72].x= 0;colormap[72].y= 74;colormap[72].z= 238;
		colormap[73].x= 0;colormap[73].y= 78;colormap[73].z= 240;
		colormap[74].x= 0;colormap[74].y= 82;colormap[74].z= 241;
		colormap[75].x= 0;colormap[75].y= 85;colormap[75].z= 243;
		colormap[76].x= 0;colormap[76].y= 89;colormap[76].z= 244;
		colormap[77].x= 0;colormap[77].y= 93;colormap[77].z= 245;
		colormap[78].x= 0;colormap[78].y= 97;colormap[78].z= 246;
		colormap[79].x= 0;colormap[79].y= 101;colormap[79].z= 247;
		colormap[80].x= 0;colormap[80].y= 105;colormap[80].z= 249;
		colormap[81].x= 0;colormap[81].y= 108;colormap[81].z= 250;
		colormap[82].x= 0;colormap[82].y= 112;colormap[82].z= 252;
		colormap[83].x= 0;colormap[83].y= 116;colormap[83].z= 253;
		colormap[84].x= 0;colormap[84].y= 120;colormap[84].z= 254;
		colormap[85].x= 0;colormap[85].y= 124;colormap[85].z= 251;
		colormap[86].x= 0;colormap[86].y= 127;colormap[86].z= 248;
		colormap[87].x= 0;colormap[87].y= 131;colormap[87].z= 245;
		colormap[88].x= 0;colormap[88].y= 135;colormap[88].z= 242;
		colormap[89].x= 0;colormap[89].y= 139;colormap[89].z= 239;
		colormap[90].x= 0;colormap[90].y= 143;colormap[90].z= 235;
		colormap[91].x= 0;colormap[91].y= 146;colormap[91].z= 232;
		colormap[92].x= 0;colormap[92].y= 150;colormap[92].z= 229;
		colormap[93].x= 0;colormap[93].y= 154;colormap[93].z= 227;
		colormap[94].x= 0;colormap[94].y= 158;colormap[94].z= 224;
		colormap[95].x= 0;colormap[95].y= 162;colormap[95].z= 221;
		colormap[96].x= 0;colormap[96].y= 166;colormap[96].z= 217;
		colormap[97].x= 0;colormap[97].y= 169;colormap[97].z= 214;
		colormap[98].x= 0;colormap[98].y= 173;colormap[98].z= 211;
		colormap[99].x= 0;colormap[99].y= 177;colormap[99].z= 208;
		colormap[100].x= 0;colormap[100].y= 181;colormap[100].z= 206;
		colormap[101].x= 0;colormap[101].y= 185;colormap[101].z= 203;
		colormap[102].x= 0;colormap[102].y= 188;colormap[102].z= 199;
		colormap[103].x= 0;colormap[103].y= 192;colormap[103].z= 196;
		colormap[104].x= 0;colormap[104].y= 196;colormap[104].z= 193;
		colormap[105].x= 0;colormap[105].y= 200;colormap[105].z= 190;
		colormap[106].x= 0;colormap[106].y= 201;colormap[106].z= 187;
		colormap[107].x= 0;colormap[107].y= 202;colormap[107].z= 184;
		colormap[108].x= 0;colormap[108].y= 204;colormap[108].z= 181;
		colormap[109].x= 0;colormap[109].y= 206;colormap[109].z= 178;
		colormap[110].x= 0;colormap[110].y= 208;colormap[110].z= 175;
		colormap[111].x= 0;colormap[111].y= 210;colormap[111].z= 172;
		colormap[112].x= 0;colormap[112].y= 212;colormap[112].z= 169;
		colormap[113].x= 0;colormap[113].y= 213;colormap[113].z= 166;
		colormap[114].x= 0;colormap[114].y= 215;colormap[114].z= 163;
		colormap[115].x= 0;colormap[115].y= 217;colormap[115].z= 160;
		colormap[116].x= 0;colormap[116].y= 219;colormap[116].z= 157;
		colormap[117].x= 0;colormap[117].y= 221;colormap[117].z= 154;
		colormap[118].x= 0;colormap[118].y= 222;colormap[118].z= 151;
		colormap[119].x= 0;colormap[119].y= 224;colormap[119].z= 148;
		colormap[120].x= 0;colormap[120].y= 225;colormap[120].z= 144;
		colormap[121].x= 0;colormap[121].y= 227;colormap[121].z= 142;
		colormap[122].x= 0;colormap[122].y= 229;colormap[122].z= 139;
		colormap[123].x= 0;colormap[123].y= 231;colormap[123].z= 135;
		colormap[124].x= 0;colormap[124].y= 233;colormap[124].z= 132;
		colormap[125].x= 0;colormap[125].y= 234;colormap[125].z= 129;
		colormap[126].x= 0;colormap[126].y= 236;colormap[126].z= 126;
		colormap[127].x= 0;colormap[127].y= 238;colormap[127].z= 123;
		colormap[128].x= 0;colormap[128].y= 240;colormap[128].z= 121;
		colormap[129].x= 0;colormap[129].y= 241;colormap[129].z= 117;
		colormap[130].x= 0;colormap[130].y= 243;colormap[130].z= 114;
		colormap[131].x= 0;colormap[131].y= 245;colormap[131].z= 111;
		colormap[132].x= 0;colormap[132].y= 246;colormap[132].z= 108;
		colormap[133].x= 0;colormap[133].y= 249;colormap[133].z= 104;
		colormap[134].x= 0;colormap[134].y= 252;colormap[134].z= 99;
		colormap[135].x= 0;colormap[135].y= 253;colormap[135].z= 95;
		colormap[136].x= 0;colormap[136].y= 254;colormap[136].z= 93;
		colormap[137].x= 0;colormap[137].y= 255;colormap[137].z= 91;
		colormap[138].x= 0;colormap[138].y= 255;colormap[138].z= 89;
		colormap[139].x= 0;colormap[139].y= 255;colormap[139].z= 87;
		colormap[140].x= 0;colormap[140].y= 255;colormap[140].z= 83;
		colormap[141].x= 0;colormap[141].y= 255;colormap[141].z= 80;
		colormap[142].x= 0;colormap[142].y= 255;colormap[142].z= 77;
		colormap[143].x= 0;colormap[143].y= 255;colormap[143].z= 75;
		colormap[144].x= 0;colormap[144].y= 255;colormap[144].z= 72;
		colormap[145].x= 0;colormap[145].y= 255;colormap[145].z= 69;
		colormap[146].x= 0;colormap[146].y= 255;colormap[146].z= 65;
		colormap[147].x= 0;colormap[147].y= 255;colormap[147].z= 62;
		colormap[148].x= 6;colormap[148].y= 255;colormap[148].z= 59;
		colormap[149].x= 12;colormap[149].y= 255;colormap[149].z= 56;
		colormap[150].x= 18;colormap[150].y= 255;colormap[150].z= 54;
		colormap[151].x= 24;colormap[151].y= 255;colormap[151].z= 51;
		colormap[152].x= 30;colormap[152].y= 255;colormap[152].z= 47;
		colormap[153].x= 36;colormap[153].y= 255;colormap[153].z= 44;
		colormap[154].x= 43;colormap[154].y= 255;colormap[154].z= 41;
		colormap[155].x= 49;colormap[155].y= 255;colormap[155].z= 38;
		colormap[156].x= 55;colormap[156].y= 255;colormap[156].z= 35;
		colormap[157].x= 60;colormap[157].y= 255;colormap[157].z= 32;
		colormap[158].x= 66;colormap[158].y= 255;colormap[158].z= 29;
		colormap[159].x= 73;colormap[159].y= 255;colormap[159].z= 26;
		colormap[160].x= 79;colormap[160].y= 255;colormap[160].z= 23;
		colormap[161].x= 85;colormap[161].y= 255;colormap[161].z= 20;
		colormap[162].x= 91;colormap[162].y= 255;colormap[162].z= 17;
		colormap[163].x= 97;colormap[163].y= 255;colormap[163].z= 14;
		colormap[164].x= 103;colormap[164].y= 255;colormap[164].z= 11;
		colormap[165].x= 109;colormap[165].y= 255;colormap[165].z= 8;
		colormap[166].x= 115;colormap[166].y= 255;colormap[166].z= 5;
		colormap[167].x= 121;colormap[167].y= 255;colormap[167].z= 2;
		colormap[168].x= 127;colormap[168].y= 255;colormap[168].z= 0;
		colormap[169].x= 134;colormap[169].y= 255;colormap[169].z= 0;
		colormap[170].x= 140;colormap[170].y= 255;colormap[170].z= 0;
		colormap[171].x= 146;colormap[171].y= 255;colormap[171].z= 0;
		colormap[172].x= 152;colormap[172].y= 255;colormap[172].z= 0;
		colormap[173].x= 158;colormap[173].y= 255;colormap[173].z= 0;
		colormap[174].x= 164;colormap[174].y= 255;colormap[174].z= 0;
		colormap[175].x= 170;colormap[175].y= 255;colormap[175].z= 0;
		colormap[176].x= 176;colormap[176].y= 255;colormap[176].z= 0;
		colormap[177].x= 182;colormap[177].y= 255;colormap[177].z= 0;
		colormap[178].x= 188;colormap[178].y= 255;colormap[178].z= 0;
		colormap[179].x= 194;colormap[179].y= 255;colormap[179].z= 0;
		colormap[180].x= 201;colormap[180].y= 255;colormap[180].z= 0;
		colormap[181].x= 207;colormap[181].y= 255;colormap[181].z= 0;
		colormap[182].x= 212;colormap[182].y= 255;colormap[182].z= 0;
		colormap[183].x= 218;colormap[183].y= 255;colormap[183].z= 0;
		colormap[184].x= 224;colormap[184].y= 255;colormap[184].z= 0;
		colormap[185].x= 231;colormap[185].y= 255;colormap[185].z= 0;
		colormap[186].x= 237;colormap[186].y= 255;colormap[186].z= 0;
		colormap[187].x= 243;colormap[187].y= 255;colormap[187].z= 0;
		colormap[188].x= 249;colormap[188].y= 255;colormap[188].z= 0;
		colormap[189].x= 255;colormap[189].y= 254;colormap[189].z= 0;
		colormap[190].x= 255;colormap[190].y= 250;colormap[190].z= 0;
		colormap[191].x= 255;colormap[191].y= 246;colormap[191].z= 0;
		colormap[192].x= 255;colormap[192].y= 243;colormap[192].z= 0;
		colormap[193].x= 255;colormap[193].y= 239;colormap[193].z= 0;
		colormap[194].x= 255;colormap[194].y= 235;colormap[194].z= 0;
		colormap[195].x= 255;colormap[195].y= 231;colormap[195].z= 0;
		colormap[196].x= 255;colormap[196].y= 227;colormap[196].z= 0;
		colormap[197].x= 255;colormap[197].y= 223;colormap[197].z= 0;
		colormap[198].x= 255;colormap[198].y= 220;colormap[198].z= 0;
		colormap[199].x= 255;colormap[199].y= 216;colormap[199].z= 0;
		colormap[200].x= 255;colormap[200].y= 212;colormap[200].z= 0;
		colormap[201].x= 255;colormap[201].y= 208;colormap[201].z= 0;
		colormap[202].x= 255;colormap[202].y= 204;colormap[202].z= 0;
		colormap[203].x= 255;colormap[203].y= 201;colormap[203].z= 0;
		colormap[204].x= 255;colormap[204].y= 197;colormap[204].z= 0;
		colormap[205].x= 255;colormap[205].y= 193;colormap[205].z= 0;
		colormap[206].x= 255;colormap[206].y= 189;colormap[206].z= 0;
		colormap[207].x= 255;colormap[207].y= 185;colormap[207].z= 0;
		colormap[208].x= 255;colormap[208].y= 181;colormap[208].z= 0;
		colormap[209].x= 255;colormap[209].y= 178;colormap[209].z= 0;
		colormap[210].x= 255;colormap[210].y= 174;colormap[210].z= 0;
		colormap[211].x= 255;colormap[211].y= 170;colormap[211].z= 0;
		colormap[212].x= 255;colormap[212].y= 166;colormap[212].z= 0;
		colormap[213].x= 255;colormap[213].y= 162;colormap[213].z= 0;
		colormap[214].x= 255;colormap[214].y= 159;colormap[214].z= 0;
		colormap[215].x= 255;colormap[215].y= 155;colormap[215].z= 0;
		colormap[216].x= 255;colormap[216].y= 151;colormap[216].z= 0;
		colormap[217].x= 255;colormap[217].y= 147;colormap[217].z= 0;
		colormap[218].x= 255;colormap[218].y= 143;colormap[218].z= 0;
		colormap[219].x= 255;colormap[219].y= 140;colormap[219].z= 0;
		colormap[220].x= 255;colormap[220].y= 136;colormap[220].z= 0;
		colormap[221].x= 255;colormap[221].y= 132;colormap[221].z= 0;
		colormap[222].x= 255;colormap[222].y= 128;colormap[222].z= 0;
		colormap[223].x= 255;colormap[223].y= 124;colormap[223].z= 0;
		colormap[224].x= 255;colormap[224].y= 121;colormap[224].z= 0;
		colormap[225].x= 255;colormap[225].y= 117;colormap[225].z= 0;
		colormap[226].x= 255;colormap[226].y= 113;colormap[226].z= 0;
		colormap[227].x= 255;colormap[227].y= 109;colormap[227].z= 0;
		colormap[228].x= 255;colormap[228].y= 105;colormap[228].z= 0;
		colormap[229].x= 255;colormap[229].y= 101;colormap[229].z= 0;
		colormap[230].x= 255;colormap[230].y= 98;colormap[230].z= 0;
		colormap[231].x= 255;colormap[231].y= 94;colormap[231].z= 0;
		colormap[232].x= 255;colormap[232].y= 90;colormap[232].z= 0;
		colormap[233].x= 255;colormap[233].y= 86;colormap[233].z= 0;
		colormap[234].x= 255;colormap[234].y= 82;colormap[234].z= 0;
		colormap[235].x= 255;colormap[235].y= 79;colormap[235].z= 0;
		colormap[236].x= 255;colormap[236].y= 75;colormap[236].z= 0;
		colormap[237].x= 255;colormap[237].y= 71;colormap[237].z= 0;
		colormap[238].x= 255;colormap[238].y= 67;colormap[238].z= 0;
		colormap[239].x= 255;colormap[239].y= 63;colormap[239].z= 0;
		colormap[240].x= 255;colormap[240].y= 60;colormap[240].z= 0;
		colormap[241].x= 255;colormap[241].y= 56;colormap[241].z= 0;
		colormap[242].x= 255;colormap[242].y= 52;colormap[242].z= 0;
		colormap[243].x= 255;colormap[243].y= 48;colormap[243].z= 0;
		colormap[244].x= 255;colormap[244].y= 44;colormap[244].z= 0;
		colormap[245].x= 255;colormap[245].y= 40;colormap[245].z= 0;
		colormap[246].x= 255;colormap[246].y= 37;colormap[246].z= 0;
		colormap[247].x= 255;colormap[247].y= 33;colormap[247].z= 0;
		colormap[248].x= 255;colormap[248].y= 29;colormap[248].z= 0;
		colormap[249].x= 255;colormap[249].y= 25;colormap[249].z= 0;
		colormap[250].x= 255;colormap[250].y= 21;colormap[250].z= 0;
		colormap[251].x= 255;colormap[251].y= 18;colormap[251].z= 0;
		colormap[252].x= 255;colormap[252].y= 14;colormap[252].z= 0;
		colormap[253].x= 255;colormap[253].y= 9;colormap[253].z= 0;
		colormap[254].x= 255;colormap[254].y= 5;colormap[254].z= 0;
		colormap[255].x= 255;colormap[255].y= 2;colormap[255].z= 0;

		for(int i=0;i<256;i++)colormap[i]/=255.0;
	}

	Vector3d HSV2RGB(Vector3d HSV){
		Vector3d RGB;
		int H_i;
		double f,p,q,t;
		if(fabs(HSV.x)>360)
			HSV.x=HSV.x-((int)(HSV.x/360))*360;
		if(HSV.x<0)
			HSV.x=360-HSV.x;

		H_i=(int)floor(HSV.X[0]/60.0)%6;
		f=HSV.X[0]/60.0-(double)H_i;
		p=HSV.X[2]*(1-HSV.X[1]);
		q=HSV.X[2]*(1-f*HSV.X[1]);
		t=HSV.X[2]*(1-(1-f)*HSV.X[1]);
		if(H_i==0){			RGB.X[0]=HSV.X[2]; RGB.X[1]=t; RGB.X[2]=p;	}
		else if(H_i==1){	RGB.X[0]=q; RGB.X[1]=HSV.X[2]; RGB.X[2]=p;	}
		else if(H_i==2){	RGB.X[0]=p; RGB.X[1]=HSV.X[2]; RGB.X[2]=t;	}
		else if(H_i==3){	RGB.X[0]=p; RGB.X[1]=q; RGB.X[2]=HSV.X[2];	}
		else if(H_i==4){	RGB.X[0]=t; RGB.X[1]=p; RGB.X[2]=HSV.X[2];	}
		else if(H_i==5){	RGB.X[0]=HSV.X[2]; RGB.X[1]=p; RGB.X[2]=q;	}
		return RGB;
	}
	void MinMax(Vector3d RGB, double *_min, double *_max){
		*_min=*_max=0.0;
		for(int i=0;i<3;i++){
			if(*_min>RGB.X[i])*_min=RGB.X[i];
			if(*_max<RGB.X[i])*_max=RGB.X[i];
		}
	}
	Vector3d RGB2HSV(Vector3d RGB){
		Vector3d HSV;
		double min,max;
		MinMax(RGB,&min,&max);
		if(max!=min){
			for(int i=0;i<3;i++){
				if(max==RGB.X[i]){
					HSV.X[0]=60.0*(RGB.X[(i+1)%3]-RGB.X[(i+2)%3])/(max-min)+120.0*(double)i;
				}
			}
			if(HSV.X[0]<0)HSV.X[0]+=360.0;
		}
		else HSV.X[0]=-1;
		HSV.X[1]=max-min;
		HSV.X[2]=max;
		return HSV;
	}
}