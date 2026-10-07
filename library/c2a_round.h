#ifndef C2A_ROUND_H_
#define C2A_ROUND_H_

/**
 * @file
 * @brief 四捨五入．C89にroundはないので
 */

/**
 * @brief 最も近い整数値を double で返す．中間値はゼロから遠ざかる方向に丸める．
 */
double c2a_round(double input);

#endif
