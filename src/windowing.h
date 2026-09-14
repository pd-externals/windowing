/* windowing.h - generic framework to implement windowing objects */

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

struct _windowing;
static const char*filename = 0;
typedef void (*t_window_fill)(struct _windowing *x, t_sample *vec, size_t n);

static t_class *windowing_class;
typedef struct _windowing {
  t_object x_obj;
  t_window_fill x_fill;
  size_t x_blocksize;
  t_sample *x_table;
  t_sample x_makeup; /* make up gain */
} t_windowing;

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

static void windowing_free(t_windowing *x) {
  freebytes(x->x_table, x->x_blocksize * sizeof(*x->x_table));
}

static void* windowing_new(t_class *cls) {
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
static void* windowing_do_new(void) {
  return windowing_new(windowing_class);
}

static t_class *windowing_setup(
  t_class *cls, const char *classname,
  t_window_fill fillfun) {
  if(cls && classname) {
    pd_error(0, "%s(): class[%p] and classname[%s] are mutually exclusive!", __FUNCTION__, cls, classname);
    return 0;
  }
  if (!cls && !classname) {
    pd_error(0, "%s(): either class or classname must be provided!", __FUNCTION__);
    return 0;
  }
  if(!cls) {
    cls = class_new(
      gensym(classname),
      (t_newmethod)windowing_do_new, (t_method)windowing_free,
      sizeof(t_windowing),
      0, 0);
  }
  windowing_class = cls;

  class_addmethod(cls, nullfn, gensym("signal"), 0);
  class_addmethod(cls, (t_method)windowing_dsp, gensym("dsp"), A_CANT, 0);
  class_addmethod(cls, (t_method)fillfun, gensym("windowfill"), A_CANT, 0);

  return cls;
}

#define WINDOWING_SETUP(name, fillfun) \
  void name##_tilde_setup(void) {\
    filename = __FILE__;					\
    windowing_setup(0, #name"~", (t_window_fill)fillfun);	\
  }

#endif /* INCLUDE_WINDOWING_H */
