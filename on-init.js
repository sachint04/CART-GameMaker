if (typeof Module === 'undefined') Module = {};
Module['onRuntimeInitialized'] = function() {
    console.log("🚀 WebAssembly Runtime is fully initialized and memory is ready!");
    var spinnercontainer = document.querySelector('#spinnercontainer');
    var spinnerLbl = document.querySelector('#spinner-lbl');
    spinnercontainer.style.display = 'none';
    var resizeTimer;
    var initialHeight = window.innerHeight;
    var fileInput = document.getElementById('fileInput');


	  
    	window.JSHTTPostRequest = function(id, url , data, where= "")
        {

	        $.ajax({
				url: url,
                type: "POST",
				data:{
					'data':data,					
				},
			//	 dataType: 'json',
			//	 crossDomain: true,
			//	 contentType: "charset=utf-8",
				success: function(res) {
				  // let json = JSON.parse(res);
				  // console.log(data);
				  const status =  res["status"];
				  //const message =  res["message"];				 
				   GetHTTPCallback(id, status, "success!");
				},
				error: function(error_log) {
					alert('There was some error performing the AJAX call!', error_log.error);

				}
			});
        }

    	window.JSHTTPGetRequest =  function(id, url, where= "")
		{
//			console.log("JSHTTPGetRequest ",id, url, where);
		  getdData(id, url, where); 
		}

		window.getdData = function(_id, _url, _where ){
			if(_where != ""){
				_url +="?"+ _where;
			}
					
			$.ajax({
				url: _url,
				data:"",
				crossDomain: true,
				contentType: "application/json; charset=utf-8",
				xhr: function() {
					var xhr = $.ajaxSettings.xhr(); // Get the native XHR object

					// Track upload progress if available
					if (xhr.upload) {
						xhr.upload.addEventListener('progress', function(event) {
							if (event.lengthComputable) {
								var percentComplete = (event.loaded / event.total) * 100;								
								//console.log('Upload Progress: ' + Math.floor(percentComplete) + '%');
								// Update UI here, e.g., a progress bar
							}
						}, false);
					}

					// Track download progress
					if (xhr) {
						xhr.addEventListener('progress', function(event) {
							if (event.lengthComputable) {
								var percentComplete = (event.loaded / event.total) * 100;								
								//console.log('Download Progress: ' + Math.floor(percentComplete) + '%');
								// Update UI here
							}
						}, false);
					}

					return xhr;
				},
				success: function(data) {
					var status = data["status"];
					if(status == "ok")
					{
						var json = JSON.stringify(data.payload);
						console.log(json);
					   GetHTTPCallback(_id, "ok", json);
				   	}else{
					 const error_msg = json["err"];
					 //window.location = "index.html";	
					 throw error(error_msg);
				   }
				},
				error: function(error_log) {
					var error = error_log.responseText;
					GetHTTPCallback(_id, 'failed', error);
					//alert('There was some error performing the AJAX call!');
				}
			});
		}

       window.JSUploadImage = function(id, url, dir, imageData, w, h, fname)
		{
			//if(fname == "")fname = 'pic_raw.png';
			const imageDataArray = new Uint8Array(imageData, w * h * 4);
			let canvas = document.createElement('canvas');
			canvas.width = w; // Set the desired width of the image
			canvas.height = h; // Set the desired height of the image
			canvas.style.display = 'none';
			const ctx = canvas.getContext('2d');
			const arr = new Uint8ClampedArray(w*h*4);
			for (let i = 0; i < imageData.length; i += 4) {
				arr[i + 0] = imageData[i]; // R value
				arr[i + 1] = imageData[i + 1]; // G value
				arr[i + 2] = imageData[i + 2]; // B value
				arr[i + 3] = imageData[i + 3]; // A value
			}
			let imageDatafroarr = new ImageData(arr, w);
			ctx.putImageData(imageDatafroarr, 0, 0); // (0,0) are the x,y coordinates to draw at			
 			document.querySelector('#main').append(canvas);			 
			let u = canvas.toDataURL('image/png');
			$.ajax({
				url: url,
				type: "POST",
				cache: false,
				data: {
					'image':u,
					'dir':dir,
					'filename': fname,
				},
				success: function(response) {
				//	let jsonresponse = JSON.parse(response);
					//console.log("JSUploadImage success - ",jsonresponse['filepath']);//$("#message").html(response);
					canvas.remove();
					PostHTTPCallback(id, "ok", response['filepath']);
				},
				error: function(xhr, status, error) {
					console.log("JSUploadImage Error! ", status, error);
					$("#message").html("Error uploading file: " + error);
					canvas.remove();
					 GetHTTPCallback(id, "error", error);
				}
			});
		}

		window.JSDeleteFile = function(id, url, dir, fname)
		{
			$.ajax({
				url: url,
				type: "POST",
				cache: false,
				data: {
					'dir':dir,
					'filename': fname,
				},
				success: function(response) {
					PostHTTPCallback(id, "ok", response['filepath']);
				},
				error: function(xhr, status, error) {
					console.log("JSUploadImage Error! ", status, error);
					 GetHTTPCallback(id, "error", error);
				}
			});
		}

		window.JSLoadAsset = async function(_id, _url)
		{
			const uid = _id;
			const url = _url;
			console.log('JSLoadAsset -> ', _url);
			const uint8Array = await fetchBlobAndConvertToUint8Array(url);
			if(uint8Array){
				 const length = uint8Array.byteLength;							
				// Allocate memory on the Emscripten heap
				const ptr = Module._malloc(uint8Array.length);
				//s Copy data to the Emscripten heap
				Module.HEAPU8.set(uint8Array, ptr);
				// Call the C++ function
				//try{

					const isValid = Module.ccall('ProcessByteArray',
					'number',
					['string','string', 'number','number'],
					[uid,url, ptr, length]);				
				//}catch(err){
				//	console.log('Fail to process image data ', err);
			//	}finally{
					Module._free(ptr);
			//	}						
				// Free the allocated memory
			}			
		}

		window.GetHTTPCallback = function(id, response, data)
		{
		  const isValid = Module.ccall('GetHTTPCallback',
			'number',
			['string','string', 'string'],
			[id,response, data]
		  );
		  return (isValid === 1);
		}

        window.PostHTTPCallback = function(id, response, data)
		{
		  const isValid = Module.ccall('PostHTTPCallback',
			'number',
			['string','string', 'string'],
			[id,response, data]
		  );
		  return (isValid === 1);
		}
		window.JSShowSpinnerView = function(_msg){
			if(spinnercontainer.style.display != 'block'){
				spinnercontainer.style.display = 'block';
			}			
			spinnerLbl.innerHTML = _msg;
    	}

		window.JSHideSpinnerView = function(){
			spinnercontainer.style.display = 'none';
		}
		
	
		window.fetchBlobAndConvertToUint8Array = async function(url){
			try{
				const response = await fetch(url);
				if(!response) throw new Error('Network response was not ok');

				const blob = await response.blob();
				// Get the ArrayBuffer from the Blob
				const arrayBuffer = await blob.arrayBuffer();

				// Create a Uint8Array from the ArrayBuffer
				const byteArray = new Uint8Array(arrayBuffer);
				
				return byteArray;				
			}catch(error)
			{
				console.error('Error fetching or converting blob:', error);    		
			}
		}

		window.ConvertBlobToByteArray= async function(imageBlob) {
			try {
				// Get the ArrayBuffer from the Blob
				const arrayBuffer = await imageBlob.arrayBuffer();

				// Create a Uint8Array from the ArrayBuffer
				const byteArray = new Uint8Array(arrayBuffer);

				return byteArray;
			} catch (error) {
				console.error("Error converting image Blob to byte array:", error);
				return null;
			}
		}
		
		// Function to call from C++ to trigger file selection
		 window.JSLoadFileFromDevice= function(id , format) {
			loadfromdevice_format =  format;
			var canvas = document.getElementById('canvas');
			var imgsel = document.getElementById('imageselect');
			var finput = document.getElementById('fileInput');
			
			finput.setAttribute('accept', format);
			finput.setAttribute('data-id', id);
			
			imgsel.style.width = canvas.style.width;
			imgsel.style.height = canvas.style.height;
			
			imgsel.classList.remove('hide');
			//document.getElementById('fileInput').click();
		}
		
		fileInput.addEventListener('change', function(event)
		{
			const file = event.target.files[0];
			if (file) 
				{				
				//console.log("File Selected!");
				const reader = new FileReader();
				reader.onload = function(e) {
				
					const byteArray = new Uint8Array(e.target.result);
					
					//console.log("file renderer on load | bytelength", byteArray.length);
					// Allocate memory on the Emscripten heap
					const ptr = Module._malloc(byteArray.length);
					//s Copy data to the Emscripten heap
					Module.HEAPU8.set(byteArray, ptr);
					// e.target.result contains the file data as an ArrayBuffer
					const fileName = file.name;
					// Call a C++ function to handle the image data
					// You will define 'handleImageDataFromJS' in your C++ code
					var uid = document.getElementById('fileInput').getAttribute("data-id");
					//console.log(" File Reader on load | filename ", fileName, " | uid ", uid);
				//	console.log("on File Select! ", uid);
					const isValid = Module.ccall('ProcessByteArray',
							'number',
							['string','string', 'number','number'],
							[uid,fileName, ptr, byteArray.length]);
					// Free the allocated memory
					Module._free(ptr);
				//	console.log("File loader complete!!	");
					document.getElementById('imageselect').classList.add('hide');
	
				};
				reader.readAsArrayBuffer(file);
			}else{
				const errmsg = "JSLoadFromDevice() Error! invalid filedata";
				window.GetHTTPCallback(id, "failed", errmsg);
			}
		});


		// Event listener for when a file is selected	
		var  fileInputHandler = function(event) {
			const file = event.target.files[0];
			if (file) 
			{
			//	console.log("File Selected!");
				const reader = new FileReader();
				reader.onload = function(e) {
				
					const byteArray = new Uint8Array(e.target.result);
					
					//console.log("file renderer on load | bytelength", byteArray.length);
					// Allocate memory on the Emscripten heap
					const ptr = Module._malloc(byteArray.length);
					//s Copy data to the Emscripten heap
					Module.HEAPU8.set(byteArray, ptr);
					// e.target.result contains the file data as an ArrayBuffer
					const fileName = file.name;
					// Call a C++ function to handle the image data
					// You will define 'handleImageDataFromJS' in your C++ code
					var uid = document.getElementById('fileInput').getAttribute("data-id");
				//	console.log(" File Reader on load | filename ", fileName, " | uid ", uid);
				//	console.log("on File Select! ", uid);
					const isValid = Module.ccall('ProcessByteArray',
							'number',
							['string','string', 'number','number'],
							[uid,fileName, ptr, byteArray.length]);
					// Free the allocated memory
					Module._free(ptr);
				//	console.log("File loader complete!!	");
					document.getElementById('imageselect').classList.add('hide');
					document.getElementById('canvas').classList.remove('hide');
				};
				reader.readAsArrayBuffer(file);
			}else{
				const errmsg = "JSLoadFromDevice() Error! invalid filedata";
				GetHTTPCallback(id, "failed", errmsg);
			}
		};

		window.processImage = function(imageDataUrl) {
			const img = new Image();
			img.onload = function() {
				const canvas = document.createElement('canvas');
				canvas.width = img.width;
				canvas.height = img.height;
				const ctx = canvas.getContext('2d');
				ctx.drawImage(img, 0, 0);

				const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
				const pixelData = new Uint8Array(imageData.data.buffer); // Raw pixel data
			};
			img.src = imageDataUrl;
		}

		window.preivewImage = function(data)
		{
			   const img = document.createElement('img');
                img.src = data;
                img.style.maxWidth = '100%'; // Optional: style the image
                img.style.height = 'auto';  // Optional: style the image
                imagePreview.appendChild(img);
		}

		window.addEventListener('resize', function()
		{		
		 clearTimeout(resizeTimer);
			// Set a new timeout
			resizeTimer = setTimeout(() => {
				const currentHeight = window.innerHeight;				
				if(Module && Module["ccall"] && APP_INIT == 1){
					var canvas = document.querySelector('#canvas');
					
					var w = canvas.width;
					var h = canvas.height;
					initialHeight = h;
					Module.ccall('OnStageResized','number',['number','number',],[w, h ]);
				}
				// Perform actions that should happen only once after resizing stops
			}, 250); // Adjust the delay (in milliseconds) as needed
		});
};