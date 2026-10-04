
class MobileKeyboard {
    static STATUS_VISIBLE = 0;
    static STATUS_DONE = 1;
    static DEBOUNCE_DELAY = 100; 

    constructor() {
        this.container = null;
        this.input = null;
        this.okButton = null;
        this.defalutText = "";
        this.placeholder = "";
        this.textCache = "";
        this.hideDelayTimer = null;        
        this.ignoreBlurEvent = false;
        this.alert = null;
        this.multiline = false;
        this.secure = false;
        this.autocorrect = false;
        this.charLimit = 255;
        this.keyboardType = 0;
        this.inputType = "text";
        this.bMobileKeyBoardToggleRequest  = false;
        this.backspcace = false;
        this.left = false;
        this.eventcount = 0;
        this.isKeyboardOn= false;
        this.holdTimeout = null;
        this.debugger = null;
    }

    /* ==============================
       Public API (called from WASM)
       ============================== */

    getIgnoreBlurEvent() {
        return this.ignoreBlurEvent;
    }

    getKeyboardStatus() {
        return this.container ? MobileKeyboard.STATUS_VISIBLE
                              : MobileKeyboard.STATUS_DONE;
    }

    getText(buffer, bufferSize) {
        const text = this.input?.value ?? this.textCache ?? "";
        if (buffer) {
            stringToUTF8(text, buffer, bufferSize);
        }
        return lengthBytesUTF8(text);
    }

    getTextSelection(outStart, outLength) {
        outStart >>= 2;
        outLength >>= 2;

        if (!this.input) {
            HEAP32[outStart] = 0;
            HEAP32[outLength] = 0;
            return;
        }

        HEAP32[outStart] = this.input.selectionStart ?? 0;
        HEAP32[outLength] =
            (this.input.selectionEnd ?? 0) - (this.input.selectionStart ?? 0);
    }

    setCharacterLimit(limit) {
        if (this.input) {
            this.input.maxLength = limit;
        }
    }

    setText(ptr) {
        if (!this.input) return;
        this.input.value = UTF8ToString(ptr);
    }

    setTextSelection(start, length) {
        if (!this.input) return;

        // iOS numeric input workaround
        if (this.input.type === "number") {
            this.input.type = "text";
            this.input.setSelectionRange(start, start + length);
            this.input.type = "number";
        } else {
            this.input.setSelectionRange(start, start + length);
        }
    }


    touchStart(textPtr, keyboardType, autocorrect, multiline, secure, alert, placeholderPtr, charLimit) {
        this.defalutText = UTF8ToString(textPtr);
      ///  window.alert(this.defalutText);
        this.keyboardType = keyboardType;
        this.autocorrect = autocorrect;
        this.multiline = multiline;
        this.secure = secure;
        this.alert = alert;
        this.placeholder = UTF8ToString(placeholderPtr);
        this.charLimit = charLimit;
        this.inputType = this.resolveInputType(keyboardType, secure);
        this.bMobileKeyBoardToggleRequest = true;
        this.isKeyboardOn = true;
        
       
       // window.console.log("touch start!");
    }

    

    interuptTouch(){
     //   window.alert("interuptTouch");
     this.hide(false);
     this.bMobileKeyBoardToggleRequest = false;
        window.console.log("interupt touch");
    }

    prepareShowKeyboard(){
        this.clearHideDelay();
       this.createUI();
    }

    hide(delay = false) {      
        if (this.hideDelayTimer){
            clearTimeout(this.hideDelayTimer);
            return;
        } 
        this.ignoreBlurEvent = true;
        const performHide = () => {
            if (this.input) {
                this.textCache = this.input.value;
            }
            this.destroyUI();

        };

        if (delay) {
            this.hideDelayTimer = setTimeout(performHide, 200);
        } else {
            performHide();
        }
        
    }

    /* ==============================
       Internal helpers
       ============================== */

    resolveInputType(keyboardType, secure) {
        if (secure) return "password";

        switch (keyboardType) {
            case 7: return "email";
            case 3: return "url";
            case 2:
            case 4:
            case 5: return "number";
            default: return "text";
        }
    }

    createUI() {
        this.container = document.createElement("div");
        this.container.style.cssText = `
            width:130px;   
            height:130px;     
            position:absolute;
            left:-200px;
            top:-200px;
            background:#fff;
            border:1px solid;                   
            overflow:hidden;
            z-index: 100000;  
            padding:10px;            
            `;

            this.input = document.createElement("textarea");
             this.input.style.cssText = `
                width:100px;
                height:100px;                 
                font-size:20px;
                resize:none;
                padding: 5px;
                border: 1px solid blue;
                text-transform: none;
            `;
        this.container.appendChild(this.input);
        document.body.appendChild(this.container);
        this.input.setAttribute('autocapitalize', 'none');
        this.configureInput();
        this.attachEvents();
        this.input.focus();
    }

    createUIiphone(){
          
        this.container = document.createElement("div");
        this.container.style.cssText = `
            width:130px;   
            height:130px;     
            position:absolute;
            left:-130px;
            top:-130px;
            background:#fff;
            border:1px solid;                   
            overflow:hidden;
            z-index: 100000;  
            padding:10px;            
            `;

            this.input = document.createElement("textarea");
             this.input.style.cssText = `
                width:100px;
                height:100px;                 
                font-size:20px;
                resize:none;
                padding: 5px;
                border: 1px solid blue;
                text-transform: none;
            `;
        this.input.setAttribute('autocapitalize', 'none');
        this.attachEvents();
        this.container.appendChild(this.input);
        document.body.appendChild(this.container);
        this.input.focus();
    }

    configureInput() {
        if(!this.multiline) this.input.type = this.inputType;    
        this.input.spellcheck = !!this.autocorrect;
        this.input.maxLength = this.charLimit > 0 ? this.charLimit : 524288;
        this.input.value = " ";
    }

    handleBackspaceKey(){
        console.log('handleBackspaceKey!!');
         let isbackspace = 1;
        const isValid = window.Module.ccall('ProcessMobileInput','number',['string','number'],["", isbackspace]); 
    }

    handleEnterKey(){
        let isbackspace = 0;
        const isValid = window.Module.ccall('ProcessMobileInput','number',['string','number'],["13", isbackspace]); 
    }

    mobileInput(str){            
        let keycode = str.charCodeAt(0);
        let isbackspace = 0; 
        const isValid = window.Module.ccall('ProcessMobileInput','number',['string', 'number'],[keycode.toString(), isbackspace]);            
    }

    attachEvents() {
        let uagent = ""+ window.useragent;
  window.mobileKeyboard.Logger(uagent);
        // For Android 
        if(window.useragent == 2)
        {
           
            if(window.mobileKeyboard.isKeyboardOn)
            {
                this.input.addEventListener('keydown', function(e)
                { 
                    
                    window.mobileKeyboard.eventcount++;
                    // check "Return" key
                    if(e.key == "Enter" || e.key == 13){
                        window.mobileKeyboard.Logger("ENTER Key presssed");
                        window.mobileKeyboard.handleEnterKey();
                        return;
                    }
                    if(!e.key || e.key === undefined || e.key === 'undefined' || e.key === 'Unidentified' || e.key === null)
                    {
                    //window.Module.ccall('LogTrace', '', ['string'],["mobile key  is Undefined! "]);

                    }else{
                    // window.Module.ccall('LogTrace', '', ['string'],["mobile key  is -> "+ e.key+" <-"]);
                        if(e.key == 'Backspace')
                        {
                            window.mobileKeyboard.Logger("Keydown -> BACKSPARE  START");
                             if (this.holdTimeout) {
                                    clearTimeout(this.holdTimeout);
                            }
                             // If this is the very first press in the sequence, trigger your custom "KeyDown"
                            if (!this.backspcace) {
                                window.mobileKeyboard.backspcace = true;
                                window.mobileKeyboard.handleBackspaceKey();
//                                this.customKeyDownAction(event);
                                 window.mobileKeyboard.Logger("Keydown -> BACKSPARE ");
                            }
                             // Set a timer. If another keydown doesn't clear this within 100ms, 
                            // it means the user lifted their finger.
                            this.holdTimeout = setTimeout(() => {
                                window.mobileKeyboard.backspcace = false;
                                 window.mobileKeyboard.Logger("KeyUp -> BACKSPARE ");
  //                              this.customKeyUpAction(event);
                            }, MobileKeyboard.DEBOUNCE_DELAY);

                        }
                    }  
                });

                
                this.input.addEventListener('input', function(e)
                {        
                    if(window.mobileKeyboard.isKeyboardOn)
                        {
                            
                            window.mobileKeyboard.eventcount++;
                            let strval = e.target.value;
                            strval = strval.slice(strval.length - 1 );                       
                            window.mobileKeyboard.input.value = ""; 
                            window.mobileKeyboard.mobileInput(strval); 
                            
                            e.preventDefault();
                            e.stopPropagation();
                        }           
                });

                // this is specially for iOS devivces where keydown event is fired only once
                this.input.addEventListener('beforeinput', (event) => {
                    // Detect if the action is a backspace loop
                    if (event.inputType === 'deleteContentBackward') {
                        window.mobileKeyboard.handleBackspaceKey();//handleActivePress('Backspace', event);
                    }
                });

            }     
        }else if (window.useragent == 1 ){
            if(window.mobileKeyboard.isKeyboardOn  )
            {
                this.input.addEventListener('keydown', function(e)
                {
                    if (e.code === 'Backspace' || e.keyCode === 8 || e.code === 229) {
                    // Call your Emscripten function to handle backspace
                        window.mobileKeyboard.Logger("Keydown -> BACKSPARE  START");
                             if (this.holdTimeout) {
                                    clearTimeout(this.holdTimeout);
                            }
                             // If this is the very first press in the sequence, trigger your custom "KeyDown"
                            if (!this.backspcace) {
                                window.mobileKeyboard.backspcace = true;
                                window.mobileKeyboard.handleBackspaceKey();
//                                this.customKeyDownAction(event);
                                 window.mobileKeyboard.Logger("Keydown -> BACKSPARE ");
                            }
                             // Set a timer. If another keydown doesn't clear this within 100ms, 
                            // it means the user lifted their finger.
                            this.holdTimeout = setTimeout(() => {
                                window.mobileKeyboard.backspcace = false;
                                 window.mobileKeyboard.Logger("KeyUp -> BACKSPARE ");
  //                              this.customKeyUpAction(event);
                            }, MobileKeyboard.DEBOUNCE_DELAY);
                        }

                });
            
                this.input.addEventListener('keyup', function(e)
                {
                //   let tststr = "Hello log trace Key Up";
                    if(window.mobileKeyboard.isKeyboardOn){

                        if (e.code === 'Backspace' || e.keyCode === 8 || e.code === 229) {
                            
                        }else{
                            
                            let strval = e.target.value;     
                            
                        }
                    }

                });
            }
        }
    
       
     //   this.input.select();
    }

    destroyUI() {
        this.input?.remove();
        this.container?.remove();
        //this.okButton?.remove();
        
        this.container = null;
        this.input = null;
       // this.okButton = null;

        this.hideDelayTimer = null;
        this.ignoreBlurEvent = false;
        this.isKeyboardOn = false

        if( this.debugger   ){
            this.debugger.remove();
        }
    }

    createDebuger(){
        this.debugger = document.createElement('textarea');
        this.debugger.style.width = '440px';
        this.debugger.style.height = '100px';
        this.debugger.style.position = 'absolute';
        this.debugger.style.border = '1px solid black'
        this.debugger.style.left = '0';
        this.debugger.style.top = '0';
        document.body.appendChild(this.debugger);
    }

    Logger(t){
         if(this.debugger == null){
            return;
            this.createDebuger();
        }
        this.debugger.value = t;
        this.debugger.scrollTop = this.debugger.scrollHeight;
    }

    clearHideDelay() {
        if (this.hideDelayTimer) {
            clearTimeout(this.hideDelayTimer);
            this.hideDelayTimer = null;
        }
    }
 
    
    
  
}

/* ==============================
   WASM / Unity WebGL bindings
   ============================== */

window.mobileKeyboard = new MobileKeyboard();

window._JS_MobileKeybard_GetIgnoreBlurEvent =
    () => mobileKeyboard.getIgnoreBlurEvent();

window._JS_MobileKeyboard_GetKeyboardStatus =
    () => mobileKeyboard.getKeyboardStatus();

window._JS_MobileKeyboard_GetText =
    (buffer, size) => mobileKeyboard.getText(buffer, size);

window._JS_MobileKeyboard_GetTextSelection =
    (start, length) => mobileKeyboard.getTextSelection(start, length);

window._JS_MobileKeyboard_SetCharacterLimit =
    limit => mobileKeyboard.setCharacterLimit(limit);

window._JS_MobileKeyboard_SetText =
    text => mobileKeyboard.setText(text);

window._JS_MobileKeyboard_SetTextSelection =
    (start, length) => mobileKeyboard.setTextSelection(start, length);

window._JS_MobileKeyboard_Start =
    (...args) => mobileKeyboard.touchStart(...args);

window._JS_MobileKeyboard_Hide =
    delay => mobileKeyboard.hide(delay);

window._JS_MobileKeyboard_InteruptTouch = () => mobileKeyboard.interuptTouch();

let lastMobileChar= 0;



/* ==============================
   Set events for Android virtual keyboard show/hide 
   ============================== */
if ("virtualKeyboard" in navigator) {
  navigator.virtualKeyboard.overlaysContent = true;

  navigator.virtualKeyboard.addEventListener("geometrychange", (event) => {
    const  rect = event.target.boundingRect;
    const iskeyboard = rect.width >  0;
    if(!iskeyboard && window.mobileKeyboard.isKeyboardOn){
        window._JS_MobileKeyboard_InteruptTouch();
        window.Module.ccall('IntruptVirtualKeyboad','number',[],[]); 
    } 
    window.mobileKeyboard.isKeyboardOn = iskeyboard;
//     window.Module.ccall('LogTrace', '', ['string'],["mobile geometrychange"+ rect.width]);
  });
}

        
	