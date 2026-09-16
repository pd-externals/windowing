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
  t_windowing *x = (t_windowing *)w[1];
  t_sample *in = (t_sample *)(w[2]);
  t_sample *out = (t_sample *)(w[3]);
  int n = (int)(w[4]);
  t_sample *win = x->x_table;
  t_sample gain = 1.;

  int i;

  if(x->x_normalize)
    gain = x->x_makeup;

  for (i = 0; i < n; i++) {
    *out++ = *(in++) * win[i] * gain;
  }
  return (w + 5);
}

void windowing_rebuildtable(t_windowing *x, size_t tablesize, size_t overlap) {
  if(overlap < 1)
    overlap = 1;
  x->x_overlap = overlap;
  x->x_makeup = 1.;

  /* reallocate table if necessary */
  if(x->x_tablesize != tablesize) {
    x->x_table = resizebytes (x->x_table, x->x_tablesize * sizeof(*x->x_table), tablesize * sizeof(*x->x_table));
    x->x_tablesize = tablesize;
  }
  /* fill table */
  if(x->x_fill)
    x->x_fill(x, x->x_table, tablesize);
  else {
    size_t i;
    for(i=0; i<tablesize; i++)
      x->x_table[i] = 1.;
  }

  /* calculate makeup gain */
  if(x->x_table && x->x_tablesize > 0) {
    t_sample sum = 0;
    size_t i;
    if (overlap > 1) {
      for(i=0; i<overlap; i++) {
	sum += x->x_table[i*(tablesize/overlap)];
      }
      x->x_makeup = (1.*overlap)/(sum);
    } else {
      for(i=0; i<tablesize; i++) {
	sum += x->x_table[i]*x->x_table[i];
      }
      x->x_makeup = 1./sqrt(sum / (t_sample)tablesize);
    }
  }
}

static void windowing_dsp(t_windowing *x, t_signal **sp) {
  int length = sp[0]->s_n;
  size_t tablesize = (length > 0)?length:1;

  int numchannels = 1;
  size_t overlap = 1;
  int totalsamples = length;
#if CLASS_MULTICHANNEL
  numchannels = sp[0]->s_nchans;
  overlap = (sp[0]->s_overlap>0)?sp[0]->s_overlap:1;
  totalsamples = length * numchannels;

  signal_setmultiout(&sp[1], numchannels);
#endif

  windowing_rebuildtable(x, tablesize, overlap);

  if (x->x_table && x->x_tablesize >= (length>0)?(size_t)length:0) {
    int offset = 0;
    for(offset=0; offset<numchannels; offset++) {
      t_sample *in = sp[0]->s_vec + offset * length;
      t_sample *out = sp[1]->s_vec + offset * length;
      dsp_add(windowing_perform, 4, x, in, out, length);
    }
  } else {
    dsp_add_zero(sp[1]->s_vec, totalsamples);
  }
}

static void windowing_normalize(t_windowing *x, t_float f) {
  int i=(int)f;
#if !CLASS_MULTICHANNEL
  static int shouldwarn = 1;
  if (i && shouldwarn) {
    pd_error(x, "windowing has been compiled without normalization support.");
    shouldwarn = 0;
    i = 0;
  }
#endif

  x->x_normalize = !!i;
}

void windowing_free(t_windowing *x) {
  freebytes(x->x_table, x->x_tablesize * sizeof(*x->x_table));
}

t_windowing* windowing_new(t_class *cls) {
  t_windowing *x = (t_windowing *)pd_new(cls);
  x->x_overlap = 1;
  x->x_makeup = 1.;
  x->x_tablesize = 0;
  x->x_table = getbytes(x->x_tablesize * sizeof(*x->x_table));
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
  class_addmethod(cls, (t_method)windowing_normalize, gensym("normalize"), A_FLOAT, 0);
  class_addmethod(cls, nullfn, gensym("signal"), 0);
  class_addmethod(cls, (t_method)windowing_dsp, gensym("dsp"), A_CANT, 0);
  class_addmethod(cls, (t_method)fillfun, gensym("windowfill"), A_CANT, 0);

  return cls;
}
