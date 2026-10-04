CART is a game framework on top of "Raylib" ( www.raylib.com). 

Checkout my page
https://sachint04.github.io/CART-GameMaker/

its a framework provides advance component layer cross platform games/simulations quickly. Currently framework supports Raylib. In future more Engine supports will be added.
  
    Instruction to compile and install
    
	#1  run->
		windows -
		download ninja complier and copy ninja-win.exe (for windows) in the root of emsdk folder. 
		Mac
		Install ninja and set ninja to environment path
  
	#2 run  ->  
		cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web -DCMAKE_TOOLCHAIN_FILE="<LOCAL FOLER>/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
  
	# 3. Compile the project
		cmake --build build --config Release

	# 4. Install the final outputs
		cmake --install build --config Release
