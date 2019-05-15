
// There are 2 shaders in this .glsl file
//  
//  1. vertexShaderNormals
//  2. geomShaderNormals
//  3. fragmentShaderNormals
//
// A shader program to render small normals
//   can be formed from shaders 1 and 2 and 3.
// 
// Author: Sam Buss, sbuss@ucsd.edu.
// Last updated 2/13/2019.


// ***************************
// A vertex shader, named "vertexShaderNormals"
//   Sets the position and color and normal of a vertex as transformed by the **Modelview** matrix
//   Only the modelview matrix is used to position the vertex and normal.
//   It copies the color to "theColor" so that the next shader can access it.
//   The output of this vertex shader is sent to the geometry shader.
// ***************************
#beginglsl vertexshader vertexShaderNormals
#version 330 core
layout (location = 0) in vec3 aPos;	// Position in attribute location 0
layout (location = 1) in vec3 aColor;  // Color in attribute location 1
layout (location = 2) in vec3 aNormal; // Normal Vector in attribute location 2
out vec3 vertColor;					// output a color to the next shaders
out vec3 vertNormal;					// output a color to the geometry shader
uniform mat4 modelviewMatrix;		// The model-view matrix
void main()
{
   gl_Position = modelviewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vertColor = aColor;
   mat3 Msmall = mat3(modelviewMatrix);
   vertNormal = normalize( transpose(inverse(Msmall)) * aNormal );
}
#endglsl

// ***************************
// Geometry shader is named "geomShaderNormals"
//    This outputs edges showing the normals at vertices.
// Geometry shader: outputs the edges and normals of a triangle
// Vertices and normals have already been transformed into world
//     coordinates by the modelview matrix (by the vertex shader).
// ***************************
#beginglsl geometryshader geomShaderNormals
#version 330 core    
layout(triangles) in;              // inputs are triangles    
layout(line_strip, max_vertices = 12) out;   // outputs are line strips    
in vec3 vertColor[];					// input: array containing colors of the vertices    
in vec3 vertNormal[];					// input: array containing normals (unit vectors) of the vertices    
uniform mat4 projectionMatrix;		// The projection matrix    
uniform bool cullBackFaces = true; // cull back faces     
uniform bool drawEdges = true;     // draw the outlines of triangles    
out vec3 theColor;                 // Vertex color output by the geometry shader    
void main()    
{    
    // The next two variables control the appearance of normals.
    vec3 normalColor = vec3(1.0, 1.0, 1.0);  // Set the color to draw the normals    
    float normalLength = 0.4;             // Length to draw the normals  
    
    vec4 projVertPos[3];                  // Holds vertices transformed by the projection matrix    
    for (int i=0; i<3; i++ ) {     
        projVertPos[i] = projectionMatrix*gl_in[i].gl_Position;     
    }    
    if ( cullBackFaces ) {     
        vec3 vert0 = vec3(projVertPos[0])/projVertPos[0].w;     
        vec3 vert1 = vec3(projVertPos[1])/projVertPos[1].w;     
        vec3 vert2 = vec3(projVertPos[2])/projVertPos[2].w;     
        if ( cross(vert1-vert0,vert2-vert1).z <=0 ) {    
            return;    // Ignore this back-facing triangle
        }    
    }    
    if ( drawEdges ) {     
        for (int i=0; i<=3; i++ ) {     
            int j = i%3;      
            gl_Position = projVertPos[j];    
            theColor = vertColor[j];    
            EmitVertex();    
        }    
        EndPrimitive();
    }    
        
    for (int i=0; i<gl_in.length(); i++ ) {     
        vec3 vPos = vec3(gl_in[i].gl_Position)/gl_in[i].gl_Position.w;    
        theColor = normalColor;    
        gl_Position = projVertPos[i];    
        EmitVertex();    
        vPos += normalLength*vertNormal[i];     
        theColor = normalColor;    
        gl_Position = projectionMatrix*vec4(vPos,1.0);    
        EmitVertex();    
        EndPrimitive();    
    }    
}
#endglsl

// ***************************
// Fragment shader, named "fragmentShaderNormals"
//    Set a general color using a fragment shader. (A "fragment" is a "pixel".)
//    The color value is passed in, obtained from the colors on the vertices.
//    Color values range from 0.0 to 1.0.
//    First three values are Red/Green/Blue (RGB).
//    Fourth color value (alpha) is 1.0, meaning there is no transparency.
// ***************************
#beginglsl fragmentshader fragmentShaderNormals
#version 330 core    
in vec3 theColor;        // Color value came from the vertex shader (smoothed)     
out vec4 FragColor;      // Color that will be used for the fragment    
void main()    
{    
   FragColor = vec4(theColor, 1.0f);   // Add alpha value of 1.0.    
}
#endglsl

