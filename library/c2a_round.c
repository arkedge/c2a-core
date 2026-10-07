#pragma section REPRO

/**
 * @file
 * @brief C89 向けの丸め関数（C99 の round 相当）．
 */
#include "c2a_round.h"
#include <math.h>

double c2a_round(double input)
{
  double integral;
  double fraction = modf(input, &integral);

  if (fraction >= 0.5) return integral + 1.0;
  if (fraction <= -0.5) return integral - 1.0;
  return integral;
}

#pragma section
