/* windowing.h - generic framework to implement windowing objects
**
** SPDX-FileCopyrightText: © 2026, IOhannes m zmölnig
** SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef INCLUDE_WINDOWING_H
#define INCLUDE_WINDOWING_H 1

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <m_pd.h>
#include <math.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
#ifndef DEFBLOCKSIZE
# define DEFBLOCKSIZE 64
#endif

#ifndef CLASS_MULTICHANNEL
# define CLASS_MULTICHANNEL 0
#endif

#ifdef UNUSED
# undef UNUSED
#endif
#if defined(__GNUC__)
# define UNUSED __attribute__ ((__unused__))
/* https://stackoverflow.com/questions/67452447, but i haven't tested this...
#elif defined _MSC_VER
# define UNUSED __pragma(warning(suppress: 4505))
*/
#else
# define UNUSED
#endif

struct _windowing;
/* fill the window function into the <vec> array of size <n> */
typedef void (*t_window_fill)(struct _windowing *x, t_sample *vec, size_t n);

/* subclass this for your own windowclasses */
typedef struct _windowing {
  t_object x_obj;
  t_window_fill x_fill;
  size_t x_tablesize;
  t_sample *x_table;
  size_t x_overlap; /* cache overlap factor of the incoming signal */
  t_sample x_makeup; /* make up gain */
  int x_normalize; /* should we normalize? */
  t_float x_f;
} t_windowing;

/* free private data; must be called in the destructor of the class */
void windowing_free(t_windowing *x);
/* allocates private data; use this instead of pd_new() */
t_windowing* windowing_new(t_class *cls);
/* register DSP functions and the class's window fill function */
t_class *windowing_setupclass(t_class *cls, t_window_fill fillfun);


/* rebuild the window
   - resize table
   - fill table with window
   - calculate makeup gain for the given overlap (and store overlap in x_overlap)
*/
void windowing_rebuildtable(t_windowing *x, size_t tablesize, size_t overlap);

/* helpers to setup an entire windowing class like so:
   static void fillLanczos(t_windowing *x, t_sample *vec, size_t n) { ... }
   WINDOWING_SETUP(lanczos, fillLanczos);
 */
UNUSED static t_class *windowing_class;
UNUSED static void* windowing_do_new(void) {
  return windowing_new(windowing_class);
}

#define WINDOWING_SETUP(name, fillfun)					\
  void name##_tilde_setup(void) {					\
    t_symbol*classname = gensym(#name"~");				\
    windowing_class = class_new(					\
      classname,							\
      (t_newmethod)windowing_do_new, (t_method)windowing_free,		\
      sizeof(t_windowing),						\
      CLASS_MULTICHANNEL, 0);						\
    windowing_setupclass(windowing_class, fillfun);			\
  }

#endif /* INCLUDE_WINDOWING_H */
