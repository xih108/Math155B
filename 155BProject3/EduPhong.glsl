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
#beginglsl vertexshader vertexShader_PhongPhong
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

out vec3 mvPos_base;         // Vertex position in modelview coordinates
out vec3 mvNormalFront_base; // Normal vector to vertex in modelview coordinates
out vec3 matEmissive_base;
out vec3 matAmbient_base;
out vec3 matDiffuse_base;
out vec3 matSpecular_base;
out float matSpecExponent_base;
out vec2 theTexCoords_base;
out float useFresnel_base;
out vec3 origVert; 

uniform mat4 projectionMatrix;        // The projection matrix
uniform mat4 modelviewMatrix;         // The modelview matrix

void main()
{
	origVert = vertPos;
    vec4 mvPos4 = modelviewMatrix * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0); 
    gl_Position = projectionMatrix * mvPos4; 
    mvPos_base = vec3(mvPos4.x,mvPos4.y,mvPos4.z)/mvPos4.w; 
    mvNormalFront_base = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface 
    matEmissive_base = EmissiveColor;
    matAmbient_base = AmbientColor;
    matDiffuse_base = DiffuseColor;
    matSpecular_base = SpecularColor;
    matSpecExponent_base = SpecularExponent;
    theTexCoords_base = vertTexCoords;
    useFresnel_base = UseFresnel;
}
#endglsl

//**************//
#beginglsl geometryshader geomShader
#version 330 core    
layout(triangles) in;              // inputs are triangles    
layout(triangle_strip, max_vertices = 23) out;   // outputs are line strips    
uniform mat4 projectionMatrix;		// The projection matrix      
uniform mat4 modelviewMatrix;

in vec3 mvPos_base[];         
in vec3 mvNormalFront_base[];
in vec3 matEmissive_base[];
in vec3 matAmbient_base[];
in vec3 matDiffuse_base[];
in vec3 matSpecular_base[];
in float matSpecExponent_base[];
in float useFresnel_base[];
in vec2 theTexCoords_base[]; 
in vec3 origVert[];

out vec3 mvPos;
out vec3 mvNormalFront;
out vec3 matEmissive;
out vec3 matAmbient;
out vec3 matDiffuse;
out vec3 matSpecular;
out float matSpecExponent;
out float useFresnel;
out vec2 theTexCoords;



void main()    
{    
    // The next two variables control the appearance of normals.
	
    // Holds vertices transformed by the projection matrix    

	vec3 middle[3];
	vec3 inner[3];

	float dist = max(max(distance(origVert[0],origVert[1]),distance(origVert[0],origVert[2])),distance(origVert[2],origVert[1]));
	middle[0] = vec3(6*origVert[0].x/8 + origVert[1].x/8 + origVert[2].x/8, 0, 6*origVert[0].z/8 + origVert[1].z/8 + origVert[2].z/8) ;
	middle[1] = vec3(origVert[0].x/8 + 6*origVert[1].x/8 + origVert[2].x/8, 0,origVert[0].z/8 + 6*origVert[1].z/8 + origVert[2].z/8) ;
	middle[2] = vec3(origVert[0].x/8 + origVert[1].x/8 + 6*origVert[2].x/8, 0,origVert[0].z/8 + origVert[1].z/8 + 6*origVert[2].z/8) ;
	
	inner[0] = vec3(6*middle[0].x/8 + middle[1].x/8 + middle[2].x/8, dist/10,6*middle[0].z/8 + middle[1].z/8 + middle[2].z/8) ;
	inner[1] = vec3(middle[0].x/8 + 6*middle[1].x/8 + middle[2].x/8, dist/10,middle[0].z/8 + 6*middle[1].z/8 + middle[2].z/8) ;
	inner[2] = vec3(middle[0].x/8 + middle[1].x/8 + 6*middle[2].x/8, dist/10,middle[0].z/8 + middle[1].z/8 + 6*middle[2].z/8) ;

	vec3 outstrip[8];
	vec3 strip1[4];
	vec3 strip2[4];
	vec3 strip3[4];

	for (int i = 0; i < 4; i++){
		outstrip[2*i] = middle[i%3];
		outstrip[2*i+1] = origVert[i%3];
	}

	for (int i = 0; i < 2; i++){
		strip1[2*i] = inner[i];
		strip1[2*i+1] = middle[i]; 
		strip2[2*i] = inner[(i+1)%3];
		strip2[2*i+1] = middle[(i+1)%3];
		strip3[2*i] = inner[(i+2)%3];
		strip3[2*i+1] = middle[(i+2)%3]; 
	}

	vec3 normals[3];
	normals[0] = cross(strip1[0]-strip1[1],strip1[1]-strip1[2]);
	normals[1] = cross(strip2[0]-strip2[1],strip2[1]-strip2[2]);
	normals[2] = cross(strip3[0]-strip3[1],strip3[1]-strip3[2]);

	for (int i = 0; i < 8; i++){
		vec4 mvPos4 = modelviewMatrix*vec4(outstrip[i], 1);
		gl_Position = projectionMatrix*mvPos4;
		mvPos = vec3(mvPos4.x,mvPos4.y, mvPos4.z)/mvPos4.w;
		mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix))) *vec3(0.0,1.0,0.0));
		matEmissive = matEmissive_base[0];
		matAmbient = matAmbient_base[0];
		matDiffuse = vec3(0.2,0.4,0.6);
		matSpecular = matSpecular_base[0];
		matSpecExponent = matSpecExponent_base[0];
		useFresnel = useFresnel_base[0];
		theTexCoords = theTexCoords_base[0];
		EmitVertex();
	}
	EndPrimitive();

	for (int i = 0; i < 4; i++){
		vec4 mvPos4 = modelviewMatrix*vec4(strip1[i], 1);
		gl_Position = projectionMatrix*mvPos4;
		mvPos = vec3(mvPos4.x,mvPos4.y, mvPos4.z)/mvPos4.w;
		mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix))) *normals[0]);
		matEmissive = matEmissive_base[0];
		matAmbient = 0.8*matAmbient_base[0];
		matDiffuse = vec3(0.2,0.0,0.4);
		matSpecular = matSpecular_base[0];
		matSpecExponent = matSpecExponent_base[0];
		useFresnel = useFresnel_base[0];
		theTexCoords = theTexCoords_base[0];
		EmitVertex();
	}
	EndPrimitive();

	for (int i = 0; i < 4; i++){
		vec4 mvPos4 = modelviewMatrix*vec4(strip2[i], 1);
		gl_Position = projectionMatrix*mvPos4;
		mvPos = vec3(mvPos4.x,mvPos4.y, mvPos4.z)/mvPos4.w;
		mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix))) *normals[1]);
		matEmissive = matEmissive_base[0];
		matAmbient = 0.8*matAmbient_base[0];
		matDiffuse = vec3(0.2,0.0,0.4);
		matSpecular = matSpecular_base[0];
		matSpecExponent = matSpecExponent_base[0];
		useFresnel = useFresnel_base[0];
		theTexCoords = theTexCoords_base[0];
		EmitVertex();
	}
	EndPrimitive();

	for (int i = 0; i < 4; i++){
		vec4 mvPos4 = modelviewMatrix*vec4(strip3[i], 1);
		gl_Position = projectionMatrix*mvPos4;
		mvPos = vec3(mvPos4.x,mvPos4.y, mvPos4.z)/mvPos4.w;
		mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix))) *normals[2]);
		matEmissive = matEmissive_base[0];
		matAmbient = 0.8*matAmbient_base[0];
		matDiffuse = vec3(0.2,0.0,0.4);
		matSpecular = matSpecular_base[0];
		matSpecExponent = matSpecExponent_base[0];
		useFresnel = useFresnel_base[0];
		theTexCoords = theTexCoords_base[0];
		EmitVertex();
	}
	EndPrimitive();

	for (int i = 0; i < 3; i++){
		vec4 mvPos4 = modelviewMatrix*vec4(inner[i], 1);
		gl_Position = projectionMatrix*mvPos4;
		mvPos = vec3(mvPos4.x,mvPos4.y, mvPos4.z)/mvPos4.w;
		mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix))) *vec3(0.0,1.0,0.0));
		matEmissive = matEmissive_base[0];
		matAmbient = matAmbient_base[0];
		matDiffuse = vec3(0.8,0.6,0.7);
		matSpecular = matSpecular_base[0];
		matSpecExponent = matSpecExponent_base[0];
		useFresnel = useFresnel_base[0];
		theTexCoords = theTexCoords_base[0];
		EmitVertex();
	}
	EndPrimitive();


}
#endglsl

//*************//
// **************
// The base code for the fragment shader for Phong lighting with Phong shading.
//   This does all the hard work of the Phong lighting by calling CalculatePhongLighting()
// **************
#beginglsl fragmentshader fragmentShader_PhongPhong
#version 330 core

in vec3 mvPos;         // Vertex position in modelview coordinates
in vec3 mvNormalFront; // Normal vector to vertex (front facing) in modelview coordinates
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

vec3 nonspecColor;
vec3 specularColor;  
out vec4 fragmentColor;         // Color that will be used for the fragment

void CalculatePhongLighting();  // Calculates: nonspecColor and specularColor. 
vec4 applyTextureFunction();

void main() { 
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

// ********************
// The vertex shader for Phong lighting (with Gouraud shading).
//   This does the hard work of the Phong lighting by calling CalculatePhongLighting()
// ********************
#beginglsl vertexshader vertexShader_PhongGouraud
#version 330 core

layout (location = 0) in vec3 vertPos;         // Position in attribute location 0
layout (location = 1) in vec3 vertNormal;      // Surface normal in attribute location 1
layout (location = 2) in vec2 vertTexCoords;   // Texture coordinates in attribute location 2
layout (location = 3) in vec3 EmissiveColor;   // Surface material properties 
layout (location = 4) in vec3 AmbientColor; 
layout (location = 5) in vec3 DiffuseColor; 
layout (location = 6) in vec3 SpecularColor; 
layout (location = 7) in float SpecularExponent; 
layout (location = 8) in float UseFresnel;	   // Should be 1.0 (for Fresnel) or 0.0 (for no Fresnel)

out vec3 nonspecColor;  
out vec3 specularColor;  
out vec2 theTexCoords;

layout (std140) uniform phGlobal { 
    vec3 GlobalAmbientColor;        // Global ambient light color 
    int NumLights;                  // Number of lights 
    bool LocalViewer;               // true for local viewer; false for directional viewer 
    bool EnableEmissive;            // Control whether emissive colors are rendered 
    bool EnableDiffuse;             // Control whether diffuse colors are rendered 
    bool EnableAmbient;             // Control whether ambient colors are rendered 
    bool EnableSpecular;            // Control whether specular colors are rendered 
	bool UseHalfwayVector;			// Control whether halfway vector is used.
};

const int MaxLights = 8;         // The maximum number of lights (must match value in C++ code)
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

uniform mat4 projectionMatrix;        // The projection matrix
uniform mat4 modelviewMatrix;         // The modelview matrix

vec3 mvPos;   // Vertex position in modelview coordinates
vec3 mvNormal; // Normal vector to vertex in modelview coordinates
vec3 matEmissive;
vec3 matAmbient;
vec3 matDiffuse;
vec3 matSpecular;
float matSpecExponent;
float useFresnel;
void CalculatePhongLighting();

void main()
{
    vec4 mvPos4 = modelviewMatrix * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0); 
    gl_Position = projectionMatrix * mvPos4; 
    mvPos = vec3(mvPos4.x,mvPos4.y,mvPos4.z)/mvPos4.w; 
    mvNormal = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); 
    matEmissive = EmissiveColor;
    matAmbient = AmbientColor;
    matDiffuse = DiffuseColor;
    matSpecular = SpecularColor;
    matSpecExponent = SpecularExponent;
    theTexCoords = vertTexCoords; 
    useFresnel = UseFresnel;
	
    CalculatePhongLighting();  // Calculates nonspecColor and specularColor. 
} 
#endglsl


// *************************
// The fragment shader for Gouraud shading (after Phong lighting)
// is very simple.  All it does it output the color as averaged (smoothed)
// the colors computed by the vertex shaders
// *************************
#beginglsl fragmentshader fragmentShader_PhongGouraud
#version 330 core
in vec3 nonspecColor;      // Nonspecular color (smoothed) calculated at vertex 
in vec3 specularColor;     // Specular color (smoothed) calculated at vertex 
in vec2 theTexCoords;
out vec4 fragmentColor;    // Color that will be used for the fragment
uniform sampler2D theTextureMap;
uniform bool applyTexture;

vec4 applyTextureFunction();

void main()
{
    fragmentColor = vec4(nonspecColor+specularColor, 1.0f);   // Add alpha value of 1.0.
    if ( applyTexture ) { 
        fragmentColor = applyTextureFunction();
    }
}
#endglsl

// **************
// CalculatePhongLighting() 
//   Shared code for calculating Phong light!
//   Inputs: 
//     (a) Material properties (mvPos through useFresnel).
//     (b) Global light properties (phGlobal uniform structure)
//     (c) Individual light properties (phLightArray, with phLight structures)
//   Outputs:
//     (a) nonspecColor (combined non-specular components of the color)
//     (b) specularColor (specular component of the color)
//   Assumes normal vector mvNormal is for the side of the triangle facing viewer.(!)
// **************
#beginglsl codeblock calcPhongLighting
// This routine calculates the two vec3's nonspecColor and specularColor
void CalculatePhongLighting() { 
    nonspecColor = vec3(0.0, 0.0, 0.0);  
    specularColor = vec3(0.0, 0.0, 0.0);  
    if ( EnableEmissive ) { 
       nonspecColor = matEmissive; 
    }
    if ( EnableAmbient ) { 
         nonspecColor += matAmbient*GlobalAmbientColor; 
    } 
    // vVector =  unit vector towards view direction
    vec3 vVector = LocalViewer ? -mvPos : vec3(0.0, 0.0, 1.0);
    vVector = normalize(vVector);
    for ( int i=0; i<NumLights; i++ ) {
        if ( Lights[i].IsEnabled ) { 
            // nonspecColorLt and specularColorLt - color from this light
            vec3 nonspecColorLt = vec3(0.0, 0.0, 0.0);        
            vec3 specularColorLt = vec3(0.0, 0.0, 0.0);
            // ellVector = unit vector towards light source
            vec3 ellVector = -Lights[i].Position;  
            if ( !Lights[i].IsDirectional ) {
                ellVector = -(ellVector + mvPos);
            }
            ellVector = normalize(ellVector); 
            float dotEllNormal = dot(ellVector, mvNormal); 
            if (dotEllNormal > 0 ) { 
                float spotCosine;
                if ( Lights[i].IsSpotLight ) {
                    spotCosine = -dot(ellVector,Lights[i].SpotDirection);
                }
                if ( !Lights[i].IsSpotLight || spotCosine > Lights[i].SpotCosCutoff ) {
                    if ( EnableDiffuse ) { 
                        nonspecColorLt += matDiffuse*Lights[i].DiffuseColor*dotEllNormal; 
                    } 
                    if ( EnableSpecular ) { 
                        float specFactor = 0.0;        // Includes (cos)^f factor and Fresnel factor
                        if ( UseHalfwayVector ) {
                            vec3 hVector = normalize(ellVector+vVector);
                            specFactor = pow( dot(hVector,mvNormal), matSpecExponent );
                        }
						else {
                            float rDotV = dot(vVector, 2.0*dotEllNormal*mvNormal - ellVector); 
                            if ( rDotV>0.0 ) {
                                specFactor = pow( rDotV, matSpecExponent);
                            }
                        }
						vec3 matspec = matSpecular;
						if ( useFresnel!=0.0 ) {
							float d = 1.0 - dotEllNormal;
							float dd = d*d;
							float fresnelBlend = dd*dd*d*useFresnel;   // Blending factor for Fresnel
							matspec = mix(matSpecular, vec3(1.0,1.0,1.0), fresnelBlend);
						}
                        specularColorLt += specFactor*matspec*Lights[i].SpecularColor; 
                    }
                    if ( Lights[i].IsSpotLight ) {
                        float spotAtten = pow(spotCosine,Lights[i].SpotExponent);
                        nonspecColorLt *= spotAtten; 
                        specularColorLt *= spotAtten;
                    } 
                }
            }
            if ( EnableAmbient ) { 
                nonspecColorLt += matAmbient*Lights[i].AmbientColor; 
            } 
            if ( Lights[i].IsAttenuated ) { 
                float dist = distance(mvPos,Lights[i].Position); 
                float atten = 1.0/(Lights[i].ConstantAttenuation + (Lights[i].LinearAttenuation + Lights[i].QuadraticAttenuation*dist)*dist);
                nonspecColorLt *= atten; 
                specularColorLt *= atten;
            } 
            nonspecColor += nonspecColorLt;
            specularColor += specularColorLt;
        }
    }
}
#endglsl

// *****************************
// applyTextureMap - code block
//    Inputs: (all global variables)
//        - nonspecColor and specularColor (global variables, vec3 objects)
//        - theTexCoords (the texture coordinates, a vec2 object)
//    Returns a vec4:
//       - Will be used as the final fragment color
// *****************************
#beginglsl codeblock applyTextureMap
vec4 applyTextureFunction()
{
    return vec4(nonspecColor, 1.0f)*texture(theTextureMap, theTexCoords) + vec4(specularColor,0.0);
}
#endglsl




