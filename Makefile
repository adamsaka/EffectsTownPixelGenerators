#*********************************************************************************************************
#This program is free software: you can redistribute it and/or modify it under the terms of the 
#GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
#or (at your option) any later version.

#This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
#without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#See the GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along with this program. 
#If not, see <https://www.gnu.org/licenses/>. 

#Authors:
#	Adam - Maths Town

#*********************************************************************************************************/

all: watercolour-texture

builddir := ..\build
htmldir := ..\public_html

#headers used by renderer
common_depend = common\colour.h common\linear-algebra.h common\noise.h

#===========================
#Watercolour texture project
#===========================
builddir_fxhash := $(builddir)\watercolour-texture\fxhash
htmldir_fxhash := $(htmldir)\effects\watercolour-texture\fxhash

watercolour-texture: watercolour-texture-fxhash

watercolour-texture-fxhash: $(htmldir_fxhash) $(builddir_fxhash) $(htmldir_fxhash)\index.html $(htmldir_fxhash)\main-cpp.js $(htmldir_fxhash)\main-background-cpp.js $(htmldir_fxhash)\main-render-worker-cpp.js 
#HTML
$(htmldir_fxhash)\index.html: hosts\fxhash\index.html
	copy /y hosts\fxhash\index.html $@

#Main Thread
$(builddir_fxhash)\main.o: hosts\fxhash\main.cpp 
	emcc hosts\fxhash\main.cpp -c -std=c++20  -o $@ -Oz -Wall -Wpedantic -Wextra

$(builddir_fxhash)\ui.o: hosts\fxhash\ui.cpp 
	emcc hosts\fxhash\ui.cpp -c -std=c++20  -o $@ -Oz -Wall -Wpedantic -Wextra

$(htmldir_fxhash)\main-cpp.js : $(builddir_fxhash)\ui.o $(builddir_fxhash)\main.o $(builddir_fxhash)\jsutil.o
	emcc $^ -o  $@ -lembind -O2 -std=c++20  -sENVIRONMENT=web --closure 1 

#Background Thread
$(builddir_fxhash)\main-background.o:
	emcc hosts\fxhash\main-background.cpp -std=c++20 -c -o $@ -Oz -Wall -Wpedantic -Wextra

$(htmldir_fxhash)\main-background-cpp.js : $(builddir_fxhash)\main-background.o $(builddir_fxhash)\jsutil.o
	emcc $^ -o  $@ -lembind -O2 -std=c++20  -sENVIRONMENT=web --closure 1 

#Render Worker Thread
$(htmldir_fxhash)\main-render-worker-cpp.js : $(builddir_fxhash)\main-render-worker.o $(builddir_fxhash)\jsutil.o
	emcc $^ -o  $@ -lembind -O2 -std=c++20  -sENVIRONMENT=worker --closure 1 

$(builddir_fxhash)\main-render-worker.o: hosts\fxhash\main-render-worker.cpp projects\watercolour-texture\renderer.h $(common_depend) 
	emcc hosts\fxhash\main-render-worker.cpp -std=c++20 -c -o $@ -O2 -Wall -Wpedantic -Wextra

#Common
$(builddir_fxhash)\jsutil.o: hosts\fxhash\jsutil.cpp hosts\fxhash\jsutil.h 
	emcc hosts\fxhash\jsutil.cpp -c -std=c++20 -o $@  -Oz -Wall  -Wextra

#Directories
$(htmldir_fxhash):
	mkdir $@
$(builddir_fxhash):
	mkdir $@







clean:
	rmdir /s /q $(builddir)