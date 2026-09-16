/* parzen~ - parzen windowing function for Pure Data
**
** SPDX-FileCopyrightText: © 2026, IOhannes m zmölnig <zmoelnig@iem.at>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

static void fillParzen(UNUSED t_windowing *w, t_sample *vec, size_t N) {
  t_sample l2 = 2./(1.+N);
  t_sample xShift = (t_sample)N / 2;
  size_t i;
  for (i = 0; i < N; i++) {
    t_sample x = fabs(i - xShift) * l2;
    t_sample y = (1-x);
    vec[i] = (x <= 0.5) ? (1 - 6*x*x*y) : (2*y*y*y);
  }
}

WINDOWING_SETUP(parzen, fillParzen);
