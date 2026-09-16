/* lanczos~ - lanczos windowing function for Pure Data 
**
** SPDX-FileCopyrightText: © 2002, Joseph A. Sarlo <jsarlo@mambo.peabody.jhu.edu>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

static void fillLanczos(UNUSED t_windowing *w, t_sample *vec, size_t n) {
  size_t i;
  t_sample xShift = (t_sample)n / 2;
  t_sample x;
  for (i = 0; i < n; i++) {
    x = (i - xShift) / xShift;
    if (x == 0) {
      vec[i] = 1;
    }
    else {
      vec[i] = (t_sample)(sin(M_PI * x) / (M_PI * x));
    }
  }
}

WINDOWING_SETUP(lanczos, fillLanczos);
