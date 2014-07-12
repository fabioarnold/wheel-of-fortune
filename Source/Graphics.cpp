#include "Graphics.h"

int Graphics::fullscreen = 0;
int Graphics::highDPI = 0;
int Graphics::videoWidth = 640;
int Graphics::videoHeight = 480;
float Graphics::aspectRatio = 640.0f / 480.0f;
float Graphics::pixelScale = 1.0f;

int Graphics::glVersionMajor = 3;
int Graphics::glVersionMinor = 2;
bool Graphics::glUseCompatibilityProfile = false;

Graphics::Projection Graphics::projection = Graphics::Projection::Orthographic;
float Graphics::fieldOfView = 90.0f;

// initialize to identity
glm::mat4 Graphics::modelMatrix = glm::mat4(1.0f);
glm::mat4 Graphics::viewMatrix = glm::mat4(1.0f);
glm::mat4 Graphics::projectionMatrix = glm::mat4(1.0f);

glm::mat4 Graphics::modelViewProjectionMatrix = glm::mat4(1.0f);

glm::mat3 Graphics::normalMatrix = glm::mat3(1.0f);

glm::vec4 Graphics::color = glm::vec4(1.0f);

void Graphics::init() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void Graphics::destroy() {
}

/*
void printMatrix(const glm::mat4 &matrix) {
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++)
			printf("%f, ", matrix[row][col]);
		putchar('\n');
	}
}
 */

void Graphics::updateProjection() {
	if (projection == Projection::Orthographic) {
		float rightOrtho = (float)(videoWidth >> 1);
		float topOrtho = (float)(videoHeight >> 1);
		projectionMatrix = glm::ortho(-rightOrtho, rightOrtho, -topOrtho, topOrtho, -64.0f, 64.0f);
		//ortho(rightOrtho, topOrtho, -64.0f, 64.0f);
		//translation.z = 0.0f;
	} else if (projection == Projection::Perspective) {
		static const float depth = 0x1 << 12; // some reasonable depth for the frustum
		//float nearFrustum = 0.5f;
		//float rightFrustum = 0.5f * nearFrustum * aspectRatio;
		//float topFrustum = 0.5f * nearFrustum;
		//projectionMatrix = glm::frustum(-rightFrustum, rightFrustum, -topFrustum, topFrustum, nearFrustum, 2.0f * depth);
		//frustum(0.5f, 2.0f * depth, aspectRatio);
		//translation.z = -videoHeight;

		projectionMatrix = glm::perspective(fieldOfView, aspectRatio, 0.5f, 2.0f * depth);
	}
	//updateTranslation();
	//glUniformMatrix4fv(_shaders[_currentShader].getUniformLocation(UNIFORM_PROJECTION_MATRIX), 1, GL_FALSE, *_projectionMatrix);
}

void Graphics::updateModelViewProjection() {
	// OpenGL uses columns major matrices
	// transpose rotation matrix
	/*
	modelViewMatrix[0][0] = scale[0] * rotation[0][0];
	modelViewMatrix[0][1] = scale[0] * rotation[1][0];
	modelViewMatrix[0][2] = scale[0] * rotation[2][0];
	modelViewMatrix[0][3] = 0.0f;
	modelViewMatrix[1][0] = scale[1] * rotation[0][1];
	modelViewMatrix[1][1] = scale[1] * rotation[1][1];
	modelViewMatrix[1][2] = scale[1] * rotation[2][1];
	modelViewMatrix[1][3] = 0.0f;
	modelViewMatrix[2][0] = scale[2] * rotation[0][2];
	modelViewMatrix[2][1] = scale[2] * rotation[1][2];
	modelViewMatrix[2][2] = scale[2] * rotation[2][2];
	modelViewMatrix[2][3] = 0.0f;
	modelViewMatrix[3][0] = translation[0];
	modelViewMatrix[3][1] = translation[1];
	modelViewMatrix[3][2] = translation[2];
	modelViewMatrix[3][3] = 1.0f;
	*/
	//glUniformMatrix4fv(_shaders[_currentShader].getUniformLocation(UNIFORM_MODELVIEW_MATRIX), 1, GL_FALSE, * _modelViewMatrix);

	modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
}

void Graphics::ortho(float rightOrtho, float topOrtho, float nearOrtho, float farOrtho) {
	projectionMatrix[0][0] = 1.0f / rightOrtho;
	projectionMatrix[0][1] = 0.0f;
	projectionMatrix[0][2] = 0.0f;
	projectionMatrix[0][3] = 0.0f;
	projectionMatrix[1][0] = 0.0f;
	projectionMatrix[1][1] = 1.0f / topOrtho;
	projectionMatrix[1][2] = 0.0f;
	projectionMatrix[1][3] = 0.0f;
	projectionMatrix[2][0] = 0.0f;
	projectionMatrix[2][1] = 0.0f;
	projectionMatrix[2][2] = -2.0f / (farOrtho - nearOrtho);
	projectionMatrix[2][3] = -(farOrtho + nearOrtho) / (farOrtho - nearOrtho);
	projectionMatrix[3][0] = 0.0f;
	projectionMatrix[3][1] = 0.0f;
	projectionMatrix[3][2] = 0.0f;
	projectionMatrix[3][3] = 1.0f;
}

void Graphics::frustum(float nearFrustum, float farFrustum, float aspectRatio) {
	// assume leftFrustum == -rightFrustum and bottomFrustum == -topFrustum
	float rightFrustum = 0.5f * nearFrustum * aspectRatio;
	float topFrustum = 0.5f * nearFrustum;
	projectionMatrix[0][0] = nearFrustum / rightFrustum;
	projectionMatrix[0][1] = 0.0f;
	projectionMatrix[0][2] = 0.0f;
	projectionMatrix[0][3] = 0.0f;
	projectionMatrix[1][0] = 0.0f;
	projectionMatrix[1][1] = nearFrustum / topFrustum;
	projectionMatrix[1][2] = 0.0f;
	projectionMatrix[1][3] = 0.0f;
	projectionMatrix[2][0] = 0.0f;
	projectionMatrix[2][1] = 0.0f;
	projectionMatrix[2][2] = -(farFrustum + nearFrustum) / (farFrustum - nearFrustum);
	projectionMatrix[2][3] = -(2.0f * farFrustum * nearFrustum) / (farFrustum - nearFrustum);
	projectionMatrix[3][0] = 0.0f;
	projectionMatrix[3][1] = 0.0f;
	projectionMatrix[3][2] = -1.0f;
	projectionMatrix[3][3] = 0.0f;
}

/*
void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam) {
	printf("Debug callback :D\n");
}
*/

SDL_Window *Graphics::sdlWindow = nullptr;
SDL_GLContext Graphics::sdlGLContext = nullptr;

void Graphics::initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "ERROR: Failed to init SDL Video\n");
		exit(1);
	}
	
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// request OpenGL 3.2 core profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glVersionMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glVersionMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glUseCompatibilityProfile ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	
	sdlWindow = SDL_CreateWindow("Isolation Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)(videoWidth * pixelScale), (int)(videoHeight * pixelScale), SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (Graphics::highDPI ? SDL_WINDOW_ALLOW_HIGHDPI : 0) | (fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE));
	if (sdlWindow == nullptr) {
		fprintf(stderr, "ERROR: Failed to create an OpenGL Window\n");
		exit(1);
	}
	
	// create gl context
	sdlGLContext = SDL_GL_CreateContext(sdlWindow);

	// do some windows specific initialization
#ifdef _WIN32
	glewExperimental = GL_TRUE; // needed for opengl 3.2 core profile context
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "ERROR: GLEW initialization failed\n");
		exit(1);
	}
#endif

	if (Graphics::highDPI) {
		int drawableWidth, drawableHeight;
		SDL_GL_GetDrawableSize(sdlWindow, &drawableWidth, &drawableHeight);
		if (drawableWidth != videoWidth || drawableHeight != videoHeight) {
			printf("Created high DPI window\n");
			pixelScale = (float)drawableWidth / (float)videoWidth;
		}
	}
	
	SDL_GL_SetSwapInterval(1); // sync with monitor refresh rate

	/*
	glDebugMessageCallback(glDebugCallback, nullptr);
	GLuint unusedIds = 0;
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	 */
}

void Graphics::destroySDL() {
	SDL_DestroyWindow(sdlWindow);
	SDL_GL_DeleteContext(sdlGLContext);
	SDL_Quit();
}
