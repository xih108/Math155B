/*
*
* RayTrace Software Package, release 4.beta, May 2018.
*
* Author: Samuel R. Buss
*
* Software accompanying the book
*		3D Computer Graphics: A Mathematical Introduction with OpenGL,
*		by S. Buss, Cambridge University Press, 2003.
*
* Software is "as-is" and carries no warranty.  It may be used without
*   restriction, but if you modify it, please change the filenames to
*   prevent confusion between different versions.  Please acknowledge
*   all use of the software in any publications or products based on it.
*
* Bug reports: Sam Buss, sbuss@ucsd.edu.
* Web page: http://math.ucsd.edu/~sbuss/MathCG
*
*/


// These libraries are needed to link the program.
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")
//#pragma comment(lib,"glew3.lib")

#include <windows.h>
// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <stdio.h>
#include "../OpenglRender/OpenGLrender.h"
#include "../Graphics/PixelArray.h"
#include "../RayTraceMgr/SceneDescription.h"
#include "../RaytraceMgr/LoadNffFile.h"
#include "../RaytraceMgr/LoadObjFile.h"
#include "../DataStructs/KdTree.h"
#include "RayTraceSetup2.h"
#include "RayTraceSetup155B.h"
#include "RayTraceKd.h"

bool check_for_opengl_errors();

// Window size and pixel array variables
bool WindowMinimized = false;
int WindowWidth = -2;	// Width in pixels
int WindowHeight = -2;	// Height in pixels
PixelArray* pixels;		// Array of pixels
unsigned int TheFrameBuffer = 0;    // Opengl Frame Buffer handle
unsigned int TheFrameTexture = 0;   // Texture map to hold the frame buffer contents
GLFWwindow* WindowID;

bool NeedsRayTracing = true;   // Set if the scene has changed and needs to be re-raytraced.
bool NeedsRerendering = true;   // Set if the scene needs to rerendered in OpenGL
bool RayTraceMode = false;		// Set true for RayTraciing,  false for rendering with OpenGL
                                // Rendering with OpenGL does not support all features, esp., texture mapping
OpenglRenderer OpenglDraw;

SceneDescription* ActiveScene;
KdTree* ActiveKdTree;

SceneDescription FileScene;			// Scene that is loaded from an .obj or .nff file.

const double fovy = 0.35;           // Field of view "y"
CameraView SavedCameraView;

// myRenderScene() chooses between using OpenGL or ray-tracing to render the scene
void myRenderScene()
{
    if (WindowWidth > 0 && WindowHeight > 0 && NeedsRerendering) {
        if (RayTraceMode) {
            if (NeedsRayTracing) {
                // Here the actual ray tracing of the whole scene is done.
                double rtTime = glfwGetTime();

                RayTraceView(*ActiveScene, *ActiveKdTree, *pixels);                     // Render with ray tracing

                rtTime = glfwGetTime() - rtTime;
                printf("  Ray Trace time: %0.4f seconds.\n", rtTime);
                NeedsRayTracing = false;
            }
            // Blit ray tracing results from "pixels" from the draw frame buffer to the screen's frame buffer.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glBindTexture(GL_TEXTURE_2D, TheFrameTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixels->GetWidth(), pixels->GetHeight(), 0, GL_RGB, GL_FLOAT, pixels->GetColorBuffer());
            glBindFramebuffer(GL_READ_FRAMEBUFFER, TheFrameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
            glBlitFramebuffer(0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

            glfwSwapBuffers(WindowID);
            check_for_opengl_errors();
        }
        else {
            OpenglDraw.RenderScene();           // Render with OpenGL
            glfwSwapBuffers(WindowID);
            check_for_opengl_errors();
        }
    }
    NeedsRerendering = false;
}

void InitializeSceneGeometry()
{
// Define the lights, materials, textures and viewable objects.
// One of the following four lines should un-commented to select the way
//		the scene is loaded into the SceneDescription.
#define MODE 0  /* Use this line to manually set the scene in RayTraceSetup155B.cpp */
//#define MODE 1  /* Use this line to manually set the scene in RayTraceSetup2.cpp */
//#define MODE 2  /* Use this line to load the scene data from an .obj file. */
//#define MODE 3  /* Use this line to load the scene data from a .nff file. */
#if MODE==0
    SetupScene155B();
    ActiveScene = &TheScene155B;
#elif MODE==1
    SetUpScene2();
    ActiveScene = &TheScene2;
#elif MODE==2
    LoadObjFile("f15.obj", FileScene);
    ActiveScene = &FileScene;
    // The next lines specify scene attributes not given in the obj file.
    ActiveScene->SetBackGroundColor(0.0, 0.0, 0.0);
    ActiveScene->SetGlobalAmbientLight(0.6, 0.6, 0.2);
    CameraView& theCV = ActiveScene->GetCameraView();
    theCV.SetPosition(0.0, 0.0, 40.0);
    theCV.SetScreenDistance(40.0);
    theCV.SetScreenDimensions(20.0, 20.0);
    SetUpLights(*ActiveScene);
    // You may add more scene elements here if you wish
#else
    LoadNffFile("jacks_5_1.nff", FileScene);
    ActiveScene = &FileScene;
    // The NFF file includes camera view information, no need to add it here.
    // You may add more scene elements here if you wish
#endif

    // Build the kd-Tree.
    double kdTime = glfwGetTime();
    ActiveKdTree = &myBuildKdTree(*ActiveScene);
    kdTime = glfwGetTime() - kdTime;
    printf("       kd-Tree build time %0.4f seconds.\n", kdTime);
    if (!ActiveScene->GetCameraView().CameraViewHasBeenSet()) {
        ActiveScene->GetCameraView().SetFromAABB(ActiveKdTree->GetBoundingBox(), fovy);
    }
    SavedCameraView = ActiveScene->GetCameraView();
    pixels = new PixelArray(10, 10);		// Array of pixels
    ActiveScene->GetCameraView().SetScreenPixelSize(*pixels);
    ActiveScene->RegisterCameraView();

    // Initialize the OpenGL renderer. Pre-load all objects into VBO/EBO storage.
    OpenglDraw.InitScene(*ActiveScene);
}

//  *********************************************************
// Framebuffer resize events.
// Allocate a buffer for the pixel array for the ray traced image.
// **********************************************************
void window_size_callback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);		// Draw into entire window
    if (width == 0 || height == 0) {
        return;     // Window has been minimized
    }
    if (WindowWidth == width && WindowHeight == height) {
        return;
    }
    WindowWidth = width;
    WindowHeight = height;
    pixels->SetSize(WindowWidth, WindowHeight);

    // Resize the camera viewpoint
    ActiveScene->CalcNewScreenDims((double)WindowWidth / (double)WindowHeight);
    ActiveScene->GetCameraView().SetScreenPixelSize(*pixels);

    OpenglDraw.InitView(*ActiveScene);;

    NeedsRayTracing = true;
    NeedsRerendering = true;
    RayTraceMode = false;

    assert(TheFrameBuffer != 0);
    glBindTexture(GL_TEXTURE_2D, TheFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowWidth, WindowWidth, 0, GL_RGB, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    check_for_opengl_errors();
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************
void key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods) {
    if (action == GLFW_RELEASE) {
        return;         // Ignore key up (key release) events
    }
    switch (key) {
    case 'G':
        if (mods&GLFW_MOD_SHIFT) {
            // Render in OpenGL mode
            NeedsRerendering = RayTraceMode;    // Switching from RayTrace mode to OpenGL mode
            RayTraceMode = false;
            return;
        }
        // Fall through if lower-case 'g'
    case GLFW_KEY_SPACE:
        // Render in ray trace mode
        NeedsRerendering = !RayTraceMode;   // Switching from OpenGL mode to RayTrace mode
        RayTraceMode = true;
        return;
    case GLFW_KEY_R:
        // Reset view
        ActiveScene->GetCameraView() = SavedCameraView;
        ActiveScene->CalcNewScreenDims((double)WindowWidth / (double)WindowHeight);
        ActiveScene->GetCameraView().SetScreenPixelSize(*pixels);
        break;
    case GLFW_KEY_UP:
        if (mods&GLFW_MOD_SHIFT) {
            ActiveScene->GetCameraView().MoveCameraView(VectorR3(0.0, -0.1, 0.0));
        }
        else {
            ActiveScene->GetCameraView().RotateViewUp(0.1);
        }
        break;
    case GLFW_KEY_DOWN:
        if (mods&GLFW_MOD_SHIFT) {
            ActiveScene->GetCameraView().MoveCameraView(VectorR3(0.0, 0.1, 0.0));
        }
        else {
            ActiveScene->GetCameraView().RotateViewUp(-0.1);
        }
        break;
    case GLFW_KEY_RIGHT:
        if (mods&GLFW_MOD_SHIFT) {
            ActiveScene->GetCameraView().MoveCameraView(VectorR3(-0.1, 0.0, 0.0));
        }
        else {
            ActiveScene->GetCameraView().RotateViewRight(0.1);
        }
        break;
    case GLFW_KEY_LEFT:
        if (mods&GLFW_MOD_SHIFT) {
            ActiveScene->GetCameraView().MoveCameraView(VectorR3(0.1, 0.0, 0.0));
        }
        else {
            ActiveScene->GetCameraView().RotateViewRight(-0.1);
        }
        break;
    case GLFW_KEY_HOME:
        ActiveScene->GetCameraView().MoveToAndFrom(1.1);
        break;
    case GLFW_KEY_END:
        ActiveScene->GetCameraView().MoveToAndFrom(0.9);
        break;
    default:
        return;
    }
    RayTraceMode = false;
    NeedsRerendering = true;
    NeedsRayTracing = true;
    OpenglDraw.InitView(*ActiveScene);
}

// *******************************************************
// Process all mouse button events.  - Mostly for debugging
// *******************************************************
void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        printf("Mouse click at: %d, %d.\n", (int)xpos, (int)ypos);
    }
}


void my_setup_OpenGL() {
    // None of these options are used for ray tracing
    glEnable(GL_DEPTH_TEST);	// Enable depth buffering
    glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    // Allocate a frame buffer to hold the screen contents
    // Attach a texture map to the frame buffer
    glGenFramebuffers(1, &TheFrameBuffer);
    glGenTextures(1, &TheFrameTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, TheFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, TheFrameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TheFrameTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void setup_callbacks() {
    // Set callback function for resizing the window (more precisely: resizing the framebuffer).
    glfwSetFramebufferSizeCallback(WindowID, window_size_callback);

    // Set callback for key up/down/repeat events
    glfwSetKeyCallback(WindowID, key_callback);

    // Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
    // glfwSetCursorPosCallback(WindowID, cursor_pos_callback);
    glfwSetMouseButtonCallback(WindowID, mouse_button_callback);
}

int main() {
    glfwInit();
    // Use next four lines for Mac OS
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GL_ALPHA_BITS, 0);       // Do not need an alpha channel.

    const int initWidth = 300;
    const int initHeight = 240;
    WindowID = glfwCreateWindow(initWidth, initHeight, "RayTraceKd", NULL, NULL);
    if (WindowID == NULL) {
        printf("Failed to create GLFW window!\n");
        return -1;
    }
    glfwMakeContextCurrent(WindowID);
    if (GLEW_OK != glewInit()) {
        printf("Failed to initialize GLEW!.\n");
        return -1;
    }

    // Print info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
#ifdef GL_SHADING_LANGUAGE_VERSION
    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
    printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));

    printf("------------------------------\n");
    fprintf(stdout, "Press 'g' or space bar to start ray tracing. (And then wait!)\n");
    fprintf(stdout, "Press 'G' to return to OpenGL render mode.\n");
    fprintf(stdout, "Arrow keys change view orientation (and use OpenGL).\n");
    fprintf(stdout, "SHIFT + Arrow keys translate the scene (and use OpenGL).\n");
    fprintf(stdout, "Home/End keys alter view distance --- resizing keeps it same view size.\n");

    setup_callbacks();

    // Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
    InitializeSceneGeometry();
    window_size_callback(WindowID, initWidth, initHeight);

    // Loop while program is not terminated.
    while (!glfwWindowShouldClose(WindowID)) {
        myRenderScene();				// Ray Trace or Render as needed
        glfwWaitEvents();				// Use this if no animation.
    }

    glfwTerminate();
    return 0;
}


// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[9][36] = {
    "Unknown OpenGL error",
    "GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
    "GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
    "GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW", "GL_CONTEXT_LOST" };
bool check_for_opengl_errors() {
    int numErrors = 0;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        numErrors++;
        int errNum = 0;
        switch (err) {
        case GL_INVALID_ENUM:
            errNum = 1;
            break;
        case GL_INVALID_VALUE:
            errNum = 2;
            break;
        case GL_INVALID_OPERATION:
            errNum = 3;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errNum = 4;
            break;
        case GL_OUT_OF_MEMORY:
            errNum = 5;
            break;
        case GL_STACK_UNDERFLOW:
            errNum = 6;
            break;
        case GL_STACK_OVERFLOW:
            errNum = 7;
            break;
        case GL_CONTEXT_LOST:
            errNum = 8;
            break;
        }
        printf("OpenGL ERROR: %s.\n", errNames[errNum]);
    }
    return (numErrors != 0);
}

