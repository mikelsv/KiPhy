#include <GL/glut.h>

// Ki Resolution
int width = 1280, height = 720;

// This is the number of frames per second to render.
static const int FPS = 60;

// Ki Global Timers
int ki_frames = 0, ki_time = 0, ki_time_delta;

// Include Ki Physics
#include "ki_phy.h"

// Colors
MRGB ki_colors[] = {"FF0000", "FF7F00", "FFFF00", "00FF00", "00FFFF", "0000FF", "7F00FF", "FF00FF"};

// Draw function
void display(){

	// Time
	int t = glutGet(GLUT_ELAPSED_TIME);

	ki_time_delta = t - ki_time;
	ki_time = t;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, 100, -100);

	// Set every pixel in the frame buffer to the current clear color.
	glClear(GL_COLOR_BUFFER_BIT);

	// Ki Draw
	ki_phy.Draw();

	// Flush drawing command buffer to make drawing happen as soon as possible.
	glFlush();

	// Swap buffer
	glutSwapBuffers();

	// Frame count
	ki_frames ++;
}

// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again, provided the program is in the spinning state.
// Since the timer function is only called once, it sets the same function to
// be called again.
void timer(int v){
  // Draw
  glutPostRedisplay();

  // Again
  glutTimerFunc(1000 / FPS, timer, v);
}


// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char** argv){

  // Use a single buffered window in RGB mode (as opposed to a double-buffered
  // window or color-index mode).
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  // Position window at (80,80)-(480,380) and give it a title.
  //glutInitWindowPosition(80, 80);
  glutInitWindowSize(width, height);
  glutCreateWindow("Ki Physics");

  // Tell GLUT that whenever the main window needs to be repainted that it
  // should call the function display().
  glutDisplayFunc(display);
  glutMouseFunc(glut_mouse);
  glutMotionFunc(glut_mouse_move);

  glutTimerFunc(100, timer, 0);

  // Ki Init
  ki_phy.Init(width, height);

  // Enable combine
  ki_phy.flag_combine = 1;

  // Add points
	KiGroup group;
	int st = 1, sti = 0;
	group.pos = KiVec2(.5, .5);
	group.size = 10;

	for(int i = 0; i < 8; i ++){
		group.radius = .04 * st; group.rotate = PI * sti / st; group.col = ki_colors[i];
		ki_phy.AddRingGroup(group);

		sti ++;
		if(sti >= st){
			st ++;
			sti = 0;

			if(st == 5)
				st = 7;
		}

		//KiExpOneEndColor(ki_colors[i]);
	}


  // Tell GLUT to start reading and processing events.  This function
  // never returns; the program only exits when the user closes the main
  // window or kills the process.
  glutMainLoop();
}