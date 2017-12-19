#pragma once

#include <process.h>
#include <windows.h>
#include <iostream>

#include <ms_vectormatrix.h>
#include <ms_transfermatrix.h>
#include <ms_glhandler.h>
#include <ms_glutilities.h>
#include <ms_objhandler.h>
#include <ms_stlhandler.h>
#include <ms_voxelhandler.h>
#include <ms_modelhandler.h>

ModelStudio::GLHandler *gl;
ModelStudio::VOXMODEL *vox;
ModelStudio::OBJMESH *obj;
ModelStudio::STLMESH *stl;
int argc;
char **argv;

//Key interface
void thread1(LPVOID pParam)
{
	std::cout<<"********************* MODEL STUDIO v0.0.0*********************"<<std::endl;
	std::cout<<"Command list"<<std::endl;
	std::cout<<"[LOAD DATA] -l:filename(*.obj, *.stl, *.vox)"<<std::endl;
	std::cout<<"[CONVERT DATA] -c:(obj2stl, stl2obj, obj2vox:#res, stl2vox:#res)"<<std::endl;
	std::cout <<"[PROCESS DATA] -p:(fill)" << std::endl;
	std::cout<<"[SAVE DATA] -s:filename(*.obj, *.stl, *.vox, (0:x,1:y,2:z):*.slice)"<<std::endl;
	std::cout<<"[EXIT] -q"<<std::endl;

	char command[256];
	char t_filename[256];
	while(true){
		std::cin>>command;
		if(strstr(command,"-l")){
			gl->setIsRun(false);
			sscanf(command,"-l:%s", t_filename);
			if(strstr(command,".obj")){
				if(obj->load(t_filename)){
					obj->setIsAffine(true);
				}
			}
			else if(strstr(command,".stl")){
				if(stl->load(t_filename)){
					stl->setIsAffine(true);
				}
			}
			else if(strstr(command,".vox")){
				if(vox->load(t_filename)){
					vox->setSlicePos(ModelStudio::Vector3d(vox->getWidth()/2.0,vox->getHeight()/2.0,vox->getDepth()/2.0));
					vox->setSlice();
					vox->setIsAffine(true);
				}
			}
			else{
				std::cout<<"-l:INVALID OPTION"<<std::endl;
			}
			gl->setIsRun(true);
		}
		else if(strstr(command,"-c")){
			gl->setIsRun(false);
			if(strstr(command,"obj2stl")){
				ModelStudio::convertOBJ2STL(obj,stl);
			}
			else if(strstr(command,"stl2obj")){
				ModelStudio::convertOBJ2STL(obj,stl);
			}
			else if(strstr(command,"obj2vox:")){
				int num=0;
				sscanf(command,"-c:obj2vox:%d", &num);
				if(num>0){
					if(ModelStudio::convertOBJ2VOX(obj,vox,num)){
						vox->setSlicePos(ModelStudio::Vector3d(num/2,num/2,num/2));
						vox->setSlice();
					}
				}
			}
			else if(strstr(command,"stl2vox:")){
				int num=0;
				sscanf(command,"-c:stl2vox:%d", &num);
				if(num>0&&num<=1024){
					if(ModelStudio::convertSTL2VOX(stl,vox,num)){
						vox->setSlicePos(ModelStudio::Vector3d(num/2,num/2,num/2));
						vox->setSlice();
					}
				}
				else{
					std::cout<<"-c:stl2vox:INVALID RESOLUTION"<<std::endl;
				}
			}
			else{
				std::cout<<"-c:INVALID OPTION"<<std::endl;
			}
			gl->setIsRun(true);
		}
		else if(strstr(command,"-s")){
			sscanf(command,"-s:%s", t_filename);
			if(strstr(command,".obj")){
				//TODO
				//obj->save(t_filename);
			}
			else if(strstr(command,".stl")){
				stl->save(t_filename);
			}
			else if(strstr(command,".vox")){
				vox->save(t_filename);
			}
			else if(strstr(command,".slice")){
				int t_axis;
				sscanf(command,"-s:%d:%s", &t_axis, t_filename);
				if(t_axis==ModelStudio::X_AXIS||t_axis==ModelStudio::Y_AXIS||t_axis==ModelStudio::Z_AXIS)
					vox->saveSlice(t_filename, t_axis);
				else{
					std::cout<<"-s:slice:INVALID OPTION"<<std::endl;
				}
			}
			else{
				std::cout<<"-s:INVALID OPTION"<<std::endl;
			}
		}
		else if (strstr(command, "-p")) {
			if (strstr(command, "fill")) {
				std::cout << "Filling color inside object...";
				if (vox->getIsLoaded()) {
					vox->labeling();
					vox->setColorWithinSurf();
					std::cout << "[OK]" << std::endl;
				}
				else std::cout << "[FAIL]" << std::endl;
			}
		}
		else if(strstr(command,"-q")){
			gl->setIsRun(false);
			std::cout<<"GOOD BYE"<<std::endl;
			Sleep(2000);
			exit(0);
		}
		else {
			std::cout<<"UNDEFINED COMMAND"<<std::endl;
		}
	}
}

//Window
void thread2(LPVOID pParam)
{
	gl->init(&argc,argv);
	glutInitWindowSize(400 , 300);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100 , 50);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(gl->displayGL);
	glutMotionFunc(gl->mouseMoveEventGL);
	glutMouseFunc(gl->mousePressEventGL);
	glutKeyboardFunc(gl->keyboardEventGL);
	glutMouseWheelFunc( gl->mouseWheelGL );
	glutIdleFunc(gl->idle);
	glutReshapeFunc(gl->resize);

	gl->initGL();
	glutMainLoop();
}

void object()
{
	if(gl->getIsRun()){
		obj->render();
		stl->render();
		vox->render();
		vox->renderGuid();
		vox->renderSlicer();
	}
}


int main(int _argc, char *_argv[])
{
	obj=new ModelStudio::OBJMESH;
	stl=new ModelStudio::STLMESH;
	vox=new ModelStudio::VOXMODEL;
	
	argc=_argc;
	argv=_argv;
	gl = new ModelStudio::GLHandler; 
	gl->getObject()->setObject(object);

	HANDLE hMutex;
	HANDLE hThread[3];
	hMutex = CreateMutex(NULL,FALSE,NULL);
	hThread[0] = (HANDLE)_beginthread(thread1,0,NULL);
	hThread[1] = (HANDLE)_beginthread(thread2,0,NULL);
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);
	CloseHandle(hMutex);

	delete gl;
	delete obj;
	delete vox;
	delete stl;
}

