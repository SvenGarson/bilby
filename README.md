# bilby
Bilby is a minimalist library to generate texture and rendering information for ASCII character pixel font rendering

## The big picture
- Compatibility
  + Generate texture in an RGBA format that OpenGL understands given the right information
- Memory
  + Allocate a texture for every instance to leave room for future changes and for total instance separation
  + Allow user to specify the max number of glyphs recorded when rendering text including growing and shrinking
- Interface
  + Lifecycle
    - Create instance with a dedicated texture instance including a buffer of n desired information structs
    - Destroy instance with texture and info buffer
  + Setting
    - Font height to nearest multiple of pixel font height
    - Horizontal and vertical glyph distance
    - Max number of info buffer length + re-allocation 
  + Queries
    - get texture information: pointer; pixels; size of; components, etc.
    - current font height
    - current max number of infos
    - Iteration of generated info
  + Generation
    - Rendering information
      + rows
      + longest line
      + infos
      + etc.
  + Constants
    - ...
  + Additional functionality
    - Generate background information to draw glyphs over
    - Capping render info for:
      + number of glyphs
      + max width and/or height (pixels and glyphs) i.e. AABB

- Documentation and logic
  + Which OpenGL texture type to use - 8-bit unsigned RGBA
  + What an instance is
  + Explain the texture, scaling and the sizing of the texture etc.
  + Tinting the font and rendering backgrou
  + Coordinate system basis for the generated texture
  + Exact character set and what is controlling and what is renderable
  + Effects on memory when using the interface
  + Example of the in-memory texture

- Q & A
  + Which C standard to use? Pick the oldest one that makes sense, probably ANSI-C
  + Specify the coordinate system y-axis for the texture and for the rendering info.
    Provide both options instead of deciding?
  + Force generated texture into the right power of 2 dimensions (lowest common denominator of API requirements)
  + Is the interface adequate to generate triangles from?
  + Does byte order matter in any way?
  + How to enable styled rendering like underline; lin1e-through; background color; foreground color; borders and negatives?

- Finishing up
  + Handle all TODOs
  + Clean documenation that is enforced by automatic tests
  + Automatic test suite: Interface; Memory leaks; Different platforms


## Example usage

```C
/* Create and destroy instances */
const bilby * const p_info = bilby_init(initial_max_number_of_infos);
bilby_destroy(p_info); /* Clear the instance so the interface knows if initialized or not */

/* Adjust the max number of infos to cache up - Screws with the memory when shrunk or used to often */
bilby_set_character_cap(p_info, 128);
int current_character_cap = bilby_get_character_cap(p_info);

/* Accessing the texture - This should be a struct with all information required for uploading the texture */
unsigned char * p_texture = bilby_get_texture(p_info); /* Pointer to start of texture - use RGBA with set transparency */

/* Set and query font height */
int selected_font_height = bilby_set_font_height(p_info, 18); /* Round to closest multiple of pixel font height */
int current_font_height = bilby_get_font_height(p_info);

/* Set spacing based on texture pixel size - 1 unit corresponds to a single texture pixel before scaling */
bilby_set_horizontal_spacing(p_info, new_hori_spacing);
int current_hori_spacing = bilby_get_horizontal_spacing(p_info);
bilby_set_vertical_spacing(p_info, new_hori_spacing);
int current_hori_spacing = bilby_set_vertical_spacing(p_info);

/*
    Generate text rendering information where each info has attributes:
      - Glyph AABB (exact width to scale the texture linearly across both axis)
      - tex coords for every vertex of the AABB
      ? where is down in the coordinate system? have two options?
      - index maybe

    Where the base is the base positon (top or bottom?) to mark positions as
*/
bilby_text_render_info(p_info, base_x, base_y);

/* Iterate to access the information structs */
const int infos_generated = bilby_glyphs_rendered(p_info);
while (bilby_has_render_info(p_info)) /* Re-sets iterator */
{
  const bilby_render_info * const p_render_info = bilby_next_render_info(p_info);
  /* Render here */
}

```
