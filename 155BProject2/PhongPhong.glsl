// *********************************
// GLSL source code for the shader programs
//     Version 0.6 - February 21, 2019.
//     Author: Sam Buss
//
// Implementation of Phong lighting model,
//    including ambient, diffuse, specular and emissive lighting
//    and optionally the halfway vector and the Schlick-S term.
//    Also allows adding a texture map to the Phong lighting
//
// Vertex and fragment shaders for Phong-Gouraud:
//    vertexShader_PhongGouraud
//         - Vertex shader for Phong lighting with Gouraud shading. 
//           (Does not do the Phong lighting itself.)
//    fragmentShader_PhongGouraud
//         - Fragment shader for Phong lighting with Gouraud shading.
//
// Vertex and fragment shaders for Phong-Phong:
//    vertexShader_PhongPhong
//         - Vertex shader for Phong lighting with Phong shading. 
//           (Does not do the Phong lighting itself.)
//    fragmentShader_PhongPhong
//         - Fragment shader for Phong lighting with Phong shading.
//
// Code blocks for common use (Phong lighting and Texture mapping)
//    calcPhongLighting 
//         - defines the function CalculatePhongLighting()
//           which does the actual Phong lighting calculation
//    applyTextureMap 
//         - defines the function applyTextureFunction()
//           which applies a bitmapped texture map



// **************
// The vertex shader for Phong lighting with Phong shading.
//   Mostly this copies material values, modelview position,
//   and modelview surface normal to the fragment shader.
// **************
#beginglsl vertexshader vertexShader1_PhongPhong
#version 330 core
layout (location = 0) in vec3 vertPos;         // Position in attribute location 0
layout (location = 1) in vec3 vertNormal;      // Surface normal in attribute location 1
layout (location = 2) in vec2 vertTexCoords;   // Texture coordinates in attribute location 2
layout (location = 3) in vec3 EmissiveColor;   // Surface material properties 
layout (location = 4) in vec3 AmbientColor; 
layout (location = 5) in vec3 DiffuseColor; 
layout (location = 6) in vec3 SpecularColor; 
layout (location = 7) in float SpecularExponent; 
layout (location = 8) in float UseFresnel;		// Shold be 1.0 (for Fresnel) or 0.0 (for no Fresnel)

out vec3 mvPos;         // Vertex position in modelview coordinates
vec3 mvNormalFront; // Normal vector to vertex in modelview coordinates
out vec3 matEmissive;
out vec3 matAmbient;
out vec3 matDiffuse;
out vec3 matSpecular;
out float matSpecExponent;
out vec2 theTexCoords;
out float useFresnel;
out vec3 localCoords;

uniform mat4 projectionMatrix;        // The projection matrix
uniform mat4 modelviewMatrix;         // The modelview matrix
uniform float curTime;
uniform int waveMode;

vec3 changed;
float amp;
float lambda;
float freq;
vec3 center;
vec3 center1;
vec3 center2;
float dist;
float dist1;
float dist2;
float lambda1;
float height;
float PI =  3.1415926535f;

void main()
{
	localCoords = vec3(vertPos.x,0,vertPos.z);
	amp = 0.4;
	lambda = 3.5;
	lambda1 = 4;
	freq = 0.1;
	center = vec3(-6,0,0);
	center1 = vec3(-6,0,-6);
	center2 = vec3(-6,0,6);
	if (waveMode == 0){
		dist = sqrt(pow((vertPos.x-center.x),2)+ pow((vertPos.z-center.z),2));
		height = amp*cos(PI*2*(dist/lambda-freq*curTime));
	}
	else{
		dist1 = sqrt(pow((vertPos.x-center1.x),2)+ pow((vertPos.z-center1.z),2));
		dist2 = sqrt(pow((vertPos.x-center2.x),2)+ pow((vertPos.z-center2.z),2));
		height = amp*cos(PI*2*(dist1/lambda1-freq*curTime))+amp*cos(PI*2*(dist2/lambda1-freq*curTime));
	}

	changed = vec3(vertPos.x,height, vertPos.z);

    vec4 mvPos4 = modelviewMatrix * vec4(changed.x,changed.y,changed.z, 1.0); 
    gl_Position = projectionMatrix * mvPos4; 
    mvPos = vec3(mvPos4.x,mvPos4.y,mvPos4.z)/mvPos4.w; 
   //mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface 
    matEmissive = EmissiveColor;
    matAmbient = AmbientColor;
    matDiffuse = DiffuseColor;
    matSpecular = SpecularColor;
    matSpecExponent = SpecularExponent;
    theTexCoords = vertTexCoords;
    useFresnel = UseFresnel;
}
#endglsl

// **************
// The base code for the fragment shader for Phong lighting with Phong shading.
//   This does all the hard work of the Phong lighting by calling CalculatePhongLighting()
// **************
#beginglsl fragmentshader fragmentShader1_PhongPhong
#version 330 core

in vec3 mvPos;         // Vertex position in modelview coordinates
vec3 mvNormalFront; // Normal vector to vertex (front facing) in modelview coordinates
in vec3 matEmissive;
in vec3 matAmbient;
in vec3 matDiffuse;
in vec3 matSpecular;
in float matSpecExponent;
in float useFresnel;

layout (std140) uniform phGlobal { 
    vec3 GlobalAmbientColor;        // Global ambient light color 
    int NumLights;                  // Number of lights 
    bool LocalViewer;               // true for local viewer; false for directional viewer 
    bool EnableEmissive;            // Control whether emissive colors are rendered 
    bool EnableDiffuse;             // Control whether diffuse colors are rendered 
    bool EnableAmbient;             // Control whether ambient colors are rendered 
    bool EnableSpecular;            // Control whether specular colors are rendered 
	bool UseHalfwayVector;			// Control whether halfway vector method is used
};

const int MaxLights = 8;        // The maximum number of lights (must match value in C++ code)
struct phLight { 
    bool IsEnabled;             // True if light is turned on 
    bool IsAttenuated;          // True if attenuation is active 
    bool IsSpotLight;           // True if spotlight 
    bool IsDirectional;         // True if directional 
    vec3 Position; 
    vec3 AmbientColor; 
    vec3 DiffuseColor; 
    vec3 SpecularColor; 
    vec3 SpotDirection;         // Should be unit vector! 
    float SpotCosCutoff;        // Cosine of cutoff angle 
    float SpotExponent; 
    float ConstantAttenuation; 
    float LinearAttenuation; 
    float QuadraticAttenuation; 
};
layout (std140) uniform phLightArray { 
    phLight Lights[MaxLights];
};

vec3 mvNormal; 
in vec2 theTexCoords;          // Texture coordinates (interpolated from vertex shader) 
uniform sampler2D theTextureMap;
uniform bool applyTexture;
uniform mat4 modelviewMatrix; 
uniform float curTime;
uniform int waveMode;

in vec3 localCoords;
vec3 nonspecColor;
vec3 specularColor;  
out vec4 fragmentColor;         // Color that will be used for the fragment

void CalculatePhongLighting();  // Calculates: nonspecColor and specularColor. 
vec4 applyTextureFunction();

vec3 changed;
vec3 vertNormal;
float amp;
float lambda;
float lambda1;
float freq;
vec3 center;
vec3 center1;
vec3 center2;
float dist;
float dist1;
float dist2;
float height;
float PI =  3.1415926535f;
float dx;
float dz;

void main()
{
	amp = 0.4;
	lambda = 3.5;
	lambda1 = 4;
	freq = 0.1;
	center = vec3(-6,0,0);
	center1 = vec3(-6,0,-6);
	center2 = vec3(-6,0,6);
	dist = sqrt(pow((localCoords.x-center.x),2)+ pow((localCoords.z-center.z),2));
	dist1 = sqrt(pow((localCoords.x-center1.x),2)+ pow((localCoords.z-center1.z),2));
	dist2 = sqrt(pow((localCoords.x-center2.x),2)+ pow((localCoords.z-center2.z),2));
	
	if (waveMode == 0){
		dx = -amp*sin(2*PI*(dist/lambda- freq*curTime))*(2*PI*(localCoords.x-center.x)/lambda/dist);
		dz = -amp*sin(2*PI*(dist/lambda- freq*curTime))*(2*PI*(localCoords.z-center.z)/lambda/dist);
	}
	else{
		dx = -amp*sin(2*PI*(dist1/lambda1- freq*curTime))*(2*PI*(localCoords.x-center1.x)/lambda1/dist1)
		     -amp*sin(2*PI*(dist2/lambda1- freq*curTime))*(2*PI*(localCoords.x-center2.x)/lambda1/dist2);
		dz = -amp*sin(2*PI*(dist1/lambda1- freq*curTime))*(2*PI*(localCoords.z-center1.z)/lambda1/dist1)
		     -amp*sin(2*PI*(dist2/lambda1- freq*curTime))*(2*PI*(localCoords.z-center2.z)/lambda1/dist2);
	}

	vertNormal = vec3(-dx,1,-dz);
	mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal);
    if ( gl_FrontFacing ) {
        mvNormal = mvNormalFront;
    }
    else {
        mvNormal = -mvNormalFront;
    }

    CalculatePhongLighting();       // Calculates: nonspecColor and specularColor. 
    fragmentColor = vec4(nonspecColor+specularColor, 1.0f);   // Add alpha value of 1.0.
    if ( applyTexture ) { 
        fragmentColor = applyTextureFunction();
    }


}
#endglsl

