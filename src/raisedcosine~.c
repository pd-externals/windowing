/* raisedcosine~ - raised cosine windowing function for Pure Data
**
** SPDX-FileCopyrightText: © 2026, IOhannes m zmölnig <zmoelnig@iem.at>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

#define DEFDELTA 0.5

static t_class *raisedcosine_class;

typedef struct _raisedcosine {
  t_windowing x_w;
  t_sample*x_coefficients;
  size_t x_numcoefficients;
} t_raisedcosine;


static void fillRaisedcosine(t_raisedcosine *x, t_sample *vec, size_t N) {
  /* w[n] = sum{k=0, K} (-1)^k * a[k] * cos(2*pi*k*n/N) */
  size_t n, i;
  t_sample fShift = (t_sample)N / 2;

  t_sample *coeff = x->x_coefficients;
  size_t I = x->x_numcoefficients;

  for (n = 0; n < N; n++) {
    t_sample f = (n - fShift) / fShift;
    t_sample sum = 0.;
    for (i=0; i<I; i++) {
      sum += coeff[i] * cos(i * M_PI * f);
    }
    vec[n] = sum;
  }
}

static void raisedcosine_list(t_raisedcosine *x, UNUSED t_symbol *s, int argc, t_atom *argv) {
  t_sample *coeffs;
  int i = 0;
  if(argc <= 0) {
    pd_error(x, "ignoring empty coefficient list");
    return;
  }
  if ((size_t)argc != x->x_numcoefficients) {
    x->x_coefficients = resizebytes(
      x->x_coefficients,
      sizeof(*x->x_coefficients) * x->x_numcoefficients,
      sizeof(*x->x_coefficients) * argc);
    x->x_numcoefficients = argc;
  }
  coeffs = x->x_coefficients;
  for (i=0; i<argc; i++) {
    coeffs[i] = atom_getfloatarg(i, argc, argv);
  }

  windowing_rebuildtable(&x->x_w, x->x_w.x_tablesize, x->x_w.x_overlap);
}

static void* raisedcosine_new(t_symbol *s, int argc, t_atom *argv) {
  t_raisedcosine *x = (t_raisedcosine *)windowing_new(raisedcosine_class);
  inlet_new(&x->x_w.x_obj, &x->x_w.x_obj.ob_pd, gensym("list"), gensym("list"));
  if (argc)
    raisedcosine_list(x, s, argc, argv);
  else {
    /* default to blackman */
    x->x_numcoefficients = 3;
    x->x_coefficients = getbytes(sizeof(*x->x_coefficients) * x->x_numcoefficients);
    x->x_coefficients[0] = 0.42;
    x->x_coefficients[1] = 0.50;
    x->x_coefficients[2] = 0.08;
  }
  return (x);
}

void raisedcosine_tilde_setup(void) {
  raisedcosine_class = class_new(gensym("raisedcosine~"),
			    (t_newmethod)raisedcosine_new,
			    (t_method)windowing_free,
    	                    sizeof(t_raisedcosine),
			    CLASS_MULTICHANNEL,
			    A_GIMME,
			    0);
  windowing_setupclass(raisedcosine_class, (t_window_fill)fillRaisedcosine);
  class_addlist(raisedcosine_class, (t_method)raisedcosine_list);
}
