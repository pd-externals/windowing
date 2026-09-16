/* gaussian~ - gaussian windowing function for Pure Data 
**
** SPDX-FileCopyrightText: © 2002, Joseph A. Sarlo <jsarlo@mambo.peabody.jhu.edu>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

#define DEFDELTA 0.5

static t_class *gaussian_class;

typedef struct _gaussian {
  t_windowing x_w;
  t_float x_delta;
} t_gaussian;


static void fillGaussian(t_gaussian *obj, t_sample *vec, size_t n) {
  size_t i;
  t_sample xShift = (t_sample)n / 2;
  t_sample x;
  t_sample delta = obj->x_delta;
  if (delta == 0) {
    delta = 1;
  }
  for (i = 0; i < n; i++) {
    x = (i - xShift) / xShift;
    vec[i] = (t_sample)(pow(2, (-1 * (x / delta) * (x / delta))));
  }
}

static void gaussian_float(t_gaussian *x, t_float delta) {
  if (delta != 0) {
    x->x_delta = delta;
    windowing_rebuildtable(&x->x_w, x->x_w.x_tablesize, x->x_w.x_overlap);
  }
}

static void* gaussian_new(t_float delta) {
  t_gaussian *x = (t_gaussian *)windowing_new(gaussian_class);
  x->x_delta = (delta == 0)?DEFDELTA:delta;
  return (x);
}

void gaussian_tilde_setup(void) {
  gaussian_class = class_new(gensym("gaussian~"),
			    (t_newmethod)gaussian_new, 
			    (t_method)windowing_free,
    	                    sizeof(t_gaussian),
			    CLASS_MULTICHANNEL,
			    A_DEFFLOAT,
			    0);
  windowing_setupclass(gaussian_class, (t_window_fill)fillGaussian);
  class_addfloat(gaussian_class, (t_method)gaussian_float);
}
