/* cosine~ - cosine windowing function for Pure Data 
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

static void fillCosine(t_windowing *unused, t_sample *vec, size_t n) {
  size_t i;
  t_sample xShift = (t_sample)n / 2;
  t_sample x;
  (void)unused;
  for (i = 0; i < n; i++) {
    x = (i - xShift) / xShift;
    vec[i] = (t_sample)cos(M_PI * x / 2);
  }
}

WINDOWING_SETUP(cosine, fillCosine);
