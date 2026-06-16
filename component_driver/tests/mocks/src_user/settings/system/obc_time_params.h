/**
 * @file
 * @brief テスト用 OBCT パラメタ (実 obc_time.c をリンクするための設定)
 *
 * flight の core デフォルトと同値。OBCT_CYCLES_PER_SEC は formula 由来とし、
 * マジックナンバーを避ける (= 1000 / 1 / 100 = 10 [cycle/s])。
 */
#ifndef OBC_TIME_PARAMS_H_
#define OBC_TIME_PARAMS_H_

#undef OBCT_STEP_IN_MSEC
#undef OBCT_STEPS_PER_CYCLE
#undef OBCT_CYCLES_PER_SEC
#undef OBCT_MAX_CYCLE

#define OBCT_STEP_IN_MSEC (1)
#define OBCT_STEPS_PER_CYCLE (100)
#define OBCT_CYCLES_PER_SEC (1000 / OBCT_STEP_IN_MSEC / OBCT_STEPS_PER_CYCLE)
#define OBCT_MAX_CYCLE (0xfffffff0u)

#endif
