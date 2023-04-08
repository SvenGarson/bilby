#ifndef BILBY_H
#define BILBY_H

/* Includes */
#include <stdlib.h>
#include <string.h>

/* Glyph designs */
const char * P_GLYPH_DESIGN_A = ".###."
                                "#...#"
                                "#...#"
                                "#...#"
                                "#####"
                                "#...#"
                                "#...#"
                                "....."
                                ".....";

const char * P_GLYPH_DESIGN_B = "####."
                                "#...#"
                                "#...#"
                                "####."
                                "#...#"
                                "#...#"
                                "####."
                                "....."
                                ".....";

const char * P_GLYPH_DESIGN_C = ".###."
                                "#...#"
                                "#...."
                                "#...."
                                "#...."
                                "#...#"
                                ".###."
                                "....."
                                ".....";

/* Datatypes */
struct bilby_texture_info {
  unsigned char * p_pixels;
  int width;
  int height;
  int color_components_per_pixel;
  size_t size_in_bytes;
  int number_of_pixels;
};

struct bilby_instance {
  /*
      - texture into
      - render info cache
      - settings
      - ... ?
  */
  struct bilby_texture_info texture_info;

};

/* Private helper functions - TODO-GS: Move to source file */

/* TODO-GS: Separate implementation into headers and source files */
/* Interface function prototypes - Lifecycle */
struct bilby_instance * bilby_create_instance(void)
{
  struct bilby_instance * p_bilby_instance = malloc(sizeof(struct bilby_instance));
  if (p_bilby_instance == NULL)
    return NULL;

  /* Generate textures based on character set */
  /* TODO-GS: Initialize a private static array to store the ascii to glyph mapping in the translation unit */


  /* TODO-GS: Initialize new texture instance texture information */

  /* TODO-GS: Initialize the rest of the instance */

  return p_bilby_instance;
}

void bilby_destroy_instance(struct bilby_instance ** pp_instance)
{
  if (pp_instance == NULL)
    return;

  /* TODO-GS: Deallocate all allocated resources for the given instance */
  free(*pp_instance);
  *pp_instance = NULL;
}

/* Interface function prototypes - Queries */
void bilby_rgba_texture(struct bilby_instance * p_instance)
{

}

#endif