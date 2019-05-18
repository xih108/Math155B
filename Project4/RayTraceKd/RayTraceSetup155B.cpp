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

#include "RayTraceSetup155B.h"

#include "../Graphics/Material.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Graphics/ViewableCone.h"
#include "../Graphics/ViewableTorus.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableParallelepiped.h"
#include "../Graphics/ViewableBezierSet.h"
#include "../Graphics/TextureCheckered.h"
#include "../Graphics/TextureBilinearXform.h"
#include "../Graphics/TextureSequence.h"
#include "../Graphics/Light.h"
#include "../Graphics/CameraView.h"
#include "../RaytraceMgr/SceneDescription.h"

// ******************************************
SceneDescription TheScene155B;			// The scene as created by the routines below
// ******************************************

void SetupScene155B()
{
    SetUpMaterials155B(TheScene155B);
    SetUpLights155B(TheScene155B);
    SetUpViewableObjects155B(TheScene155B);
}

void SetUpMaterials155B(SceneDescription& scene) {
    // Initialize Array of Materials

    // First material for the floor (yellow)
    Material* mat0 = new Material;
    scene.AddMaterial(mat0);
    mat0->SetColorAmbientDiffuse(0.5, 0.5, 0.0);
    mat0->SetColorSpecular(0.8, 0.8, 0.8);
    mat0->SetColorReflective(0.6, 0.6, 0.6);
    mat0->SetShininess(512);                        // Specular exponent

    // Second material for the floor (bright blue)
    Material* mat1 = new Material;
    scene.AddMaterial(mat1);
    mat1->SetColorAmbientDiffuse(0.0, 0.2, 0.8);
    mat1->SetColorSpecular(0.8, 0.8, 0.8);
    mat1->SetColorReflective(0.6, 0.6, 0.6);
    mat1->SetShininess(512);

    // Material for side walls, dark yellow
    Material* mat2 = new Material;
    scene.AddMaterial(mat2);
    mat2->SetColorAmbientDiffuse(0.3, 0.3, 0.0);
    mat2->SetColorSpecular(0.8, 0.8, 0.8);
    mat2->SetColorReflective(0.8, 0.8, 0.4);
    mat2->SetShininess(160);

    // White. Goes on back wall and a sphere under a texture map
    Material* mat3 = new Material;
    scene.AddMaterial(mat3);
    mat3->SetColorAmbientDiffuse(0.7, 0.7, 0.7);
    mat3->SetColorSpecular(0.6, 0.6, 0.6);
    mat3->SetColorReflective(0.1, 0.1, 0.1);
    mat3->SetShininess(160);
    //mat3->SetUseFresnel(true);        // Default is to not use Fresnel approximation 

    // Black, with a high gloss
    Material* mat4 = new Material;
    scene.AddMaterial(mat4); 
    mat4->SetColorAmbientDiffuse(0.0, 0.0, 0.0);
    mat4->SetColorSpecular(0.95, 0.95, 0.95);
    mat4->SetColorReflective(0.0, 0.0, 0.0);
    mat4->SetShininess(160);

    // Glass with a specular highlight
    Material* mat5 = new Material;
    scene.AddMaterial(mat5);
    mat5->SetColorAmbientDiffuse(0.0, 0.0, 0.0);
    mat5->SetColorSpecular(0.3, 0.3, 0.3);
    mat5->SetColorReflective(0.3, 0.3, 0.3);
    mat5->SetColorTransmissive(1.0, 1.0, 1.0);
    mat5->SetShininess(512);
    mat5->SetIndexOfRefraction(1.5);	// Glass!

    // A near perfect mirror
    Material* mat6 = new Material;
    scene.AddMaterial(mat6);
    mat6->SetColorAmbientDiffuse(0.05, 0.05, 0.05);
    mat6->SetColorSpecular(0.6, 0.6, 0.6);
    mat6->SetColorReflective(0.95, 0.95, 0.95);
    mat6->SetShininess(160);
}


void SetUpLights155B(SceneDescription& scene) {

    // Global ambient light and the background color are set here.
    scene.SetGlobalAmbientLight(0.2, 0.2, 0.2);
    scene.SetBackGroundColor(0.0, 0.0, 0.0);

    // Initialize Array of Lights
    // Both lights are white, but with no ambient component

    Light* myLight0 = new Light();
    scene.AddLight(myLight0);
    myLight0->SetColorAmbient(0.0, 0.0, 0.0);
    myLight0->SetColorDiffuse(1.0, 1.0, 1.0);
    myLight0->SetColorSpecular(1.0, 1.0, 1.0);
    myLight0->SetPosition(7.0, 15.0, 12.0);

    Light* myLight1 = new Light();
    scene.AddLight(myLight1);
    myLight1->SetColorAmbient(0.0, 0.0, 0.0);
    myLight1->SetColorDiffuse(1.0, 1.0, 1.0);
    myLight1->SetColorSpecular(1.0, 1.0, 1.0);
    myLight1->SetPosition(-7.0, 25.0, 12.0);

}

void SetUpViewableObjects155B(SceneDescription& scene) {

    // Fill the scene with viewable objects

    // Vertices of two rectangles
    //   Consists of three verts in CCW order (4th vertex is generated automatically to make a rectangle
    float FloorVerts[3][3] = { { -8.0f,0.0f,10.0f },{ 8.0f,0.0f,10.0f },{ 8.0f,0.0f, -10.0f } };
    float BackWallVerts[3][3] = { { -8.0f,0.0f,-10.0f },{ 8.0f,0.0f,-10.0f },{ 8.0f,10.0f,-10.0f } };

    // Vertices for the two side triangles - in CCW order
    float TriangleLeft[3][3] = { { -8.0f,0.0f,-10.0f },{ -8.0f,10.0f,-10.0f },{ -8.0f,0.0f,10.0f } };
    float TriangleRight[3][3] = { { 8.0f,0.0f,-10.0f },{ 8.0f,0.0f,10.0f },{ 8.0f,10.0f,-10.0f } };

    // Flat plane (the floor)
    ViewableParallelogram* vp;
    vp = new ViewableParallelogram();
    vp->Init(&FloorVerts[0][0]);
    vp->SetMaterial(&scene.GetMaterial(0));
    scene.AddViewable(vp);

    // Base material on floor is material 0, as set by SetMaterial
    //   Overlay a second material to make a checkerboard texture
    TextureCheckered* txchecked = scene.NewTextureCheckered();
    txchecked->SetMaterial1(&scene.GetMaterial(1));
    txchecked->SetWidths(1.0/15.0, 1.0/9.0);
    vp->TextureMap(txchecked);                      // This line adds the texture map "texchecked" to the floor ("vp")

    // Back wall

    vp = new ViewableParallelogram();
    vp->Init(&BackWallVerts[0][0]);
    vp->SetMaterial(&scene.GetMaterial(3));     // Underlying material is white
    scene.AddViewable(vp);

    TextureRgbImage* txRgb = scene.NewTextureRgbImage("BrickWallTexture.bmp");
    txRgb->SetBlendMode(TextureRgbImage::MultiplyColors);
    vp->TextureMap(txRgb);                      // Apply the RGB Image texture to the back wall.

    // Left wall (triangular)

    ViewableTriangle* vt;
    vt = new ViewableTriangle();
    vt->Init(&TriangleLeft[0][0]);
    vt->SetMaterial(&scene.GetMaterial(2));
    scene.AddViewable(vt);

    // Right wall (triangular)

    vt = new ViewableTriangle;
    vt->Init(&TriangleRight[0][0]);
    vt->SetMaterial(&scene.GetMaterial(2));
    scene.AddViewable(vt);


    // Left glass sphere
    ViewableSphere* vs;
    vs = new ViewableSphere();
    vs->SetCenter(-5.5, 2.0, 3.0);
    vs->SetRadius(2.0);
    vs->SetMaterial(&scene.GetMaterial(5));
    int leftSphereIdx = scene.AddViewable(vs);
    
    // Right texture mapped sphere
    vs = new ViewableSphere();
    vs->SetCenter(5.5, 2.0, 3.0);
    vs->SetRadius(2.0);
    vs->SetMaterial(&scene.GetMaterial(3));
    scene.AddViewable(vs);
    txRgb = scene.NewTextureRgbImage("earth_clouds.bmp");
    vs->TextureMap(txRgb);                      // Apply the RGB Image texture to the sphere.
    vs->SetuvCylindrical();		              // SetuvSpherical is the default

    // Back mirrored cylinder
    ViewableCylinder* vCyl = new ViewableCylinder();
    vCyl->SetCenter(0.0, 2.4, -4.0);
    vCyl->SetHeight(4.0);
    vCyl->SetRadius(1.5);
    vCyl->SetMaterial(&scene.GetMaterial(6));
    scene.AddViewable(vCyl);

    // Back flat black cylinder
    vCyl = new ViewableCylinder();
    vCyl->SetCenter(0.0, 0.2, -4.0);
    vCyl->SetHeight(0.4);
    vCyl->SetRadius(2.0);
    vCyl->SetMaterial(&scene.GetMaterial(4));
    scene.AddViewable(vCyl);

    // Torus
    ViewableTorus* vTorus = new ViewableTorus();
    vTorus->SetCenter(0.0, 1.0, 3.0);
    vTorus->SetRadii(2.0, 0.75);
    vTorus->SetAxis(VectorR3(0.0, 1.0, 0.0));
    vTorus->SetMaterial(&scene.GetMaterial(5));     
    scene.AddViewable(vTorus);

    // Ellipsoid
    ViewableEllipsoid* vEllip = new ViewableEllipsoid();
    vEllip->SetCenter(-6.0, 2.0, -8.0);
    vEllip->SetRadii(1.5, 0.7, 0.3);
    vEllip->SetAxes(VectorR3(-1.0, 0.0, 1.0), VectorR3(0.0,1.0,0.0));
    vEllip->SetMaterial(&scene.GetMaterial(0));
    scene.AddViewable(vEllip);

    // Use a command like the following to get a handle to an object
    // ViewableSphere* vsX = (ViewableSphere*)(&scene.GetViewable(leftSphereIdx));
 
}

