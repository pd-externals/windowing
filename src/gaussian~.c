/* gaussian~ - gaussian windowing function for Pure Data 
**
** Copyright (C) 2002 Joseph A. Sarlo
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
** jsarlo@mambo.peabody.jhu.edu
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
    fillGaussian(x, x->x_w.x_table, x->x_w.x_blocksize);
  }
}

static void* gaussian_new(float delta) {
  t_gaussian *x = (t_gaussian *)windowing_new(gaussian_class);
  x->x_delta = (delta == 0)?DEFDELTA:delta;
  return (x);
}

void gaussian_tilde_setup(void) {
  filename = __FILE__;

  gaussian_class = class_new(gensym("gaussian~"),
			    (t_newmethod)gaussian_new, 
			    (t_method)windowing_free,
    	                    sizeof(t_gaussian),
			    0,
			    A_DEFFLOAT,
			    0);
  windowing_setup(gaussian_class, 0, (t_window_fill)fillGaussian);
  class_addfloat(gaussian_class, (t_method)gaussian_float);
}
