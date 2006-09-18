#ifndef __DEMO_WINDOW__
#define __DEMO_WINDOW__

/** Show a given image in a window.
 *
 * @param name Window name.
 *
 * @param transformed Image to be show.
 *
 * TODO: make some widget resource management (I want to keep
 *       opened windows still active).
 */
void show_img(const char* name, void *img);



#endif
