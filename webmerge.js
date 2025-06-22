mergeInto(LibraryManager.library, {
  UpdateHostAboutError: function(errorMessagePointer) {
    setErrorMessage(Module.UTF8ToString(errorMessagePointer));
  },
  GetHTTP:function(id, url)
  {
    HTTPGetRequest(Module.UTF8ToString(id), Module.UTF8ToString( url));
  }

});

