/*
*
* RayTrace Software Package, release 4.gamma.  May 8, 2019.
*
* Author: Sam Buss
*
* Software accompanying a possible second edition to the book
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

// Use the static library (so glew32.dll is not needed):
#include <windows.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h> 

#include "OpenglRender.h"
#include "../RaytraceMgr/SceneDescription.h"
#include "../VrMath/MathMisc.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableParallelepiped.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableTorus.h"
#include "../Graphics/ViewableTriangle.h"
#include "GlShaderMgr.h"

extern bool check_for_opengl_errors();

bool OpenglRenderer::OpenGLisSetup = false;
unsigned int OpenglRenderer::ShaderProgramRT = 0;    // Shader program handle
unsigned int OpenglRenderer::VertPos_LocRT;          // Location of vertex position in shader program
unsigned int OpenglRenderer::VertNormal_LocRT;       // Location of vertex normal in shader program.
int OpenglRenderer::ProjMat_LocRT;                   // Location of uniform projection matrix in shader program
int OpenglRenderer::ModelViewMat_LocRT;              // Location of uniform model view matrix in shader program


// Set OpenGL options.
// This will be called automatically the first time any rendering is done.
void OpenglRenderer::SetupOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 
    SetupShaderProgram();
    SetupOpenGlBuffers();
    check_for_opengl_errors();
}

void OpenglRenderer::SetupShaderProgram() {
    if (ShaderProgramRT == 0) {
        GlShaderMgr::LoadShaderSource("EduPhong.glsl");
        unsigned int shader_VPP = GlShaderMgr::CompileShader("vertexShader_PhongPhong");
        unsigned int shader_FPP = GlShaderMgr::CompileShader("fragmentShader_PhongPhong", "calcPhongLighting", "applyTextureMap");
        unsigned int shaders_PP[2] = { shader_VPP, shader_FPP };
        unsigned int phShaderRtPhong = GlShaderMgr::LinkShaderProgram(2, shaders_PP);
        GlShaderMgr::FinalizeCompileAndLink();

        phRegisterShaderProgram(phShaderRtPhong);
        ShaderProgramRT = phShaderRtPhong;
        VertPos_LocRT = phVertPos_loc;
        VertNormal_LocRT = phVertNormal_loc;
        ProjMat_LocRT = phGetProjMatLoc(phShaderRtPhong);
        ModelViewMat_LocRT = phGetModelviewMatLoc(phShaderRtPhong);
    }
    check_for_opengl_errors();
}

void OpenglRenderer::SetupOpenGlBuffers()
{
    if (theVAO != 0) {
        return;
    }
    SetupShaderProgram();
    glGenVertexArrays(1, &theVAO);
    glGenBuffers(1, &theVBO);
    glGenBuffers(1, &theEBO);

    glBindVertexArray(theVAO);
    glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);

    glVertexAttribPointer(VertPos_LocRT, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(VertPos_LocRT);
    glVertexAttribPointer(VertNormal_LocRT, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(VertNormal_LocRT);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    check_for_opengl_errors();
}

void OpenglRenderer::RenderScene()
{
    SetupOpenGL();
    float clearColor[3];
    BackgroundColor.Dump(clearColor);
    glClearBufferfv(GL_COLOR, 0, clearColor);
    const float clearDepth = 1.0f;
    glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a pointer to the depth value!

    // Set projection and modelview matrices
    glUseProgram(ShaderProgramRT);
    glBindVertexArray(theVAO);

    float matEntries[16];
    ProjectionMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(ProjMat_LocRT, 1, false, matEntries);
    ModelViewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(ModelViewMat_LocRT, 1, false, matEntries);

    // Load global light information
    GlobalLightInfo.LoadIntoShaders();

    // Load lights information (has to be done since model view matrix may have changed).
    for (int i = 0; i < (int)GlobalLightInfo.NumLights; i++) {
        phLight& thisLight = LightInfo[i];
        if (thisLight.IsDirectional) {
            thisLight.SetDirection(ModelViewMatrix, -SceneLights[i].GetPosition());
        }
        else {
            thisLight.SetPosition(ModelViewMatrix, SceneLights[i].GetPosition());
        }
        if (thisLight.IsSpotLight) {
            thisLight.SetSpotlightDirection(ModelViewMatrix, SceneLights[i].GetSpotDirection());
        }
        thisLight.LoadIntoShaders(i);
    }

    for (const RenderCommand cmd : RenderCmds) {
        if (cmd.numTriangles < 0) {
            break;
        }
        const MaterialBase& sceneMat = *cmd.materialFront;
        phMaterial newMat;
        newMat.AmbientColor = sceneMat.GetColorAmbient();
        newMat.DiffuseColor = sceneMat.GetColorDiffuse();
        newMat.SpecularColor = sceneMat.GetColorSpecular();
        newMat.EmissiveColor = sceneMat.GetColorEmissive();
        newMat.SpecularExponent = (float)sceneMat.GetPhongShininess();
        newMat.LoadIntoShaders();
        glDrawElements(GL_TRIANGLES, 3 * cmd.numTriangles, GL_UNSIGNED_INT, (void*)(cmd.startElement*sizeof(unsigned int)));
    }
    
    glUseProgram(0);
    check_for_opengl_errors();
} 


// Use for initialization
// meshResolution - desired mesh resolution
// meshFlats = true if desired to mesh triangles too.
// InitScene() or InitGeometries() must be called after calling this
void OpenglRenderer::SetMeshCounts(int meshRes, bool meshFlats)
{
    MeshRes = Min(1, meshRes);
    MeshFlats = meshFlats;

    // Prebuild the VBO and EBO for a generic sphere, cylinder and torus.
    
    BaseSphere.Remesh(meshRes, meshRes);
    BaseSphereNumElts = BaseSphere.GetNumElements();
    BaseSphereNumVerts = BaseSphere.GetNumVerticesNoTexCoords();
    delete[] BaseSphereVerts;
    BaseSphereVerts = new float[3 * BaseSphereNumVerts];
    delete[] BaseSphereElts;
    BaseSphereElts = new unsigned int[BaseSphereNumElts];
    BaseSphere.CalcVboAndEbo(BaseSphereVerts, BaseSphereElts, 0, -1, -1, 3);

    BaseCylinder.Remesh(meshRes, meshRes, meshRes);
    BaseCylinderNumElts = BaseCylinder.GetNumElements();
    BaseCylinderNumVerts = BaseCylinder.GetNumVerticesNoTexCoords();
    BaseCylinderDiskNumVerts = BaseCylinder.GetNumVerticesDisk();
    delete[] BaseCylinderVerts;
    BaseCylinderVerts = new float[6 * BaseCylinderNumVerts];
    delete[] BaseCylinderElts;
    BaseCylinderElts = new unsigned int[BaseCylinderNumElts];
    BaseCylinder.CalcVboAndEbo(BaseCylinderVerts, BaseCylinderElts, 0, 3, -1, 6);

    BaseTorus.Remesh(meshRes, meshRes);         // Major radius is 1.0; minor radius is 0.5
    BaseTorusNumElts = BaseTorus.GetNumElements();
    BaseTorusNumVerts = BaseTorus.GetNumVerticesNoTexCoords();
    delete[] BaseTorusVerts;
    BaseTorusVerts = new float[6 * BaseTorusNumVerts];
    delete[] BaseTorusElts;
    BaseTorusElts = new unsigned int[BaseTorusNumElts];
    BaseTorus.CalcVboAndEbo(BaseTorusVerts, BaseTorusElts, 0, 3, -1, 6);
}

void OpenglRenderer::InitScene(const SceneDescription& theScene)
{
    InitView(theScene);
    InitLights(theScene);
    InitMaterials(theScene);
    InitViewables(theScene);
}

// InitView or SetCameraView are used to set the Projection and ModelView matrices
//    This is applied to all objects in the SceneDescription, including the lights.
// Can be called at any time to update the view, 
//    there is no need to call InitViewables again.
void OpenglRenderer::InitView(const SceneDescription& theScene) 
{
    SetCameraView(theScene.GetCameraView());
}

void OpenglRenderer::SetCameraView(const CameraView& theCameraView)
{
    double nearDistance = theCameraView.GetNearClippingDist();
    double farDistance = theCameraView.GetFarClippingDist();

    if (theCameraView.IsDirectional()) {
        double right = 0.5*theCameraView.GetScreenWidth();
        double top = 0.5*theCameraView.GetScreenHeight();
        ProjectionMatrix.Set_glOrtho(-right, right, -top, top, nearDistance, farDistance);
    }
    else {
        double aspectRatio = theCameraView.GetAspectRatio();
        double theta = 2.0*atan(0.5*theCameraView.GetScreenHeight() / theCameraView.GetScreenDistance());
        ProjectionMatrix.Set_gluPerspective(theta, aspectRatio, nearDistance, farDistance);
    }

    VectorR3 up = theCameraView.GetPixeldV();
    const VectorR3& eye = theCameraView.GetPosition();
    const VectorR3& cntr = theCameraView.GetScreenCenter();
    ModelViewMatrix.Set_gluLookAt(eye, cntr, up);
}

void OpenglRenderer::InitLights(const SceneDescription& theScene) 
{
    BackgroundColor = theScene.BackgroundColor();
    GlobalLightInfo.GlobalAmbientColor = theScene.GlobalAmbientLight();
    GlobalLightInfo.NumLights = Min(phMaxNumLights, theScene.NumLights());
    // We don't fill in the position or directions for the light until render time
    //   This way we are sure to be using the correct ModelViewMatrix for position and direction
    for (unsigned int i = 0; i < GlobalLightInfo.NumLights; i++) {
        SceneLights[i] = theScene.GetLight(i);
        LightInfo[i].AmbientColor = SceneLights[i].GetColorAmbient();
        LightInfo[i].DiffuseColor = SceneLights[i].GetColorDiffuse();
        LightInfo[i].SpecularColor = SceneLights[i].GetColorSpecular();
        LightInfo[i].IsEnabled = true;
        LightInfo[i].IsDirectional = SceneLights[i].IsDirectional();
        LightInfo[i].IsSpotLight = SceneLights[i].SpotActive();
        LightInfo[i].SpotExponent = (float)SceneLights[i].GetSpotExponent();
        LightInfo[i].SpotCosCutoff = (float)SceneLights[i].GetSpotCutoff();        // Cosine of spotlight cutoff angle
        LightInfo[i].IsAttenuated = SceneLights[i].AttenuateActive();
        LightInfo[i].ConstantAttenuation = (float)SceneLights[i].GetAttenuateLinear();
        LightInfo[i].LinearAttenuation = (float)SceneLights[i].GetAttenuateLinear();
        LightInfo[i].QuadraticAttenuation = (float)SceneLights[i].GetAttenuateQuadratic();
    }
}

void OpenglRenderer::InitMaterials(const SceneDescription& /*theScene*/)
{
    return;
}

void OpenglRenderer::InitViewables(const SceneDescription& theScene)
{
    int num = theScene.GetViewableArray().SizeUsed();
    for (int i = 0; i < num; i++) {
        Load_Vbo_Ebo_Mats(theScene.GetViewable(i));
    }
    AddRenderCommand(-1, (const MaterialBase*)0xffffffff, (const MaterialBase*)0xffffffff);

    if (!VBOdata.empty()) {
        SetupOpenGlBuffers();
        glBindBuffer(GL_ARRAY_BUFFER, theVBO);
        glBufferData(GL_ARRAY_BUFFER, VBOdata.size() * sizeof(float), VBOdata.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBOdata.size() * sizeof(unsigned int), EBOdata.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    check_for_opengl_errors();
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableBase& object) {
    switch (object.GetViewableType()) {
    case ViewableBase::Viewable_BezierSet:
        break;
    case ViewableBase::Viewable_Cone:
        break;
    case ViewableBase::Viewable_Cylinder:
        Load_Vbo_Ebo_Mats((const ViewableCylinder&)object);
        break;
    case ViewableBase::Viewable_Ellipsoid:
        Load_Vbo_Ebo_Mats((const ViewableEllipsoid&)object);
        break;
    case ViewableBase::Viewable_Parallelepiped:
        Load_Vbo_Ebo_Mats((const ViewableParallelepiped&)object);
        break;
    case ViewableBase::Viewable_Parallelogram:
        Load_Vbo_Ebo_Mats((const ViewableParallelogram&)object);
        break;
    case ViewableBase::Viewable_Sphere:
        Load_Vbo_Ebo_Mats((const ViewableSphere&)object);
        break;
    case ViewableBase::Viewable_Torus:
        Load_Vbo_Ebo_Mats((const ViewableTorus&)object);
        break;
    case ViewableBase::Viewable_Triangle:
        Load_Vbo_Ebo_Mats((const ViewableTriangle&)object);
        break;
    }

}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableBezierSet& /*object*/) {
    return;  // Not implemented yet
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableCone& /*object*/) {
    return;  // Not implemented yet
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableCylinder& object) {
    if (!object.IsRightCylinder()) {
        return;
    }

    AddRenderCommand(BaseCylinderNumElts / 3, object.GetMaterialBottomInner(), object.GetMaterialBottomOuter());

    VectorR3 center = object.GetCenter();
    VectorR3 centerAxis = object.GetCenterAxis();
    double halfHeight = 0.5*object.GetHeight();
    LinearMapR3 mat(object.GetRadiusB()*object.GetAxisB(), halfHeight*centerAxis, object.GetRadiusA()*object.GetAxisA());
    LinearMapR3 matInvTranspose;
    matInvTranspose.Set(object.GetInvScaledAxisB(), centerAxis/halfHeight, object.GetInvScaledAxisA());

    int baseVertex = VBOdata.size() / 6;
    VectorR3 normal, pos;
    float *vPtr = BaseCylinderVerts;
    for (int i = 0; i < BaseCylinderNumVerts; i++) {
        pos.x = *(vPtr++);
        pos.y = *(vPtr++);
        pos.z = *(vPtr++);
        mat.Transform(&pos);
        pos += center;
        normal.x = *(vPtr++);
        normal.y = *(vPtr++);
        normal.z = *(vPtr++);
        matInvTranspose.Transform(&normal);
        normal.Normalize();
        AddVertPosNormal(pos, normal);
    }
    unsigned int* ePtr = BaseCylinderElts;
    for (int i = 0; i < BaseCylinderNumElts; i++) {
        EBOdata.push_back((*(ePtr++)) + baseVertex);
    }


    return; 
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableEllipsoid& object) {
    AddRenderCommand(BaseSphereNumElts / 3, object.GetMaterialOuter(), object.GetMaterialInner());

    LinearMapR3 mat;
    mat.Set(object.GetScaledAxisB(), object.GetScaledCentralAxis(), object.GetScaledAxisA());
    LinearMapR3 matInvTranspose;
    matInvTranspose.Set(object.GetScaledInvAxisB(), object.GetScaledInvCentralAxis(), object.GetScaledInvAxisA());
    VectorR3 center = object.GetCenter();

    int baseVertex = VBOdata.size() / 6;
    VectorR3 normal;
    float *vPtr = BaseSphereVerts;
    for (int i = 0; i < BaseSphereNumVerts; i++) {
        normal.x = *(vPtr++);
        normal.y = *(vPtr++);
        normal.z = *(vPtr++);
        VectorR3 pos = normal;
        matInvTranspose.Transform(&normal);
        mat.Transform(&pos);
        pos += center;
        AddVertPosNormal(pos, normal);
    }
    unsigned int* ePtr = BaseSphereElts;
    for (int i = 0; i < BaseSphereNumElts; i++) {
        EBOdata.push_back((*(ePtr++)) + baseVertex);
    }
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableParallelepiped& object) {
    // Will add twelve (12) triangles.
    AddRenderCommand(12, object.GetMaterialOuter(), object.GetMaterialInner());

    const VectorR3 normalABC = object.GetNormalABC();
    const VectorR3 normalACD = object.GetNormalACD();
    const VectorR3 normalADB = object.GetNormalABD();
    const VectorR3 vertA = object.GetVertexA();
    const VectorR3 vertB = object.GetVertexB();
    const VectorR3 vertC = object.GetVertexC();
    const VectorR3 vertD = object.GetVertexD();
    const VectorR3 vertBopp = vertC + vertD - vertA;
    const VectorR3 vertCopp = vertB + vertD - vertA;
    const VectorR3 vertDopp = vertB + vertC - vertA;
    const VectorR3 vertAopp = vertBopp + vertB - vertA;
    AddRectangle(vertA, vertB, vertDopp, vertC, -normalABC);
    AddRectangle(vertA, vertC, vertBopp, vertD, -normalACD);
    AddRectangle(vertA, vertD, vertCopp, vertB, -normalADB);
    AddRectangle(vertAopp, vertCopp, vertD, vertBopp, normalABC);
    AddRectangle(vertAopp, vertDopp, vertB, vertCopp, normalACD);
    AddRectangle(vertAopp, vertBopp, vertC, vertDopp, normalADB);
    return;  // Not implemented yet
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableParallelogram& object) {
    // Will add two (2) triangles.
    AddRenderCommand(2, object.GetMaterialFront(), object.GetMaterialBack());

    AddRectangle(object.GetVertexA(), object.GetVertexB(),
                 object.GetVertexC(), object.GetVertexD(), object.GetNormal());
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableSphere& object) {
    AddRenderCommand(BaseSphereNumElts/3, object.GetMaterialOuter(), object.GetMaterialInner());

    LinearMapR3 mat;
    mat.Set(object.GetAxisB(), object.GetAxisC(), object.GetAxisA());
    VectorR3 center = object.GetCenter();

    int baseVertex = VBOdata.size()/6;
    VectorR3 normal;
    VectorR3 pos;
    float *vPtr = BaseSphereVerts;
    for (int i = 0; i < BaseSphereNumVerts; i++) {
        normal.x = *(vPtr++);
        normal.y = *(vPtr++);
        normal.z = *(vPtr++);
        mat.Transform(&normal);
        pos = object.GetRadius()*normal + center;
        AddVertPosNormal(pos, normal);
    }
    unsigned int* ePtr = BaseSphereElts;
    for (int i = 0; i < BaseSphereNumElts; i++) {
        EBOdata.push_back((*(ePtr++)) + baseVertex);
    }
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableTorus& object) {
    AddRenderCommand(BaseTorusNumElts / 3, object.GetMaterialOuter(), object.GetMaterialInner());

    LinearMapR3 mat;
    mat.Set(object.GetAxisB(), object.GetAxisC(), object.GetAxisA());
    VectorR3 center = object.GetCenter();

    int baseVertex = VBOdata.size() / 6;
    VectorR3 normal;
    VectorR3 pos;
    float *vPtr = BaseTorusVerts;
    for (int i = 0; i < BaseTorusNumVerts; i++) {
        pos.x = *(vPtr++);
        pos.y = *(vPtr++);
        pos.z = *(vPtr++);
        normal.x = *(vPtr++);
        normal.y = *(vPtr++);
        normal.z = *(vPtr++);
        VectorR3 centralRingPos = pos - 0.5*normal;
        centralRingPos *= object.GetMajorRadius();
        VectorR3 torusPos = centralRingPos + object.GetMinorRadius()*normal;
        mat.Transform(&torusPos);
        torusPos += center;
        mat.Transform(&normal);
        AddVertPosNormal(torusPos, normal);
    }
    unsigned int* ePtr = BaseTorusElts;
    for (int i = 0; i < BaseTorusNumElts; i++) {
        EBOdata.push_back((*(ePtr++)) + baseVertex);
    }
}

void OpenglRenderer::Load_Vbo_Ebo_Mats(const ViewableTriangle& object) 
{
    // Will add one (1) triangle.
    AddRenderCommand(1, object.GetMaterialFront(), object.GetMaterialBack());

    // Push three verts (positions and normals)
    unsigned int firstElt = VBOdata.size()/6;         // Number of vertices in VBO already
    const VectorR3& normal = object.GetNormal();
    AddVertPosNormal(object.GetVertexA(), normal);
    AddVertPosNormal(object.GetVertexB(), normal);
    AddVertPosNormal(object.GetVertexC(), normal);

    // Make as one triangle
    EBOdata.push_back(firstElt);
    EBOdata.push_back(firstElt + 1);
    EBOdata.push_back(firstElt + 2);
    return;  
}

void OpenglRenderer::AddRectangle(
    const VectorR3& vertA, const VectorR3& vertB,
    const VectorR3& vertC, const VectorR3& vertD,
    const VectorR3& normal)
{
    // Push four verts (positions and normals) for two triangles
    unsigned int firstElt = VBOdata.size() / 6;         // Number of vertices in VBO already
    AddVertPosNormal(vertA, normal);
    AddVertPosNormal(vertB, normal);
    AddVertPosNormal(vertC, normal);
    AddVertPosNormal(vertD, normal);

    // Make as two triangles
    EBOdata.push_back(firstElt);
    EBOdata.push_back(firstElt + 1);
    EBOdata.push_back(firstElt + 2);
    EBOdata.push_back(firstElt + 2);
    EBOdata.push_back(firstElt + 3);
    EBOdata.push_back(firstElt);
}

void OpenglRenderer::AddVertPosNormal(const VectorR3& pos, const VectorR3& normal)
{
    VBOdata.push_back((float)pos.x);
    VBOdata.push_back((float)pos.y);
    VBOdata.push_back((float)pos.z);
    VBOdata.push_back((float)normal.x);
    VBOdata.push_back((float)normal.y);
    VBOdata.push_back((float)normal.z);
}

// Must call AddRenderCommand *before* adding elements to the EBO (to get the start index correct)
void OpenglRenderer::AddRenderCommand(int numTriangles, const MaterialBase* matFront, const MaterialBase* matBack)
{
    if (!RenderCmds.empty() 
            && RenderCmds.back().materialFront == matFront
            && RenderCmds.back().materialBack == matBack) {
        RenderCmds.back().numTriangles += numTriangles;
    }
    else {
        RenderCommand newCmd;
        newCmd.numTriangles = numTriangles;
        newCmd.materialFront = matFront;
        newCmd.materialBack = matBack;
        newCmd.startElement = EBOdata.size();
        RenderCmds.push_back(newCmd);
    }
}




