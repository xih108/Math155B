/*
 * GlGeomTorus.cpp - Version 0.4 - May 8, 2019
 *
 * C++ class for rendering tori in Modern OpenGL.
 *   A GlGeomTorus object encapsulates a VAO, a VBO, and an EBO,
 *   which can be used to render a cylinder.
 *   The number of slices and stacks and rings can be varied.
 *
 * Author: Sam Buss
 *
 * Software accompanying POSSIBLE SECOND EDITION TO the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG2
 */

// Set this true if you only want to get vertex data without using OpenGL here.
#define DONT_USE_OPENGL false

#if !DONT_USE_OPENGL
// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#endif  // DONT_USE_OPENGL

#include "GlGeomTorus.h"
#include "../VrMath/MathMisc.h"
#include "assert.h"


void GlGeomTorus::Remesh(int sides, int rings, float minorRadius)
{
    if (sides == numSides && rings == numRings && minorRadius == radius) {
        return;
    }
    numSides = ClampRange(sides, 3, 255);
    numRings = ClampRange(rings, 3, 255);
    radius = minorRadius;           // Should be between 0.0 and 1.0

    VboEboLoaded = false;
}


void GlGeomTorus::CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
    int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, unsigned int stride)
{
    assert(vertPosOffset >= 0 && stride > 0);
    bool calcNormals = (vertNormalOffset >= 0);       // Should normals be calculated?
    bool calcTexCoords = (vertTexCoordsOffset >= 0);  // Should texture coordinates be calculated?

    // VBO Data is laid out: Around each ring. Starting with ring at x==0 and z<0.
    //          Each ring starts at the innermost seam of the torus (nearest to the y-axis).
    float* toPtr = VBOdataBuffer;

    // Outermost loop over the rings
    int stopRings = calcTexCoords ? numRings : numRings-1;
    for (int i = 0; i <= stopRings; i++) {
        // Handle a ring of vertices.
        // theta measures from the negative z-axis, counterclockwise viewed from above.
        float sCoord = ((float)(i)) / (float)(numRings);
        float theta = (float)PI2 * ((float)(i%numRings)) / (float)(numRings);
        float c = -cosf(theta);      // Negated values (start at negative z-axis)
        float s = -sinf(theta);
        int stopSides = calcTexCoords ? numSides : numSides - 1;
        for (int j = 0; j <= stopSides; j++, toPtr += stride) {
            // phi measures from the inner seam, going under, around and over, back to the inner seam.
            float tCoord = ((float)(j)) / (float)(numSides);
            float phi = (float)PI2 * ((float)(j%numSides)) / (float)(numSides);
            float cphi = -cosf(phi);      // Negated value (start at inner seam)
            float sphi = -sinf(phi);       // Negated, start downward (-y)
            float* posPtr = toPtr;
            *(posPtr++) = s * (1.0f + radius * cphi);    // x coordinate
            *(posPtr++) = radius*sphi;                  // y coordinate
            *posPtr = c * (1.0f + radius * cphi);        // z coordinate
            if (calcNormals) {
                float* nPtr = toPtr + vertNormalOffset;
                *(nPtr++) = s * cphi;           // Normal in x direction
                *(nPtr++) = sphi;                  // Normal in y direction
                *nPtr = c * cphi;               // Normal in z direction
            }
            if (calcTexCoords) {
                float* tcPtr = toPtr + vertTexCoordsOffset;
                *(tcPtr++) = sCoord;
                *tcPtr = tCoord;
            }
        }

        // EBO data is also laid out in the same order, for GL_TRIANGLES
        unsigned int* eboPtr = EBOdataBuffer;
        int ringDelta = calcTexCoords ? numSides + 1 : numSides;
        for (int ii = 0; ii < numRings; ii++) {
            int iii = calcTexCoords ? (ii + 1) : ((ii + 1) % numRings);
            int leftR = ii * ringDelta;
            int rightR = iii *ringDelta;
            for (int j = 0; j < numSides; j++) {
                int jj = calcTexCoords ? (j + 1) : ((j + 1) % numSides);
                *(eboPtr++) = rightR + j;
                *(eboPtr++) = leftR + jj;
                *(eboPtr++) = leftR+j;

                *(eboPtr++) = rightR + j;
                *(eboPtr++) = rightR + jj;
                *(eboPtr++) = leftR + jj;
            }
        }
    }
}


#if !DONT_USE_OPENGL
void GlGeomTorus::InitializeAttribLocations(
    unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
    posLoc = pos_loc;
    normalLoc = normal_loc;
    texcoordsLoc = texcoords_loc;

    // Generate Vertex Array Object and Buffer Objects, not already done.
    if (theVAO == 0) {
        glGenVertexArrays(1, &theVAO);
        glGenBuffers(1, &theVBO);
        glGenBuffers(1, &theEBO);
    }

    // Link the VBO and EBO to the VAO, and request OpenGL to
    //   allocate memory for them.
    glBindVertexArray(theVAO);
    glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    int numVertices = UseTexCoords() ? GetNumVerticesTexCoords() : GetNumVerticesNoTexCoords();
    glBufferData(GL_ARRAY_BUFFER, StrideVal() * numVertices * sizeof(float), 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements() * sizeof(unsigned int), 0, GL_STATIC_DRAW);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posLoc);
    int normalOffset = -1;
    if (UseNormals()) {
        normalOffset = NormalOffset();
        glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float),
            (void*)(normalOffset * sizeof(float)));
        glEnableVertexAttribArray(normalLoc);
    }
    int tcOffset = -1;
    if (UseTexCoords()) {
        tcOffset = TexOffset();
        glVertexAttribPointer(texcoordsLoc, 2, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float),
            (void*)(tcOffset * sizeof(float)));
        glEnableVertexAttribArray(texcoordsLoc);
    }

    // Calculate the buffer data - map and the unmap the two buffers.
    float* VBOdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    unsigned int* EBOdata = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    CalcVboAndEbo(VBOdata, EBOdata, 0, normalOffset, tcOffset, StrideVal());
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    VboEboLoaded = true;

    // Good practice to unbind things: helps with debugging if nothing else
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


// **********************************************
// This routine does the rendering.
// If the torus's VAO, VBO, EBO need to be loaded, it does this first.
// **********************************************

void GlGeomTorus::PreRender()
{
    if (theVAO == 0) {
        assert(false && "GlGeomTorus::InitializeAttribLocations must be called before rendering!");
    }
    if (!VboEboLoaded) {
        InitializeAttribLocations(posLoc, normalLoc, texcoordsLoc);
    }
}

#endif

// Render entire torus as triangles
void GlGeomTorus::Render()
{
    PreRender();

    glBindVertexArray(theVAO);
    glDrawElements(GL_TRIANGLES, GetNumElements(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// Render one ring as triangles
void GlGeomTorus::RenderRing(int i)
{
    assert(i >= 0 && i < numRings);
    PreRender();

    int numElementsPerRing = numSides * 6;
    glBindVertexArray(theVAO);
    glDrawElements(GL_TRIANGLES, numElementsPerRing, GL_UNSIGNED_INT, (void*)(i*numElementsPerRing*sizeof(unsigned int)));
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// Render one strip of sides as a triangle strip
void GlGeomTorus::RenderSideStrip(int j)
{
    assert(j >= 0 && j < numSides);
    PreRender();

    int numElts = 2 * (numRings + 1);
    unsigned int* stackElts = new unsigned int[numElts];
    int numEltsPerRing = UseTexCoords() ? numSides + 1 : numSides;
    int delta = UseTexCoords() ? 1 : (((j+1)%numRings) - j);
    unsigned int* toElt = stackElts;
    for (int i = 0; i <= numRings; i++) {
        int ii = UseTexCoords() ? i : (i%numRings);
        int eltA = ii * numEltsPerRing + j;
        *(toElt++) = eltA+delta;
        *(toElt++) = eltA;
    }

    unsigned int tempEBO;
    glGenBuffers(1, &tempEBO);
    glBindVertexArray(theVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElts * sizeof(unsigned int), stackElts, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLE_STRIP, numElts, GL_UNSIGNED_INT, 0);

    delete[] stackElts;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);  // Restore the main EBO (The VAO maintains its knowledge of this)
    glDeleteBuffers(1, &tempEBO);
    glBindVertexArray(0);



}


GlGeomTorus::~GlGeomTorus()
{
#if !DONT_USE_OPENGL
    glDeleteBuffers(3, &theVAO);  // The three buffer id's are contigous in memory!
#endif  // !DONT_USE_OPENGL
}


