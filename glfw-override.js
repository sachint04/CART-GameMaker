Module = Module || {};

Module.postRun = Module.postRun || [];
Module.postRun.push(function () {

  if (!window.GLFW) return;

   window.removeEventListener("keydown", GLFW.onKeydown, true);
 
  const originalOnKeydown = GLFW.onKeydown;

  const customKeydown = function(event) {
    const target = event.target;
    const tag = target && target.tagName;
    const isEditable = tag === "INPUT" || tag === "TEXTAREA" || tag == "SHARE-CARD-ELEMENT" || (target && target.isContentEditable);
    // If typing inside popup input → DO NOT block Backspace
    if (isEditable) {
      // Still notify GLFW so game receives keys if needed
      GLFW.onKeyChanged(event.keyCode, 1);
      return;
    }
    // Otherwise use original behavior
    originalOnKeydown(event);
  };
  window.addEventListener("keydown", customKeydown, true);
 
});