


function initializePage() {
 
}



function setErrorMessage(error) {

  const errorMessage = document.getElementById("errorMessage");
  if(!errorMessage)
  {
     errorMessage = document.createElement("div");
  }
  errorMessage.innerText = error; 
  errorMessage.style.display = (error === "" ? "none" : "");
}

function HTTPGetRequest(id, url)
{
  loadSetting(id, url);
  //GetHTTPCallback("ok", "GetHTTP request communication established!");
  
}


var loadSetting = function(_id, _url){
    $.ajax({
        url: _url,
        data: "",
        dataType: 'json',
        crossDomain: true,
        contentType: "application/json; charset=utf-8",
        success: function(data) {
           // $images = data;
           var strjson = JSON.stringify(data);
           console.log(strjson);
		   GetHTTPCallback(_id, "ok", strjson);
          // UnityContext.SendMessage('Grid', "OnSettingLoad", strjson)
        },
        error: function(error_log) {
            alert('There was some error performing the AJAX call!');
        }
    });

}



function GetHTTPCallback(id, response, data)
{
  const isValid = Module.ccall('GetHTTPCallback',
    'number',
    ['string','string', 'string'],
    [id,response, data]
  );
  return (isValid === 1);
}
