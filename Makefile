#********************************************************************************************************
#
#Authors:		(c) 2022 Maths Town
#
#Licence:		The MIT License
#
#*********************************************************************************************************
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
#associated documentation files (the "Software"), to deal in the Software without restriction, including
#without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
#following conditions:

#The above copyright notice and this permission notice shall be included in all copies or substantial
#portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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