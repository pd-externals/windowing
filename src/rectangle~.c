/* rectangle~ - rectangle windowing function for Pure Data
**
** SPDX-FileCopyrightText: © 2026, IOhannes m zmölnig <zmoelnig@iem.at>
** SPDX-License-Identifier: GPL-2.0-or-later
**
*/

#include "windowing.h"

static void fillRectangle(UNUSED t_windowing *w, t_sample *vec, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    vec[i] = 1.;
  }
}

WINDOWING_SETUP(rectangle, fillRectangle);
