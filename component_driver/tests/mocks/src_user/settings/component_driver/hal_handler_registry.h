/**
 * @file hal_handler_registry.h
 * @brief HAL Handler Registry のモック
 *
 * driver_super.c のコンパイル時は driver_super_mock_prelude.h が
 * HAL_HANDLER_REGISTRY_H_ を事前定義するため、このファイルは空になる。
 *
 * テストコード（.cpp）は mocks/mock_hal_handler_registry.h を直接インクルードする。
 */
#ifndef HAL_HANDLER_REGISTRY_H_
#define HAL_HANDLER_REGISTRY_H_

// 型定義は driver_super_mock_prelude.h で提供される

#endif
