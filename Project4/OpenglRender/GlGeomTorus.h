/*
 * GlGeomTorus.h - Version 0.3 - February 28, 2019
 *
 * C++ class for rendering tori in Modern OpenGL.
 *   A GlGeomTorus object encapsulates a VAO, a VBO, and an EBO,
 *   which can be used to render a torus.
 *   The number of rings and sides can be varied.
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

#ifndef GLGEOM_TORUS_H
#define GLGEOM_TORUS_H

#include <limits.h>

// GlGeomTorus
//     Generates vertices, normals, and texture coodinates for a torus.
//     Torus is formed of "rings" and "sides"
//     Torus is centered at the origin, symmetrically around the y-axis.
//     The central axis is the y-axis. Texture coord (0.5,0.5) is on the z-axis.
// Supports either of the modes:
//    (1) Allocating and loading a VAO, VBO, and EBO, and doing the rendering.
//    (2) Loading an external VBO with vertex data, and an external EBO with
//        elements. This requires the calling program to allocate and load the
//        VAO, VBO and EBO; to set the attribute information in the VAO,
//        and to issue the glDrawElements(GL_TRIANGLES,...) commands.
// Mode (1) is easiest when rendering simple tori as independent objects.
// Mode (2) allows more sophisticated handling of triangle data.
// For both modes:
//          First call either the constructor or Remesh() to set the numbers 
//          of sides and rings.
// For Mode (1), then call the routines:
//          InitializeAttribLocations() - gives locations in the VBO buffer for the shader program
//          Render() - gives the glDrawElements commands for the torus using the VAO, VBO and EBO.
// For Mode (2), then call the routines
//          CalcVboAndEbo()
// The ratio of the major and minor radii can be veried by user specification.
//         Value minorRatio is the radius of the circular tube.
//         Major radius is fixed at 1.0.
//         Minor radius should be less than 1.0.
// The number of rings = number of cuts at right angles to the inner circular path.
//         E.g. To share a doughtnut four ways, you would want to cut four rings.
// The number of sides = number of wedges along the inner circular path.


class GlGeomTorus
{
public:
    GlGeomTorus() : GlGeomTorus(8, 8) {}
    GlGeomTorus(int sides, int rings, float minorRadius= 0.5);
    ~GlGeomTorus();

	// Re-mesh to change the number sides and.
    // Mode (1): Can be called either before or after InitAttribLocations(), but it is
    //    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
    // Mode (2): Call before CalcVboAndEbo (or use the constructor to specify mesh resolution).
    void Remesh(int sides, int rings) { Remesh(sides, rings, radius); }
    void Remesh(int sides, int rings, float minorRadius);

	// Allocate the VAO, VBO, and EBO.
	// Set up info about the Vertex Attribute Locations
	// This must be called before Render() is first called.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional.
    void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

    void Render();          // Render: renders entire torus

    // Mode (2) 
    // CalcVboAndEbo- return all VBO vertex information, and EBO elements for GL_TRIANGLES drawing.
    // Inputs:
    //   VBOdataBuffer, EBOdataBuffer are filled with the vertex info and elements for GL_TRIANGLES drawing
    //   vertPosOffset and stride control where the vertex positions are placed.
    //   vertNormalOffset and stride control where the vertex normals are placed.
    //   vertTexCoordsOffset and stride control where the texture coordinates are placed.
    //   Offset and stride values are **integers** (not bytes), measuring offsets in terms of floats.
    //   Use "-1" for the offset for any value which should be omitted.
    //   For the (unit) sphere, the normals are always exactly equal to the positions.
    // Output: 
    //   Data VBO and EBO data is calculated and loaded into the two buffers VBOdataBuffer and EBOdataBuffer.
    // Typical usages are:
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, -1, -1, 3); // positions only, tightly packed
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, -1, 3, 5); // positions, then (s,t) texture coords, tightly packed
    //   CalcVboAndEbo( vboPtr, eboPtr, 0, 3, 6, 8); // positions, normals, then (s,t) texture coords, tightly packed
    // Use GetNumElements() and GetNumVertices()
    //    to determine the amount of data that will returned by CalcVboAndEbo.
    //    The calling program must preallocate this before the call to CalcVboAndEbo of course.
    void CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
        int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset,
        unsigned int stride);
    int GetNumElements() const { return 6 * numRings * numSides; } 
    int GetNumVerticesNoTexCoords() const { return numRings * numSides; }
    int GetNumVerticesTexCoords() const { return (numRings + 1) * (numSides + 1); }
 
    int GetVAO() const { return theVAO; }
    int GetVBO() const { return theVBO; }
    int GetEBO() const { return theEBO; }

    int GetNumSides() const { return numSides; }
    int GetNumRings() const { return numRings; }
    float GetMinorRadius() const { return radius; }
    float GetMajorRadius() const { return 1.0; }

    // Some specialized render routines for rendering portions of the torus
    // Selectively render a ring or a strip of sides
    // Ring numbers i rangle from 0 to numRings-1.
    // Stack numbers j are allowed to range from 1 to numStacks-2.
    void RenderRing(int i);         // Renders the i-th ring as triangles
    void RenderSideStrip(int j);    // Renders the j-th side-strip as a triangle strip

    // Disable all copy and assignment operators.
	// A GlGeomTorus can be allocated as a global or static variable, or with new.
	//     If you need to pass it to/from a function, use references or pointers
    //     and be sure that there are no implicit copy or assignment operations!
    GlGeomTorus(const GlGeomTorus&) = delete;
    GlGeomTorus& operator=(const GlGeomTorus&) = delete;
    GlGeomTorus(GlGeomTorus&&) = delete;
    GlGeomTorus& operator=(GlGeomTorus&&) = delete;

private:
    int numSides;            // Number sides going around the inner circular path
    int numRings;           // Number of ring-like pieces
    float radius;          // Minor radius (major radius is fixed equal to 1.0).

private: 
    unsigned int theVAO = 0;        // Vertex Array Object
    unsigned int theVBO = 0;        // Vertex Buffer Object
    unsigned int theEBO = 0;        // Element Buffer Object;
    bool VboEboLoaded = false;

    void PreRender();

    // Stride value, and offset values for the data in the VBO.
    //   These take into account whether normals and texture coordinates are used.
    bool UseNormals() const { return normalLoc != UINT_MAX; }
    bool UseTexCoords() const { return texcoordsLoc != UINT_MAX; }
    int StrideVal() const {
        return 3 + (UseNormals() ? 3 : 0) + (UseTexCoords() ? 2 : 0);
    }
    int NormalOffset() const { return 3; }
    int TexOffset() const { return 3 + (UseNormals() ? 3 : 0); }
 
	unsigned int posLoc;            // location of vertex position x,y,z data in the shader program
	unsigned int normalLoc;         // location of vertex normal data in the shader program
	unsigned int texcoordsLoc;      // location of s,t texture coordinates in the shader program.
};

inline GlGeomTorus::GlGeomTorus(int sides, int rings, float minorRadius)
{
	numSides = sides;
    numRings = rings;
    radius = minorRadius;
}

#endif  // GLGEOM_TORUS_H
