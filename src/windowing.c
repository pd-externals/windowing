/*
**  windowing.c - generic framework to implement windowing objects
**
** SPDX-FileCopyrightText: © 2002, Joseph A. Sarlo <jsarlo@mambo.peabody.jhu.edu>
** SPDX-FileCopyrightText: © 2026, IOhannes m zmölnig <zmoelnig@iem.at>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

static t_int* windowing_perform(t_int *w) {
  t_sample *in = (t_sample *)(w[1]);
  t_sample *win = (t_sample *)(w[2]);
  t_sample *out = (t_sample *)(w[3]);
  int n = (int)(w[4]);
  int i;
  for (i = 0; i < n; i++) {
    *out++ = *(in++) * win[i];
  }
  return (w + 5);
}

static void windowing_dsp(t_windowing *x, t_signal **sp) {
  size_t blocksize = (sp[0]->s_n > 0)?sp[0]->s_n:1;
  if(x->x_fill && x->x_blocksize != blocksize) {
    size_t i;
    t_sample sum = 0;
    x->x_table = resizebytes (x->x_table, x->x_blocksize * sizeof(*x->x_table), blocksize * sizeof(*x->x_table));
    x->x_blocksize = blocksize;
    x->x_fill(x, x->x_table, blocksize);
    for(i=0; i<blocksize; i++) {
      sum += x->x_table[i]*x->x_table[i];
    }
    x->x_makeup = 1./sqrt(sum / (t_sample)blocksize);
  }
  if (x->x_table && x->x_blocksize == blocksize)
    dsp_add(windowing_perform, 4, sp[0]->s_vec, x->x_table, sp[1]->s_vec, sp[0]->s_n);
  else
    dsp_add_zero(sp[1]->s_vec, sp[0]->s_n);
}

void windowing_free(t_windowing *x) {
  freebytes(x->x_table, x->x_blocksize * sizeof(*x->x_table));
}

t_windowing* windowing_new(t_class *cls) {
  t_windowing *x = (t_windowing *)pd_new(cls);
  x->x_blocksize = 0;
  x->x_table = getbytes(x->x_blocksize * sizeof(*x->x_table));
  x->x_fill = (t_window_fill)zgetfn(&cls, gensym("windowfill"));
  if(!x->x_fill) {
    pd_error(x, "no table implementation found!");
  }

  outlet_new(&x->x_obj, gensym("signal"));
  return x;
}

t_class *windowing_setupclass(
  t_class *cls,
  t_window_fill fillfun) {
  class_addmethod(cls, nullfn, gensym("signal"), 0);
  class_addmethod(cls, (t_method)windowing_dsp, gensym("dsp"), A_CANT, 0);
  class_addmethod(cls, (t_method)fillfun, gensym("windowfill"), A_CANT, 0);

  return cls;
}
