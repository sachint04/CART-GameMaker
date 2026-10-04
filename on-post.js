if (!Module['postRun']) Module['postRun'] = [];

Module.postRun.push(function(){ 
    // Detect GPU TIER
    (async () =>  
        {
            var canvasemscripten  = document.querySelector('canvas.emscripten');        
            var w = canvasemscripten.clientWidth;
            var h = canvasemscripten.clientHeight;
            try {
                // 1. Call the library (DetectGPU is the global object)
                const gpuData = await DetectGPU.getGPUTier(1, true);
                const tier = gpuData.tier || 1; // Default to 1 for safety
                
                // 2. Allocate 4 bytes on the WASM HEAP for an int
                Module.gpuTierPtr = Module._malloc(4);

                // 3. Write the tier value to that memory address
                //Module.setValue(Module.gpuTierPtr, tier, 'i32');
                Module.HEAP32[Module.gpuTierPtr >> 2] = tier;
            //	alert("Hardware Tier " + tier + " written to HEAP. \n" + gpuData.reason);
            //	console.log("Hardware Tier " + tier + " written to HEAP.");
            } catch (e) {
                console.warn("GPU Detection failed, defaulting to Tier 1", e);
            }            
            let struagnt = window.navigator.userAgent.toLocaleLowerCase();
            var useragent = 0;
            if(struagnt.search("iphone") >= 0 || struagnt.search("mac") >= 0){
                useragent = 1;
            }else if(struagnt.search("android") >= 0){
                useragent = 2;
            }
            const strinfo = '{"width":'+w+', "height":'+h+', "useragent":'+ useragent+'}';
            console.log("before main 1 ", strinfo);
            const isValid = Module.ccall('beforeMain',
            'number',
                ['string'],
                [strinfo ]
            );
            APP_INIT  = 1; 
                   
        })();
    // End

});