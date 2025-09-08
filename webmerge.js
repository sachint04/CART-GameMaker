mergeInto(LibraryManager.library, {
  UpdateHostAboutError: function(errorMessagePointer) {
    setErrorMessage(Module.UTF8ToString(errorMessagePointer));
  },
  GetHTTP:function(id,url,where)
  {
	
    JSHTTPGetRequest(Module.UTF8ToString(id), Module.UTF8ToString( url), Module.UTF8ToString( where));
  
  },PostHTTP:function(id,url,data,where)
  {
    JSHTTPostRequest(Module.UTF8ToString(id), Module.UTF8ToString( url), Module.UTF8ToString(data), Module.UTF8ToString( where));
  
  },Upload:function(_id, _url, imageDataPtr, w, h, filenamePtr) 
   {
	   
        // Get the image data from Emscripten's memory heap
		const id = Module.UTF8ToString(_id);
        const url = Module.UTF8ToString(_url);
		const buffersize =  w * h * 4;
        const imageData = new Uint8Array(Module.HEAPU8.buffer, imageDataPtr,buffersize);
        const filename = Module.UTF8ToString(filenamePtr);
		JSUploadImage(id, url, imageData, w, h, filename);
		

      /*  // Create a Blob or File object
        const imageBlob = new Blob([imageData], { type: 'image/png' }); // Adjust type as needed
        const imageFile = new File([imageBlob], filename, { type: 'image/png' });

        // Example: Upload using fetch API
        const formData = new FormData();
        formData.append('image', imageFile);
        formData.append('filename', filename); // Optionally send filename as a separate field
        console.log("filename ", filename, " - url ", url, " - image ", imageData);

        fetch('/upload-endpoint', {
            method: 'POST',
            body: formData
        })
        .then(response => response.json())
        .then(data => {
            console.log('Upload successful:', data);
        })
        .catch(error => {
            console.error('Upload failed:', error);
        });
     */
    }

});

