/*
 * ConnectDotsModern.cpp - Version 1.0, March 28, 2019.
 *
 * Example program illustrating a simple use
 * of Modern OpenGL to take mouse clicks and
 * connects points with straight lines.
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

// Use space to toggle what image is shown.
// Use Escape or 'X' or 'x' to exit.

// These libraries are needed to link the program (Visual Studio specific)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "ShaderMgrSDM.h"
bool check_for_opengl_errors();     // Function prototype (should really go in a header file)
int type = 0;
bool polygon = 0;
// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>
#include <assert.h>
#include <math.h>

// ********************
// Animation controls and state infornation
// ********************

constexpr int MaxNumDots = 100;     // Maximum allowed number of dots
int NumDots = 0;                    // Current number of points
float dotArray[MaxNumDots][2];
const int meshRes = 30;
float point[meshRes*(MaxNumDots-1)+1][2];
int windowWidth, windowHeight;

int selectedVert = -1;          // Either currently selected vertex or -1 (none selected)


float cltpts[3 * (MaxNumDots - 1) + 1][2];



// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************

const int NumCurves = 4;
const int iSeg = 0;
const int icltpts = 1;
const int iCurve = 2;

unsigned int myVBO[NumCurves];  // Vertex Buffer Object - holds an array of data
unsigned int myVAO[NumCurves];  // Vertex Array Object - holds info about an array of vertex data;

// We create one shader program: it consists of a vertex shader and a fragment shader
unsigned int shaderProgram1;
const unsigned int vertPos_loc = 0;   // Corresponds to "location = 0" in the verter shader definition
const unsigned int vertColor_loc = 1; // Corresponds to "location = 1" in the verter shader definition

// *************************
// mySetupGeometries defines the scene data, especially vertex  positions and colors.
//    - It also loads all the data into the VAO's (Vertex Array Objects) and
//      into the VBO's (Vertex Buffer Objects).
// This routine is only called once to initialize the data.
// *************************
void mySetupGeometries() {

	// In this simple example, we do not use the Projection or
	//   ModelView matrices. Hence, all x, y, z positions
	//   should be in the range [-1,1].

    // Allocate Vertex Array Objects (VAOs) and Vertex Buffer Objects (VBOs).
    glGenVertexArrays(NumCurves, &myVAO[iSeg]);
    glGenBuffers(NumCurves, &myVBO[iSeg]);

    // Bind (and initialize) the Vertex Array Object and Vertex Buffer Object
    // The glBufferData command allocates space in the VBO for the vertex data, but does
    //    not any data into the VBO.  For this, the third parameter is the null pointer, (void*)0.
    // The VBO will hold only the vertex positions.  The color will be a generic attribute.
    glBindVertexArray(myVAO[iSeg]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iSeg]);
    glBufferData(GL_ARRAY_BUFFER, MaxNumDots*2*sizeof(float), (void*)0, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);	
    glEnableVertexAttribArray(vertPos_loc);		
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iSeg]);
	glBindVertexArray(myVBO[iSeg]);

	glBindVertexArray(myVAO[icltpts]);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[icltpts]);
	glBufferData(GL_ARRAY_BUFFER, (3*(MaxNumDots-1)+1) * 2 * sizeof(float), (void*)0, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vertPos_loc);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[icltpts]);
	glBindVertexArray(myVBO[icltpts]);

	glBindVertexArray(myVAO[iCurve]);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCurve]);
	glBufferData(GL_ARRAY_BUFFER, (meshRes * (MaxNumDots - 1) + 1) * 2 * sizeof(float), (void*)0, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vertPos_loc);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCurve]);
	glBindVertexArray(myVBO[iCurve]);


    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void LoadPointsIntoVBO() 
{
    // Using glBufferSubData (with "Sub") does not resize the VBO.  
    // The VBO was sized earlier with glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iSeg]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NumDots * 2 * sizeof(float), dotArray);

	glBindBuffer(GL_ARRAY_BUFFER, myVBO[icltpts]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (3 * (NumDots - 1) + 1) * 2 * sizeof(float), cltpts);
    
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iCurve]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (meshRes * (NumDots - 1) + 1) * 2 * sizeof(float), point);

	check_for_opengl_errors();
}

float lerp(float x0, float x1, float a) {
	//printf("0:  %f, 1:   %f, a:  %f\n",x0 ,x1 ,a );

	return (1 - a)*x0 + a * x1;
}
void DeCasteljau(int meshRes) {
	for (int i = 0; i < NumDots; i++) {
		if (i == 0) {
			point[0][0] = cltpts[0][0];
			point[0][1] = cltpts[0][1];
		}
		if (i >= 1) {
			for (int j = 1; j <= meshRes; j++) {
				float a = float(j) / meshRes;
				//printf("&&&& %f",a);
				float r0_x = lerp(cltpts[3 * (i - 1)][0], cltpts[3 * (i - 1) + 1][0], a);
				float r0_y = lerp(cltpts[3 * (i - 1)][1], cltpts[3 * (i - 1) + 1][1], a);
				float r1_x = lerp(cltpts[3 * (i - 1) + 1][0], cltpts[3 * (i - 1) + 2][0], a);
				float r1_y = lerp(cltpts[3 * (i - 1) + 1][1], cltpts[3 * (i - 1) + 2][1], a);
				float r2_x = lerp(cltpts[3 * (i - 1) + 2][0], cltpts[3 * (i - 1) + 3][0], a);
				float r2_y = lerp(cltpts[3 * (i - 1) + 2][1], cltpts[3 * (i - 1) + 3][1], a);
				float s0_x = lerp(r0_x, r1_x, a);
				float s0_y = lerp(r0_y, r1_y, a);
				float s1_x = lerp(r1_x, r2_x, a);
				float s1_y = lerp(r1_y, r2_y, a);
				float t0_x = lerp(s0_x, s1_x, a);
				float t0_y = lerp(s0_y, s1_y, a);
				point[meshRes*(i - 1) + j][0] = t0_x;
				point[meshRes*(i - 1) + j][1] = t0_y;
			}
		}
	}
	LoadPointsIntoVBO();
}

void Catmull()
{
	for (int i = 0; i < NumDots; i++) {
		cltpts[3 * i][0] = dotArray[i][0];
		cltpts[3 * i][1] = dotArray[i][1];
		if (i == 1) {
			cltpts[1][0] = dotArray[0][0] + (dotArray[1][0] - dotArray[0][0]) / 6;
			cltpts[1][1] = dotArray[0][1] + (dotArray[1][1] - dotArray[0][1]) / 6;
			cltpts[2][0] = dotArray[1][0] - (dotArray[1][0] - dotArray[0][0]) / 6;
			cltpts[2][1] = dotArray[1][1] - (dotArray[1][1] - dotArray[0][1]) / 6;;
		}

		if (i > 1) {
			cltpts[3 * i - 4][0] = dotArray[i - 1][0] - (dotArray[i][0] - dotArray[i - 2][0]) / 6;
			cltpts[3 * i - 4][1] = dotArray[i - 1][1] - (dotArray[i][1] - dotArray[i - 2][1]) / 6;
			cltpts[3 * i - 2][0] = dotArray[i - 1][0] + (dotArray[i][0] - dotArray[i - 2][0]) / 6;
			cltpts[3 * i - 2][1] = dotArray[i - 1][1] + (dotArray[i][1] - dotArray[i - 2][1]) / 6;
			cltpts[3 * i - 1][0] = dotArray[i][0] - (dotArray[i][0] - dotArray[i - 1][0]) / 6;
			cltpts[3 * i - 1][1] = dotArray[i][1] - (dotArray[i][1] - dotArray[i - 1][1]) / 6;
		}
	}
	LoadPointsIntoVBO();
	DeCasteljau(meshRes);
}

float dist(float x1, float x2, float y1, float y2) {
	float dist = (x1 - x2)*(x1 - x2) + (y1 - y2) *(y1 - y2);
	return sqrt(dist);
}

float dist_sqrt(float x1, float x2, float y1, float y2) {
	float dist = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2) *(y1 - y2));
	return sqrt(dist);
}

void Chord() {
	for (int i = 0; i < NumDots; i++) {
		cltpts[3 * i][0] = dotArray[i][0];
		cltpts[3 * i][1] = dotArray[i][1];
		if (i == 1) {
			cltpts[1][0] = dotArray[0][0];
			cltpts[1][1] = dotArray[0][1];
			cltpts[2][0] = dotArray[1][0];
			cltpts[2][1] = dotArray[1][1];
		}

		if (i > 1) {
			float dist1 = dist(dotArray[i][0], dotArray[i - 1][0], dotArray[i][1], dotArray[i - 1][1]);
			float dist0 = dist(dotArray[i - 1][0], dotArray[i - 2][0], dotArray[i - 1][1], dotArray[i - 2][1]);
			float dist2 = dist1 + dist0;

			//printf("*******dist1 %f \n",dist1);
			//printf("dist2 %f \n", dist2);
			//printf("dist0 %f \n", dist0);
			float v_p_x = 0;
			float v_p_y = 0;
			if (dist1 > 0) {
				v_p_x = (dotArray[i][0] - dotArray[i - 1][0]) / dist1;
				v_p_y = (dotArray[i][1] - dotArray[i - 1][1]) / dist1;
			}

			//printf("v_p_x %f \n", v_p_x);
			//printf("v_p_y %f \n", v_p_y);

			float v_m_x = 0;
			float v_m_y = 0;
			if (dist0 > 0) {
				v_m_x = (dotArray[i - 1][0] - dotArray[i - 2][0]) / dist0;
				v_m_y = (dotArray[i - 1][1] - dotArray[i - 2][1]) / dist0;
			}
			//printf("v_m_x %f \n", v_m_x);
			//printf("v_m_y %f \n", v_m_y);

			float v_x = 0;
			float v_y = 0;
			if (dist2 > 0) {
				v_x = (dist1 * v_m_x + dist0 * v_p_x) / dist2;
				v_y = (dist1 * v_m_y + dist0 * v_p_y) / dist2;
			}

			//printf("v_x %f \n", v_x);
			//printf("v_y %f \n", v_y);

			cltpts[3 * i - 4][0] = dotArray[i - 1][0] - dist0 * v_x / 3;
			cltpts[3 * i - 4][1] = dotArray[i - 1][1] - dist0 * v_y / 3;
			cltpts[3 * i - 2][0] = dotArray[i - 1][0] + dist1 * v_x / 3;
			cltpts[3 * i - 2][1] = dotArray[i - 1][1] + dist1 * v_y / 3;
			cltpts[3 * i - 1][0] = dotArray[i][0];
			cltpts[3 * i - 1][1] = dotArray[i][1];
		}
	}
	LoadPointsIntoVBO();
	DeCasteljau(meshRes);
}


void Centri() {
	for (int i = 0; i < NumDots; i++) {
		cltpts[3 * i][0] = dotArray[i][0];
		cltpts[3 * i][1] = dotArray[i][1];
		if (i == 1) {
			cltpts[1][0] = dotArray[0][0];
			cltpts[1][1] = dotArray[0][1];
			cltpts[2][0] = dotArray[1][0];
			cltpts[2][1] = dotArray[1][1];
		}

		if (i > 1) {
			float dist1 = dist_sqrt(dotArray[i][0], dotArray[i - 1][0], dotArray[i][1], dotArray[i - 1][1]);
			float dist0 = dist_sqrt(dotArray[i - 1][0], dotArray[i - 2][0], dotArray[i - 1][1], dotArray[i - 2][1]);
			float dist2 = dist1 + dist0;
			//printf("*******dist1 %f \n", dist1);
			//printf("dist2 %f \n", dist2);
			//printf("dist0 %f \n", dist0);
			float v_p_x = 0;
			float v_p_y = 0;
			if (dist1 > 0) {
				v_p_x = (dotArray[i][0] - dotArray[i - 1][0]) / dist1;
				v_p_y = (dotArray[i][1] - dotArray[i - 1][1]) / dist1;
			}

			//printf("v_p_x %f \n", v_p_x);
			//printf("v_p_y %f \n", v_p_y);

			float v_m_x = 0;
			float v_m_y = 0;
			if (dist0 > 0) {
				v_m_x = (dotArray[i - 1][0] - dotArray[i - 2][0]) / dist0;
				v_m_y = (dotArray[i - 1][1] - dotArray[i - 2][1]) / dist0;
			}
			//printf("v_m_x %f \n", v_m_x);
			//printf("v_m_y %f \n", v_m_y);

			float v_x = 0;
			float v_y = 0;
			if (dist2 > 0) {
				v_x = (dist1 * v_m_x + dist0 * v_p_x) / dist2;
				v_y = (dist1 * v_m_y + dist0 * v_p_y) / dist2;
			}

			//printf("v_x %f \n", v_x);
			//printf("v_y %f \n", v_y);

			cltpts[3 * i - 4][0] = dotArray[i - 1][0] - dist0 * v_x / 3;
			cltpts[3 * i - 4][1] = dotArray[i - 1][1] - dist0 * v_y / 3;
			cltpts[3 * i - 2][0] = dotArray[i - 1][0] + dist1 * v_x / 3;
			cltpts[3 * i - 2][1] = dotArray[i - 1][1] + dist1 * v_y / 3;
			cltpts[3 * i - 1][0] = dotArray[i][0];
			cltpts[3 * i - 1][1] = dotArray[i][1];
		}
	}
	LoadPointsIntoVBO();
	DeCasteljau(meshRes);
}

void AddPoint(float x, float y)
{
	if (NumDots < MaxNumDots) {
		dotArray[NumDots][0] = x;
		dotArray[NumDots][1] = y;
		NumDots++;
		LoadPointsIntoVBO();
		switch (type){
			case 0:
				break;
			case 1:
				Catmull();
				break;
			case 2:
				Chord();
				break;
			case 3:
				Centri();
				break;
		}

	}
		
}

void ChangePoint(int i, float x, float y)
{
    assert(i >= 0 && i < NumDots);
    dotArray[i][0] = x;
    dotArray[i][1] = y;
	LoadPointsIntoVBO();
	switch (type) {
	case 0:
		break;
	case 1:
		Catmull();
		break;
	case 2:
		Chord();
		break;
	case 3:
		Centri();
		break;
	}
	
}

void RemoveFirstPoint()
{
    if (NumDots == 0) {
        return;
    }
    for (int i = 0; i < NumDots - 1; i++) {
        dotArray[i][0] = dotArray[i + 1][0];
        dotArray[i][1] = dotArray[i + 1][1];
    }
    NumDots--;
	LoadPointsIntoVBO();
    if (NumDots > 0) {
		switch (type) {
		case 0:
			break;
		case 1:
			Catmull();
			break;
		case 2:
			Chord();
			break;
		case 3:
			Centri();
			break;
		}
    }
}



void drawDot() {
	glUseProgram(shaderProgram1);
	glBindVertexArray(myVAO[iSeg]);
	glVertexAttrib3f(vertColor_loc, 0.0f, 0.0f, 0.0f);		// Black
	glDrawArrays(GL_POINTS, 0, NumDots);
	glBindVertexArray(myVBO[iSeg]);

}

void drawSegment() {
	glUseProgram(shaderProgram1);
	glBindVertexArray(myVAO[iSeg]);

	glLineWidth(5);
	// Draw the line segments
	if (NumDots > 0) {
		glVertexAttrib3f(vertColor_loc, 1.0f, 0.0f, 0.8f);		// Reddish magenta
		glDrawArrays(GL_LINE_STRIP, 0, NumDots);
	}

	// Draw the dots
	glBindVertexArray(myVBO[iSeg]);
}

void drawCtlPolygon() {
	glUseProgram(shaderProgram1);
	glBindVertexArray(myVAO[icltpts]);

	// Draw the line segments

	glLineWidth(2);
	if (NumDots > 0) {
		glVertexAttrib3f(vertColor_loc, 0.5f, 0.5f, 0.5f);
		glDrawArrays(GL_LINE_STRIP, 0, 3 * (NumDots - 1) + 1);
	}

	// Draw the dots
	glVertexAttrib3f(vertColor_loc, 0.5f, 0.5f, 0.5f);
	glDrawArrays(GL_POINTS, 0, 3 * (NumDots - 1) + 1);
	glBindVertexArray(myVBO[icltpts]);

	check_for_opengl_errors();
}

void drawCurve() {
	glUseProgram(shaderProgram1);
	glBindVertexArray(myVAO[iCurve]);

	// Draw the line segments
	glLineWidth(5);
	if (NumDots > 0) {
		glVertexAttrib3f(vertColor_loc, 1.0f, 0.0f, 0.8f);
		glDrawArrays(GL_LINE_STRIP, 0, meshRes * (NumDots - 1) + 1);

	}

//	glVertexAttrib3f(vertColor_loc, 0.5f, 0.5f, 0.5f);
	//glDrawArrays(GL_POINTS, 0, meshRes * (NumDots - 1) + 1);
	glBindVertexArray(myVBO[iCurve]);

	check_for_opengl_errors();
}

// *************************************
// Main routine for rendering the scene
// myRenderScene() is called every time the scene needs to be redrawn.
// mySetupGeometries() has already created the vertex and buffer objects, etc.
// setup_shaders() has already created the shader programs.
// *************************************


void myRenderScene() {

	// Clear the rendering window
    static const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, white);
    const float clearDepth = 1.0f;
    glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a pointer to the depth value!

    if (NumDots == 0) {
        return;
    }
	
	if (type == 0) {
		drawSegment();
	}
	else {
		drawCurve();
	}
	
	if (type && polygon) {
		drawCtlPolygon();
	}
	
	drawDot();
    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void RemoveLastPoint()
{
	NumDots = (NumDots > 0) ? NumDots - 1 : 0;
	// No need to load points into the VBO again: they are already loaded.
	myRenderScene();
}


void my_setup_SceneData() {
	mySetupGeometries();
	setup_shaders();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		return;			// Ignore key up (key release) events
	}
	if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_X) {
		glfwSetWindowShouldClose(window, true);
	}
    else if (key == GLFW_KEY_F) {
        if (selectedVert != 0) {   // Don't allow removing "selected" vertex
            RemoveFirstPoint();
            selectedVert = (selectedVert<0) ? selectedVert : selectedVert - 1;
        }
    }
    else if (key == GLFW_KEY_L) {
        if (selectedVert < NumDots - 1) {   // Don't allow removing "selected" vertex
            RemoveLastPoint();
        }
    }
	else if (key == GLFW_KEY_C) {
		polygon = !polygon;
	}
	else if (key == GLFW_KEY_0) {
		type = 0;
		myRenderScene();
	}
	else if (key == GLFW_KEY_1) {
		type = 1;
		Catmull();
	}
	else if (key == GLFW_KEY_2) {
		type = 2;
		Chord();
	}
	else if (key == GLFW_KEY_3) {
		type = 3;
		Centri();
	}
}

// *******************************************************
// Process all mouse button events.
// This routine is called each time a mouse button is pressed or released.
// *******************************************************
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // Scale x and y values into the range [-1,1]. 
        // Note that y values are negated since mouse measures y position from top of the window
        float dotX = (2.0f*(float)xpos / (float)(windowWidth-1)) - 1.0f;
        float dotY = 1.0f - (2.0f*(float)ypos / (float)(windowHeight-1));
        AddPoint(dotX, dotY);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            assert(selectedVert == -1);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float dotX = (2.0f*(float)xpos / (float)(windowWidth - 1)) - 1.0f;
            float dotY = 1.0f - (2.0f*(float)ypos / (float)(windowHeight - 1));
            // Find closest extant point, if any. (distances minDist and thisDist are measured in pixels)
            float minDist = 10000.0f;
            int minI;
            for (int i = 0; i < NumDots; i++) {
                float thisDistX = 0.5f*(dotX - dotArray[i][0])*(float)windowWidth;
                float thisDistY = 0.5f*(dotY - dotArray[i][1])*(float)windowHeight;
                float thisDist = sqrtf(thisDistX*thisDistX + thisDistY * thisDistY);
                if (thisDist < minDist) {
                    minDist = thisDist;
                    minI = i;
                }
            }
            if (minDist <= 4.0) {      // If clicked within 4 pixels of the vertex
                selectedVert = minI;
                ChangePoint(selectedVert, dotX, dotY);
            }
        }
        else if (action == GLFW_RELEASE) {
            selectedVert = -1;
        }
    }
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    if (selectedVert == -1) {
        return;
    }
    float dotX = (2.0f*(float)x / (float)(windowWidth - 1)) - 1.0f;
    float dotY = 1.0f - (2.0f*(float)y / (float)(windowHeight - 1));

    ChangePoint(selectedVert, dotX, dotY);
}



// *************************************************
// This function is called with the graphics window is first created,
//    and again whenever it is resized.
// The Projection View Matrix is typically set here.
//    But this program does not use any transformations or matrices.
// *************************************************
void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);		// Draw into entire window
    windowWidth = width;
    windowHeight = height;
}

void my_setup_OpenGL() {
	
	glEnable(GL_DEPTH_TEST);	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders

// TRY IT OUT: How do the results look different if you disable the next block
// of code. (By changing "#if 1" to "#if 0"
#if 1
	// The following commands should induce OpenGL to create round points and 
	//	antialias points and lines.  (This is implementation dependent unfortunately.)
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

	// Specify the diameter of points, and the width of lines. Measured in pixels.
    // Results can be implementation dependent.
	// TRY IT OUT: Experiment with increasing and decreasing these values.
	glPointSize(8);
	glLineWidth(5);
	
}

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window) {
	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	// Set callback for key up/down/repeat events and for mouse button events
	glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Set callbacks for mouse movement (cursor position).
    glfwSetCursorPosCallback(window, cursor_pos_callback);
}

// **********************
// Here is the main program
// **********************

int main() {
	glfwSetErrorCallback(error_callback);	// Supposed to be called in event of errors. (doesn't work?)
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	const int initWidth = 800;
	const int initHeight = 600;
	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, "ConnectDotsModern", NULL, NULL);
	if (window == NULL) {
		fprintf(stdout, "Failed to create GLFW window!\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewInit();

	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
    printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));

	printf("------------------------------\n");
	printf("Left-click with mouse to add points.\n");
    printf("Right-click and hold and move mouse to select and move vertices.\n");
    printf("Press 'f' or 'l' to remove the first point or the last point.\n");
    printf("Maximum of %d points permitted.\n", MaxNumDots);
    printf("Press ESCAPE or 'X' or 'x' to exit.\n");
	
    setup_callbacks(window);
    window_size_callback(window, initWidth, initHeight);

	// Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
	my_setup_SceneData();
 
    // Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {
	
		myRenderScene();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		// Poll events (key presses, mouse events)
		glfwWaitEvents();					// Use this if no animation.
		//glfwWaitEventsTimeout(1.0/60.0);	// Use this to animate at 60 frames/sec (timing is NOT reliable)
		// glfwPollEvents();				// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[8][36] = {
    "Unknown OpenGL error",
    "GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
    "GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
    "GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW" };
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
        }
        printf("OpenGL ERROR: %s.\n", errNames[errNum]);
    }
    return (numErrors != 0);
}