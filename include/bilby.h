#ifndef BILBY_H
#define BILBY_H

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Defines */
#define BILBY_ASCII_CHARACTERS (256)

/* Constants */
const int BILBY_SMALLEST_ASCII_CODE = 0;
const int BILBY_LARGEST_ASCII_CODE = BILBY_ASCII_CHARACTERS - 1;
const int BILBY_GLYPH_DESIGN_WIDTH = 5;
const int BILBY_GLYPH_DESIGN_HEIGHT = 9;
const int BILBY_GLYPH_DESIGN_SIZE = BILBY_GLYPH_DESIGN_WIDTH * BILBY_GLYPH_DESIGN_HEIGHT;

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
enum bilby_bool { 
  BILBY_FALSE = 0,
  BILBY_TRUE = 1
};

/* TODO-GS: Return the following struct as texture information to upload or use offline */
struct bilby_texture_info {
  unsigned char * p_pixels;
  int width;
  int height;
  int color_components_per_pixel;
  size_t size_in_bytes;
  int number_of_pixels;
};

struct bilby_vec2f {
  float x;
  float y;
};

struct bilby_region2d {
  struct bilby_vec2f min;
  struct bilby_vec2f max;
};

struct bilby_glyph_info {
  const char * p_glyph_design;
  struct bilby_region2d texcoords_region;
};

struct bilby_instance {
  /*
      - texture into
      - render info cache
      - settings
      - ... ?
  */
  struct bilby_glyph_info ascii_glyph_info[BILBY_ASCII_CHARACTERS];
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

  /* Establish ASCII glyph mapping to specify printable characters */
  for (int ascii_code = BILBY_SMALLEST_ASCII_CODE; ascii_code < BILBY_LARGEST_ASCII_CODE; ascii_code++)
  {
    struct bilby_glyph_info * p_glyph_info = p_bilby_instance->ascii_glyph_info + ascii_code;
    p_glyph_info->p_glyph_design = NULL;
  }

  /* Register printable ascii characters to glyph designs */
  p_bilby_instance->ascii_glyph_info['A'].p_glyph_design = P_GLYPH_DESIGN_A;
  p_bilby_instance->ascii_glyph_info['B'].p_glyph_design = P_GLYPH_DESIGN_B;
  p_bilby_instance->ascii_glyph_info['C'].p_glyph_design = P_GLYPH_DESIGN_C;

  /* Build the ascii glyph texture */
  /*
      To avoid texture bleeding, every glyph design region is surrounded by at least a single pixel
      that is not used for glyph texture data

      1  5  1  5  1  5  1
      +-----+-----+-----+ 1
      |.###.|####.|.###.|
      |#...#|#...#|#...#|
      |#...#|#...#|#....|
      |#...#|####.|#....|
      |#####|#...#|#....|  9
      |#...#|#...#|#...#|
      |#...#|####.|.###.|
      |.....|.....|.....|
      |.....|.....|.....|
      +-----+-----+-----+ 1

  */
  const int GLYPH_DESIGN_PADDING = 1; /* TODO-GS: Padding not working as expected */
  const int HORIZONTAL_TEXTURE_GLYPHS = 25;
  const int VERTICAL_TEXTURE_GLYPHS = 4;
  const int TEXTURE_WIDTH = HORIZONTAL_TEXTURE_GLYPHS * (BILBY_GLYPH_DESIGN_WIDTH + GLYPH_DESIGN_PADDING) + GLYPH_DESIGN_PADDING;
  const int TEXTURE_HEIGHT = VERTICAL_TEXTURE_GLYPHS * (BILBY_GLYPH_DESIGN_HEIGHT + GLYPH_DESIGN_PADDING) + GLYPH_DESIGN_PADDING;
  const int TEXTURE_TEXELS = TEXTURE_WIDTH * TEXTURE_HEIGHT;
  const int TEXTURE_COLOR_COMPONENTS = 4;

  /* Allocate and initialize the pixel data as fully transparent plane - TODO-GS: Enforce power of two size */
  const size_t TEXTURE_PIXEL_DATA_SIZE_IN_BYTES = sizeof(unsigned char) * TEXTURE_COLOR_COMPONENTS * TEXTURE_TEXELS;
  unsigned char * p_texture_pixels = malloc(TEXTURE_PIXEL_DATA_SIZE_IN_BYTES);
  if (p_texture_pixels == NULL)
    return NULL;

  for (int pixel_index = 0; pixel_index < TEXTURE_TEXELS; pixel_index++)
  {
    unsigned char * p_pixel_rgba = p_texture_pixels + (pixel_index * TEXTURE_COLOR_COMPONENTS);
    p_pixel_rgba[0] = 0x00; /* Red */
    p_pixel_rgba[1] = 0x00; /* Green */
    p_pixel_rgba[2] = 0x00; /* Blue */
    p_pixel_rgba[3] = 0xFF; /* Alpha TODO-GS: Reset to transparent after debugging */
  }

  /*
      Initialized glyph texture - Now plot the glyph designs for all printable characters
      and track the texcoords for later retrieval.
  */
  int texture_glyph_cell_x = 0;
  int texture_glyph_cell_y = 0;
  for (int ascii_code = BILBY_SMALLEST_ASCII_CODE; ascii_code < BILBY_LARGEST_ASCII_CODE; ascii_code++)
  {
    /* Ignore ascii codes on non-printable characters for texture generation */
    struct bilby_glyph_info * const p_glyph_info = p_bilby_instance->ascii_glyph_info + ascii_code;
    if (p_glyph_info->p_glyph_design == NULL)
      continue;

    /* Make sure the glyph design has the correct length - TODO-GS: If not, then plot some debuggin thing instead? */
    const size_t actual_glyph_design_size = strlen(p_bilby_instance->ascii_glyph_info[ascii_code].p_glyph_design);
    if (actual_glyph_design_size != (unsigned int)BILBY_GLYPH_DESIGN_SIZE)
    {
      printf(
        "\nAscii code '%d' glyph has '%d' instead of '%d' design points!",
        ascii_code,
        (int)actual_glyph_design_size,
        BILBY_GLYPH_DESIGN_SIZE
      );
      continue;
    }

    /* Make sure there is a vacant texture glyph cell */
    if (texture_glyph_cell_x >= HORIZONTAL_TEXTURE_GLYPHS || texture_glyph_cell_y >= VERTICAL_TEXTURE_GLYPHS)
    {
      printf("\nNo more texture glyph cell at glyph coordinate [%-3d, %-3d]", texture_glyph_cell_x, texture_glyph_cell_y);
      break;
    }

    /* Ascii code is printable character with a valid design - Plot it into the texture glyph cell */
    /* TODO-GS: Make padding work properly and get rid of the magic ones - Padding should work automatically with the single constant */
    const int glyph_texel_min_x = texture_glyph_cell_x * (BILBY_GLYPH_DESIGN_WIDTH + GLYPH_DESIGN_PADDING) + 1;
    const int glyph_texel_min_y = texture_glyph_cell_y * (BILBY_GLYPH_DESIGN_HEIGHT + GLYPH_DESIGN_PADDING) + 1;
    const int glyph_texel_max_x = glyph_texel_min_x + (BILBY_GLYPH_DESIGN_WIDTH - 1);
    const int glyph_texel_max_y = glyph_texel_min_y + (BILBY_GLYPH_DESIGN_HEIGHT - 1);

    /* Iterate the glyph cell texture region and plot every set texel from glyph design */
    int glyph_design_char_index = 0;
    for (int glyph_plot_y = glyph_texel_min_y; glyph_plot_y <= glyph_texel_max_y; glyph_plot_y++)
    {
     for (int glyph_plot_x = glyph_texel_min_x; glyph_plot_x <= glyph_texel_max_x; glyph_plot_x++)
     {
       /* Only plot the set design characters */
       const char * p_glyph_design_char = p_glyph_info->p_glyph_design + glyph_design_char_index++;
       if ('#' != *p_glyph_design_char)
         continue;

        /* Plot pixel into texture */
        const int glyph_plot_texel_index = (glyph_plot_y * TEXTURE_WIDTH) + glyph_plot_x;
        unsigned char * p_glyph_plot_texel = p_texture_pixels + (glyph_plot_texel_index * TEXTURE_COLOR_COMPONENTS);
        p_glyph_plot_texel[0] = 0xFF; /* Red */
        p_glyph_plot_texel[1] = 0xFF; /* Green */
        p_glyph_plot_texel[2] = 0xFF; /* Blue */
        p_glyph_plot_texel[3] = 0xFF; /* Alpha */
      }
    }

    /* Associate ASCII code to the plotted texture region - TODO-GS: Decide on texture coordinate system here and texte y-axis */
    p_glyph_info->texcoords_region.min.x = ((float)glyph_texel_min_x + 0.5f) / (float)TEXTURE_WIDTH;
    p_glyph_info->texcoords_region.min.y = ((float)glyph_texel_min_y + 0.5f) / (float)TEXTURE_HEIGHT;
    p_glyph_info->texcoords_region.max.x = ((float)glyph_texel_max_x + 0.5f) / (float)TEXTURE_WIDTH;
    p_glyph_info->texcoords_region.max.y = ((float)glyph_texel_max_y + 0.5f) / (float)TEXTURE_HEIGHT;

    /* Choose next texture glyph cell */
    texture_glyph_cell_x++;
    if (texture_glyph_cell_x >= HORIZONTAL_TEXTURE_GLYPHS)
    {
      texture_glyph_cell_x = 0;
      texture_glyph_cell_y++;
    }
  }

  /* Keep track of generated texture attributes */
  p_bilby_instance->texture_info.p_pixels = p_texture_pixels;
  p_bilby_instance->texture_info.width = TEXTURE_WIDTH;
  p_bilby_instance->texture_info.height = TEXTURE_HEIGHT;
  p_bilby_instance->texture_info.color_components_per_pixel = TEXTURE_COLOR_COMPONENTS;
  p_bilby_instance->texture_info.size_in_bytes = TEXTURE_PIXEL_DATA_SIZE_IN_BYTES;
  p_bilby_instance->texture_info.number_of_pixels = TEXTURE_TEXELS;

  /* TODO-GS: Output texture as text for debugging */
  for (int tex_row = 0; tex_row < TEXTURE_HEIGHT; tex_row++)
  {
    printf("\n");
    for (int tex_col = 0; tex_col < TEXTURE_WIDTH; tex_col++)
    {
      const int texel_index = (tex_row * TEXTURE_WIDTH) + tex_col;
      unsigned char * p_texel_rgba = p_texture_pixels + (texel_index * TEXTURE_COLOR_COMPONENTS);
      if (p_texel_rgba[3] == 0xFF)
        putchar('#');
      else
        putchar('.');
    }
  }

  /* Success - Return established bilby instance */
  return p_bilby_instance;
}

void bilby_destroy_instance(struct bilby_instance ** pp_instance)
{
  if (pp_instance == NULL || *pp_instance)
    return;

  /* Deallocate all instance resources */
  free((*pp_instance)->texture_info.p_pixels);
  free(*pp_instance);

  /* Set external reference to mark isntance as destroyed */
  *pp_instance = NULL;
}

/* Interface function prototypes - Queries */
const struct bilby_texture_info * bilby_texture_info(struct bilby_instance * p_instance)
{
  return &p_instance->texture_info;
}

#endif