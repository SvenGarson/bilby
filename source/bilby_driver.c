/*
    Driver source for testing the bilby implemenation
*/
#include "../include/bilby.h"
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

/* Constants */
const float CANVAS_REGION = 100.0f;

/* State */
struct bilby_instance * p_instance = NULL;

/* Function definitions */
void SetupRC(void)
{
  /* Bilby */
  p_instance = bilby_create_instance();

  /* OpenGL */
  glClearColor(0.25f, 0.50f, 0.75f, 1.0f);

  /* Texturing */
  glEnable(GL_TEXTURE_2D);
  GLuint bilby_texture_handle;
  glGenTextures(1, &bilby_texture_handle);
  glBindTexture(GL_TEXTURE_2D, bilby_texture_handle);

  const unsigned char pixels[] = {
    0xFF, 0x00, 0x00, 0xFF,
    0x00, 0xFF, 0x00, 0xFF,
    0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0x00, 0xFF, 0xFF,
  };

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void cleanup(void)
{
  bilby_destroy_instance(&p_instance);
}

void ChangeSize(GLsizei w, GLsizei h)
{
  GLfloat aspectRatio;

  /* Prevent divide by zero */
  if (h == 0)
    h = 1;

  /* Set viewport to window dimensions */
  glViewport(0, 0, w, h);

  /* Adjust coordinate system */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* Specify clipping volume */
  aspectRatio = (GLfloat)w / (GLfloat)h;
  if (w <= h)
  {
    /* Tall or square window */
    glOrtho(-CANVAS_REGION, CANVAS_REGION, -CANVAS_REGION / aspectRatio, CANVAS_REGION / aspectRatio, 1.0, -1.0);
  }
  else
  {
    /* Wide window */
    glOrtho(-CANVAS_REGION * aspectRatio, CANVAS_REGION * aspectRatio, -CANVAS_REGION, CANVAS_REGION, 1.0, -1.0);
  }

  /* Set and clear active matrix */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void RenderScene(void)
{
  /* Clear framebuffer */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Rendering stuff */
  const float inset = 10.0f;
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-CANVAS_REGION + inset, -CANVAS_REGION + inset);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f( CANVAS_REGION - inset, -CANVAS_REGION + inset);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f( CANVAS_REGION - inset,  CANVAS_REGION - inset);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-CANVAS_REGION + inset,  CANVAS_REGION - inset);
  glEnd();

  /* Swap buffers */
  glutSwapBuffers();
}

void TimerFunction(int value)
{
  /* Re-draw the scene */
  glutPostRedisplay();
  glutTimerFunc(33, TimerFunction, 1);
}

void ListOpenglErrors(const char * const p_tag)
{
  GLenum error = glGetError();
  if (error == GL_NO_ERROR)
  {
    printf("\nNo OpenGL errors tagged '%s'\n", p_tag);
    return;
  }

  printf("\nOpenGL errors tagged '%s'", p_tag);
  int error_index = 0;
  do {
    printf("\n\t[%-2d] Error: %s", error_index++, gluErrorString(error));
  } while ((error = glGetError()) != GL_NO_ERROR);
  printf("\n");
}

void InputHandler(unsigned char key, int x, int y)
{
  if (key == 27)
    glutLeaveMainLoop();
}


int main(int argc, char * argv[])
{
  /* Exit callback */
  atexit(cleanup);

  /* Initialize Freeglut */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutCreateWindow("Learning OpenGL");
  glutDisplayFunc(RenderScene);
  glutReshapeFunc(ChangeSize);
  glutTimerFunc(33, TimerFunction, 1);

  /* Handle keyboard events */
  glutKeyboardFunc(InputHandler);

  /* Initialize */
  SetupRC();

  /* Kick of the gameloop */
  glutMainLoop();

  /* To OS successfully */
  return 0;
}