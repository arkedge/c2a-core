#ifndef C2A_ROUND_H_
#define C2A_ROUND_H_

/**
 * @file
 * @brief C89 向けの丸め関数（C99 の round 相当）．
 */

/**
 * @brief 最も近い整数値を double で返す．中間値はゼロから遠ざかる方向に丸める．
 */
double c2a_round(double input);

#endif
