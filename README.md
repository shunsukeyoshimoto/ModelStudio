# ModelStudio
	Software to view, edit, and convert 3D models

*Author
	Shunsuke Yoshimoto

*Overview


*Smpale command

********************* MODEL STUDIO v0.0.0*********************

Command list

[LOAD DATA] -l:filename(*.obj, *.stl, *.vox)

[CONVERT DATA] -c:(obj2stl, stl2obj, obj2vox:#res, stl2vox:#res)

[SAVE DATA] -s:filename(*.obj, *.stl, *.vox, (0:x,1:y,2:z):*.slice)

[EXIT] -q

-l:../data/sample.obj
Loading .obj data ...[OK]

-c:obj2stl
Converting OBJ data to STL data ...[OK]

-s:../data/sample.stl
Saving .stl data ...[OK]

-c:obj2vox:128
Converting OBJ data to VOX data ...[OK]

-s:2:../data/slice/img.slice
Saving .bmp data ...[OK]

-s:../data/sample.vox
Saving .vox data ...[OK]
