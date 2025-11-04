mergeInto(LibraryManager.library, {
	  UpdateHostAboutError: function(errorMessagePointer) {
		setErrorMessage(Module.UTF8ToString(errorMessagePointer));
	  },
	  GetHTTP:function(id,url,where)
	  {
		
		JSHTTPGetRequest(Module.UTF8ToString(id), Module.UTF8ToString( url), Module.UTF8ToString( where));
	  
	  },
	  PostHTTP:function(id,url,data,where)
	  {
		JSHTTPostRequest(Module.UTF8ToString(id), Module.UTF8ToString( url), Module.UTF8ToString(data), Module.UTF8ToString( where));
	  
	  },
	  Upload:function(_id, _url, _dir, imageDataPtr, w, h, filenamePtr) 
	{
	   
		// Get the image data from Emscripten's memory heap
		const id = Module.UTF8ToString(_id);
		const url = Module.UTF8ToString(_url);
		const dir = Module.UTF8ToString(_dir);
		const buffersize =  w * h * 4;
		const imageData = new Uint8Array(Module.HEAPU8.buffer, imageDataPtr,buffersize);
		const filename = Module.UTF8ToString(filenamePtr);
		JSUploadImage(id, url, dir, imageData, w, h, filename);
	},
	LoadAssetHTTP:function(_id, _url)
	{
		const id = Module.UTF8ToString(_id);
		const url = Module.UTF8ToString(_url);
		//const filename = Module.UTF8ToString(_filename);
		JSLoadAsset(id, url);
	}

});

