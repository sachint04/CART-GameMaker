/// Instructions:: Include this functions in main html in the published package
		/// <summary>
		/// Post Ajax request from emscripten Module
		/// </summary>
		/// <param name="id" datatype="string>unique call back reference function in c++ code , will be passed back to callback handler as an identifier</param>
		/// <param name="url" datatype="string">location of resource</param>
		/// <param name="data">payload</param>
		/// <param name="where">addition data for SQL query</param>
	   var JSHTTPostRequest = function(id, url , data, where= "")
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
				   let json = JSON.parse(res);
				  // console.log(data);
				  const status =  json["status"];
				  const message =  json["message"];				 
				   GetHTTPCallback(id, status, message);
				},
				error: function(error_log) {
					alert('There was some error performing the AJAX call!');
				}
			});
        }
		/// <summary>
		/// GET Ajax  request from emscripten Module
		/// </summary>
		/// <param name="id" datatype="string>string>unique call back reference function in c++ code, will be passed back to callback handler as an identifier</param>
		/// <param name="url" datatype="string">location of resource</param>		
		/// <param name="where">addition data for SQL query</param>
    	var JSHTTPGetRequest =  function(id, url, where= "")
		{
			console.log("JSHTTPGetRequest ",id, url, where);
		  getdData(id, url, where); 
		}
		function findCookieValFor(_cookie, _param){
			let l =  _cookie.split(';');
			for(let i = 0; i< l.length;i++)
			{
				let param = l[i].split('=')[0];
				let val = l[i].split('=')[1];
				if(param == _param)
				{
					return val;
				}   
			}
			return "";
		}
		var getdData = function(_id, _url, _where ){
			if(_where != ""){
				_url +="?"+ _where;
			}
			$.ajax({
				url: _url,
				data:"",
				dataType: 'json',
				crossDomain: true,
				contentType: "application/json; charset=utf-8",
				success: function(data) {
				   var strjson = JSON.stringify(data);
				   GetHTTPCallback(_id, "ok", strjson);
				},
				error: function(error_log) {
					alert('There was some error performing the AJAX call!');
				}
			});
		}
		/// <summary>
		/// Upload Image to server
		/// </summary>
		/// <param name="id" datatype="string>string, will be passed back to callback handler as an identifier</param>
		/// <param name="url" datatype="string">location of resource</param>		
		/// <param name="data" datatype="UTF8ToString">image data converted from c++ unint pointer</param>
		/// <param name="where">addition data for SQL query</param>
        function JSUploadImage(id, url, imageData, w, h, fname)
		{
			
			const imageDataArray = new Uint8Array(imageData, w * h * 4);
			let filename = fname; // Desired filename	
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
				data: {
					'image':u,
					'filename': filename,
				},
				success: function(response) {
					let jsonresponse = JSON.parse(response);
					//console.log("JSUploadImage success - ",jsonresponse['filepath']);//$("#message").html(response);
					canvas.remove();
					PostHTTPCallback(id, "ok", jsonresponse['filepath']);
				},
				error: function(xhr, status, error) {
					console.log("JSUploadImage Error! ", status, error);
					$("#message").html("Error uploading file: " + error);
					canvas.remove();
					 GetHTTPCallback(id, "error", error);
				}
			});
		}
		/// <summary>
		/// Download assets/file i.e. image, models, audio etc Image from server
		/// </summary>
		/// <param name="id" datatype="string>string, will be passed back to callback handler as an identifier</param>
		/// <param name="fileUrl" datatype="string">location of resource</param>		
		/// <param name="fileName" datatype="UTF8ToString">filename will be sent back to c++ code for as file identifier</param>
		async function JSLoadAsset(id, fileUrl, fileName )
		{
			try {
				const response = await fetch(fileUrl);
				if (!response.ok) {
					console.error('HTTP error!', response.statusText);
					GetHTTPCallback(id, "failed", response.statusText);
				}
				const blob = await response.blob();				
				const conv = ConvertBlobToByteArray(blob)
				.then(byteArray => {
					if (byteArray) {
						 const byteLength = byteArray.byteLength;
						//  for(var i =0 ;i<byteLength;i++)
						//  {
						// 	console.log("blob data ", byteArray[i]);
						//  }
					   // Allocate memory on the Emscripten heap
						const ptr = Module._malloc(byteArray.length);
						//s Copy data to the Emscripten heap
						Module.HEAPU8.set(byteArray, ptr);
						// Call the C++ function
						 const isValid = Module.ccall('ProcessByteArray',
						 'number',
						 ['string','string', 'number','number'],
						 [id,fileName, ptr, byteLength]);

						//Module.ProcessByteArray(id, filename, ptr, byteLength);
						// Free the allocated memory
						Module._free(ptr);
					//	const s = new TextDecoder().decode(byteArray);
						//console.log(s);
					//GetHTTPCallback(id, "ok", s);
					}
				});

				if(conv == null)
					{
					const errmsg = "Error converting image Blob to byte array";
					GetHTTPCallback(id, "failed", errmsg);
				}
			} catch (error) {
				console.error('Download failed:', error);
				GetHTTPCallback(id, "failed", error.log);
			}

		}
		// call back function to c++
		function GetHTTPCallback(id, response, data)
		{
		  const isValid = Module.ccall('GetHTTPCallback',
			'number',
			['string','string', 'string'],
			[id,response, data]
		  );
		  return (isValid === 1);
		}
		// call back function to c++
        function PostHTTPCallback(id, response, data)
		{
		  const isValid = Module.ccall('PostHTTPCallback',
			'number',
			['string','string', 'string'],
			[id,response, data]
		  );
		  return (isValid === 1);
		}
		// Helper function
		async function ConvertBlobToByteArray(imageBlob) {
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
</script>