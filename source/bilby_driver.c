/*
    Driver source for testing the bilby implemenation
*/
#include "../include/bilby.h"
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

/* Constants */
const float CANVAS_REGION = 200.0f;

/* State */
struct bilby_instance * p_instance = NULL;
const struct bilby_texture_info * p_tex_info = NULL;

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* Blending */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

  /* Check texture */
  p_tex_info = bilby_texture_info(p_instance);

  /* Uploade texture */
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    p_tex_info->width,
    p_tex_info->height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    p_tex_info->p_pixels
  );
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

  /* Rendering the texture */
  const float inset = 10.0f;
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(p_tex_info->width, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(p_tex_info->width, p_tex_info->height);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, p_tex_info->height);
  glEnd();

  /* Rendering the text data */
  bilby_render_text(p_instance, "ABC", 50, 50);
  glBegin(GL_QUADS);
  for (int i = 0; i < p_instance->render_infos_cached; i++)
  {
    const struct bilby_glyph_render_info * p_render_info = p_instance->render_infos + i;
    glTexCoord2f(p_render_info->texcoords_region.min.x, p_render_info->texcoords_region.max.y);
    glVertex2i(p_render_info->render_region.min.x, p_render_info->render_region.min.y);

    glTexCoord2f(p_render_info->texcoords_region.max.x, p_render_info->texcoords_region.max.y);
    glVertex2i(p_render_info->render_region.max.x, p_render_info->render_region.min.y);

    glTexCoord2f(p_render_info->texcoords_region.max.x, p_render_info->texcoords_region.min.y);
    glVertex2i(p_render_info->render_region.max.x, p_render_info->render_region.max.y);

    glTexCoord2f(p_render_info->texcoords_region.min.x, p_render_info->texcoords_region.min.y);
    glVertex2i(p_render_info->render_region.min.x, p_render_info->render_region.max.y);
  }
  glEnd();

  /* Swap buffers */
  glutSwapBuffers();
}

void ListOpenglErrors(const char * const p_tag)
{
  GLenum error = glGetError();
  if (error == GL_NO_ERROR)
    return;

  printf("\nOpenGL errors tagged '%s'", p_tag);
  int error_index = 0;
  do {
    printf("\n\t[%-2d] Error: %s", error_index++, gluErrorString(error));
  } while ((error = glGetError()) != GL_NO_ERROR);
  printf("\n");
}

void TimerFunction(int value)
{
  /* Re-draw the scene */
  ListOpenglErrors("OpenGL errors:");
  glutPostRedisplay();
  glutTimerFunc(33, TimerFunction, 1);
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
  glutReshapeWindow(1200, 800);
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