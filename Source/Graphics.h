#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Graphics {
public:
	static int fullscreen;
	static int highDPI;
	static int videoWidth, videoHeight;
	static float aspectRatio;
	static float pixelScale; // for retina iPhone

	static int glVersionMajor; // default is 3.2 core
	static int glVersionMinor;
	static bool glUseCompatibilityProfile;

	static glm::mat4 modelMatrix;
	static glm::mat4 viewMatrix;
	static glm::mat4 projectionMatrix;
	static glm::mat4 modelViewProjectionMatrix;

	static glm::mat3 normalMatrix; // = transpose(inverse(mat3(modelViewMatrix)))

	/* legacy
	static glm::vec3 translation;
	static glm::vec3 scale;
	static glm::mat3 rotation;
	 */

	enum Projection {
		Orthographic,
		Perspective
	};
	static Projection projection;
	static float fieldOfView; // in degrees

	static glm::vec4 color;
	
	static void init();
	static void destroy();
	
	static void updateProjection(); // call this whenever videoWidth, videoHeight or projectionMode changes
	static void updateModelViewProjection(); // refreshes modelViewProjectionMatrix
	
	static SDL_Window *sdlWindow;
	static SDL_GLContext sdlGLContext;
	
	static void initSDL();
	static void destroySDL();

private:
	static void ortho(float rightOrtho, float topOrtho, float nearOrtho, float farOrtho);
	static void frustum(float nearFrustum, float farFrustum, float aspectRatio);
};

#endif
