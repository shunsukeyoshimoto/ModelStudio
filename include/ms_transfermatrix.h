#pragma once
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ms_vectormatrix.h>

namespace ModelStudio{
	enum{
		EULER_ANGLE,
		ROLL_PITCH_YAW,
		YAW_PITCH_ROLL
	};
	template<class T>class transferMatrix
	{
	public:
		T X[16];
		T trX[16];
		Vector3<T> abg;
		transferMatrix(){for(int i=0;i<16;i++)X[i]=0;for(int i=0;i<4;i++)X[4*i+i]=1;}
		transferMatrix(T *_X){for(int i=0;i<16;i++)X[i]=_X[i];}
		~transferMatrix(){}

		transferMatrix &operator=(const transferMatrix &_src){for(int i=0;i<16;i++)X[i]=_src.X[i];abg=_src.abg;return(*this);}
		transferMatrix &operator=(const T _a){for(int i=0;i<16;i++)X[i]=_a;return(*this);}
		transferMatrix &operator+=(const transferMatrix &_src){for(int i=0;i<16;i++)X[i]+=_src.X[i];return(*this);}
		transferMatrix &operator-=(const transferMatrix &_src){for(int i=0;i<16;i++)X[i]-=_src.X[i];return(*this);}
		transferMatrix &operator*=(const T &_a){for(int i=0;i<16;i++)X[i]*=_a;return(*this);}
		transferMatrix &operator/=(const T &_a){for(int i=0;i<16;i++)X[i]/=_a;return(*this);}
		transferMatrix &operator*=(const transferMatrix &_src){
			transferMatrix dst;
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					dst.X[4*i+j]=0.0;
					for(int k=0;k<4;k++)
						dst.X[4*i+j]+=X[4*i+k]*_src.X[4*k+j];
				}
			}
			for(int i=0;i<16;i++)X[i]=dst.X[i];
			return(*this);
		}
		const bool operator!=(const transferMatrix &_src){
			for(i=0;i<4;i++)for(j=0;j<4;j++)if(this->X[4*i+j]!=_src.X[4*i+j])return true;
			return false;
		}

		template<class T>friend transferMatrix operator *(const T &_a, const transferMatrix<T> &_src){
			transferMatrix dst=_src;dst*=_a;return dst;
		}
	
		transferMatrix inv(){
			transferMatrix<T> src=(*this);
			transferMatrix<T> dst;
			Inverse_LU(dst.X,src.X,4);
			return dst;		
		}
		transferMatrix getRotate(){
			transferMatrix dst;
			for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
					dst.X[4*i+j]=this->X[4*i+j];
			return dst;
		}
		transferMatrix getTranslate(){
			transferMatrix dst;
			dst.X[ 3]=this->X[ 3];
			dst.X[ 7]=this->X[ 7];
			dst.X[11]=this->X[11];
			return dst;
		}
		Vector3<T> getTranslateVector3(){
			Vector3<T> dst;
			dst.x=this->X[ 3];
			dst.y=this->X[ 7];
			dst.z=this->X[11];
			return dst;
		}
		Vector3<T> getAlphaBetaGamma(){
			return abg;
		}
		void transferMatrix::setTranslate(Vector3<T> _tran){
			this->X[ 3]=_tran.x;
			this->X[ 7]=_tran.y;
			this->X[11]=_tran.z;
		}
		void setScale(T _scale){
			for(int i=0;i<3;i++)
				this->X[4*i+i]*=_scale;
		}
		void setScale(Vector3<T> _scale){
			for(int i=0;i<3;i++)
				this->X[4*i+i]*=_scale.X[i];
		}
		void setTranScale(T _scale){
			this->X[4*3+3]=_scale;
		}
		void setIdentity(){
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					if(i==j)X[4*j+i]=1;
					else X[4*j+i]=0;
				}
			}
		}
		void setRotate(Vector3<T> _rotate,int method=EULER_ANGLE){
			abg=_rotate;
			transferMatrix dst;
			if(method==EULER_ANGLE){
				transferMatrix roll;
				transferMatrix pitch;
				transferMatrix yaw;

				roll.X[0]=cos(_rotate.x/180*M_PI);
				roll.X[1]=-sin(_rotate.x/180*M_PI);
				roll.X[4]=sin(_rotate.x/180*M_PI);
				roll.X[5]=cos(_rotate.x/180*M_PI);

				pitch.X[5]=cos(_rotate.y/180*M_PI);
				pitch.X[6]=-sin(_rotate.y/180*M_PI);
				pitch.X[9]=sin(_rotate.y/180*M_PI);
				pitch.X[10]=cos(_rotate.y/180*M_PI);

				yaw.X[0]=cos(_rotate.z/180*M_PI);
				yaw.X[2]=sin(_rotate.z/180*M_PI);
				yaw.X[8]=-sin(_rotate.z/180*M_PI);
				yaw.X[10]=cos(_rotate.z/180*M_PI);

				dst=yaw*pitch;
				dst*=roll;
			}
			if(method==YAW_PITCH_ROLL){
				transferMatrix roll;
				transferMatrix pitch;
				transferMatrix yaw;

				roll.X[0]=cos(_rotate.x/180*M_PI);
				roll.X[1]=-sin(_rotate.x/180*M_PI);
				roll.X[4]=sin(_rotate.x/180*M_PI);
				roll.X[5]=cos(_rotate.x/180*M_PI);

				pitch.X[5]=cos(_rotate.y/180*M_PI);
				pitch.X[6]=-sin(_rotate.y/180*M_PI);
				pitch.X[9]=sin(_rotate.y/180*M_PI);
				pitch.X[10]=cos(_rotate.y/180*M_PI);

				yaw.X[0]=cos(_rotate.z/180*M_PI);
				yaw.X[2]=sin(_rotate.z/180*M_PI);
				yaw.X[8]=-sin(_rotate.z/180*M_PI);
				yaw.X[10]=cos(_rotate.z/180*M_PI);

				dst=roll*pitch;
				dst*=yaw;
			}
			else{
				dst.X[ 0]=cos(_rotate.x/180*M_PI)*cos(_rotate.y/180*M_PI);
				dst.X[ 1]=cos(_rotate.x/180*M_PI)*sin(_rotate.y/180*M_PI)*sin(_rotate.z/180*M_PI)-sin(_rotate.x/180*M_PI)*cos(_rotate.z/180*M_PI);
				dst.X[ 2]=cos(_rotate.x/180*M_PI)*sin(_rotate.y/180*M_PI)*cos(_rotate.z/180*M_PI)+sin(_rotate.x/180*M_PI)*sin(_rotate.z/180*M_PI);
	//			dst.X[ 3]=0;

				dst.X[ 4]=sin(_rotate.x/180*M_PI)*cos(_rotate.y/180*M_PI);
				dst.X[ 5]=sin(_rotate.x/180*M_PI)*sin(_rotate.y/180*M_PI)*sin(_rotate.z/180*M_PI)+cos(_rotate.x/180*M_PI)*cos(_rotate.z/180*M_PI);
				dst.X[ 6]=sin(_rotate.x/180*M_PI)*sin(_rotate.y/180*M_PI)*cos(_rotate.z/180*M_PI)-cos(_rotate.x/180*M_PI)*sin(_rotate.z/180*M_PI);
	//			dst.X[ 7]=0;

				dst.X[ 8]=-sin(_rotate.y/180*M_PI);
				dst.X[ 9]=cos(_rotate.y/180*M_PI)*sin(_rotate.z/180*M_PI);
				dst.X[10]=cos(_rotate.y/180*M_PI)*cos(_rotate.z/180*M_PI);
	//			dst.X[11]=0;

				dst.X[12]=0;
				dst.X[13]=0;
				dst.X[14]=0;
				dst.X[15]=1;
			}
			for(int ii=0;ii<16;ii++){
				this->X[ii]=dst.X[ii];
			}
		}
		void setRotMatrix(Vector3<T> _nx, Vector3<T> _ny, Vector3<T> _nz){
			X[ 0]=_nx.x;X[ 1]=_nx.y;X[ 2]=_nx.z;
			X[ 4]=_ny.x;X[ 5]=_ny.y;X[ 6]=_ny.z;
			X[ 8]=_nz.x;X[ 9]=_nz.y;X[10]=_nz.z;
		}

		T* getTr4GL(){
			for(int i=0;i<4;i++)for(int j=0;j<4;j++)trX[4*i+j]=X[4*j+i];return trX;
		}
		T tx(){return X[ 3];}
		T ty(){return X[ 7];}
		T tz(){return X[11];}

		void allineXYZto(Vector3d *_directions)
		{
			for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
					this->X[4*i+j]=_directions[i].X[j]/_directions[i].abs();
		}
		void allineZXYto(Vector3d *_directions)
		{
			for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
					this->X[4*i+j]=_directions[(i+1)%3].X[j]/_directions[(i+1)%3].abs();
		}
		void allineYZXto(Vector3d *_directions)
		{
			for(int i=0;i<3;i++)
				for(int j=0;j<3;j++)
					this->X[4*i+j]=_directions[(i+2)%3].X[j]/_directions[(i+2)%3].abs();
		}
		transferMatrix allineXto(Vector3d _vec){return alline(_vec, Vector3d(1,0,0)).inv();}
		transferMatrix allineYto(Vector3d _vec){return alline(_vec, Vector3d(0,1,0)).inv();}
		transferMatrix allineZto(Vector3d _vec){return alline(_vec, Vector3d(0,0,1)).inv();}
		transferMatrix alline(Vector3d _vec, Vector3d _XYZ){
			double d;
			transferMatrix result;
			transferMatrix r;
			transferMatrix s;
			Vector3d u=_XYZ;
			Vector3d n;
			Vector3d un;
			Vector3d vn;
			Vector3d v;
			v=_vec/_vec.abs();
			if(_vec.x==0&&_vec.y!=0&&_vec.z==0){
				result.X[5]*=_vec.y;
				return result;
			}
			n=u%v;
			un=u%n;
			vn=v%n;
			if (n.abs() != 0.0){
				r.X[1] = s.X[4] = n.x/n.abs();
				r.X[5] = s.X[5] = n.y/n.abs();
				r.X[9] = s.X[6] = n.z/n.abs();
				r.X[0] = u.x;
				r.X[4] = u.y;
				r.X[8] = u.z;
				r.X[2] = un.x/un.abs();
				r.X[6] = un.y/un.abs();
				r.X[10]= un.z/un.abs();


				s.X[0] = v.x/v.abs();
				s.X[1] = v.y/v.abs();
				s.X[2] = v.z/v.abs();
				s.X[8] = vn.x/vn.abs();
				s.X[9] = vn.y/vn.abs();
				s.X[10]= vn.z/vn.abs();

				result=r*s;
			}
			d=_vec.abs();
			result.X[0]*=d;
			result.X[1]*=d;
			result.X[2]*=d;
			result.X[4]*=d;
			result.X[5]*=d;
			result.X[6]*=d;
			result.X[8]*=d;
			result.X[9]*=d;
			result.X[10]*=d;
			return result;
		}

	};

	template<class T> transferMatrix<T> operator *(const transferMatrix<T> &_src1, const transferMatrix<T> &_src2){
			transferMatrix<T> dst=_src1;dst*=_src2;return dst;
	}

	template<class T> Vector3<T> operator *(const transferMatrix<T> &_src, const Vector3<T> &u){
		Vector3<T> result;
		Vector4<T> input;
		Vector4<T> output;
		for(int i=0;i<3;i++)input.X[i]=u.X[i];input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				output.X[i]+=_src.X[4*i+j]*input.X[j];
		for(int i=0;i<3;i++)result.X[i]=output.X[i]/output.X[3];
		return result;
	}
	template<class T> Vector3<T> operator *(const Vector3<T> &u, const transferMatrix<T> &_src){
		Vector3<T> result;
		Vector4<T> input;
		Vector4<T> output;
		for(int i=0;i<3;i++)input.X[i]=u.X[i];input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				output.X[i]+=input.X[j]*_src.X[4*j+i];
		for(int i=0;i<3;i++)result.X[i]=output.X[i]/output.X[3];
		return result;
	}
	template <class T>std::ostream& operator<<(std::ostream& stream, const transferMatrix<T> &_src){
		return stream <<_src.X[0]<<","<<_src.X[1]<<","<<_src.X[2]<<","<<_src.X[3]<<std::endl
					<<_src.X[4]<<","<<_src.X[5]<<","<<_src.X[6]<<","<<_src.X[7]<<std::endl
					<<_src.X[8]<<","<<_src.X[9]<<","<<_src.X[10]<<","<<_src.X[11]<<std::endl
					<<_src.X[12]<<","<<_src.X[13]<<","<<_src.X[14]<<","<<_src.X[15];
	}

	typedef	transferMatrix<int> transferMatrixi;
	typedef	transferMatrix<double> transferMatrixd;
	typedef	transferMatrix<float> transferMatrixf;

	class localObject{
	private:
		//世界座標・モデル座標変換行列
		transferMatrixd world2model;
		transferMatrixd model2world;
		//モデル座標・ボクセル座標変換行列
		transferMatrixd voxel2model;
		transferMatrixd model2voxel;
		//スケール位置自動調整行列
		transferMatrixd model2uni;
		transferMatrixd uni2model;
	public:
		localObject(){}
		~localObject(){}

		void setWorld2Model(transferMatrixd _world2model){
			this->world2model=_world2model;
			this->model2world=this->world2model.inv();
		}
		transferMatrixd getWorld2Model(){return this->world2model;}
		void setModel2World(transferMatrixd _model2world){
			this->model2world=_model2world;
			this->world2model=this->model2world.inv();
		}
		transferMatrixd getModel2World(){return this->model2world;}
		void setModel2Voxel(transferMatrixd _model2voxel){
			this->model2voxel=_model2voxel;
			this->voxel2model=this->model2voxel.inv();
		}
		transferMatrixd getModel2Voxel(){return this->model2voxel;}
		void setVoxel2Model(transferMatrixd _voxel2model){
			this->voxel2model=_voxel2model;
			this->model2voxel=this->voxel2model.inv();
		}
		transferMatrixd getVoxel2Model(){return this->voxel2model;}
		void setUni2Model(transferMatrixd _uni2model){
			this->uni2model=_uni2model;
			this->model2uni=this->uni2model.inv();
		}
		transferMatrixd getUni2Model(){return this->uni2model;}
		void setModel2Uni(transferMatrixd _model2uni){
			this->model2uni=_model2uni;
			this->uni2model=this->model2uni.inv();
		}
		transferMatrixd getModel2Uni(){return this->model2uni;}
	};
}