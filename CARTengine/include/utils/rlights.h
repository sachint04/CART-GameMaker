
#ifndef RLIGHTS_H
#define RLIGHTS_H

#include "raylib.h"

#define MAX_LIGHTS  4         // Max dynamic lights supported by shader
  namespace cart{


typedef struct {
    int type;
    bool enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    float attenuation;

    // Shader locations
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int attenuationLoc;
} Light;

// Light type
typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT
} LightType;



#ifdef __cplusplus
extern "C" {
#endif

    Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);   // Create a light and get shader locations
    void UpdateLightValues(Shader shader, Light light);         // Send light properties to shader

#ifdef __cplusplus
}
#endif

#endif // RLIGHTS_H

  }


