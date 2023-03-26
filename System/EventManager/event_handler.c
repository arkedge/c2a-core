#pragma section REPRO
/**
 * @file
 * @brief イベント発火型処理を行う
 * @note  event_logger の情報を元に指定した BC (対応 BC) を展開する (Event 発火に応じて対応を行う)
 */
#include "event_handler.h"
#include <string.h>
#include <stdlib.h>
#include "../../TlmCmd/common_cmd_packet_util.h"
#include "../../Applications/timeline_command_dispatcher_id_define.h"
#include "../TimeManager/time_manager.h"

#ifdef EL_IS_ENABLE_TLOG

/**
 * @enum   EH_EL_LOCAL_ID
 * @brief  EL_CORE_GROUP_EVENT_HANDLER イベントの local id
 * @note   uint8_t を想定
 */
typedef enum
{
  EH_EL_LOCAL_ID_EL_TOTAL_COUNTER_ERR = 0,  //!< ELとEHのカウンタの不整合エラー (counter_total)
  EH_EL_LOCAL_ID_EL_COUNTER_ERR,            //!< ELとEHのカウンタの不整合エラー (counters)
  EH_EL_LOCAL_ID_TOO_MANY_EVENT,            //!< イベントが発生しすぎて，TLogが失われている
  EH_EL_LOCAL_ID_FAIL_TO_RESPOND,           //!< EH 対応時に BC 展開に失敗
  EH_EL_LOCAL_ID_LOG_TABLE_FULL,            //!< EH_LogTable が満杯になり， wp が 0 に戻った
  EH_EL_LOCAL_ID_SEARCH_ERR,                //!< EH_search_rule_table_index_ の返り値不正
  EH_EL_LOCAL_ID_RECURSION_ERR,             //!< 多段対応時に再帰呼び出し回数が設定値を超えた
  EH_EL_LOCAL_ID_UNKNOWN_ERR                //!< 不明なエラー
} EH_EL_LOCAL_ID;

/**
 * @enum   EH_ACK
 * @brief  EH の内部関数返り値
 * @note   uint8_t を想定
 */
typedef enum
{
  EH_ACK_OK,
  EH_ACK_ERR
} EH_ACK;

/**
 * @enum   EH_RULE_SORTED_INDEX_ACK
 * @brief  EH_RuleSortedIndex を操作するときの返り値
 * @note   uint8_t を想定
 */
typedef enum
{
  EH_RULE_SORTED_INDEX_ACK_OK = 0,           //!< 正常終了
  EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID,  //!< 不正な EH_RULE_ID
  EH_RULE_SORTED_INDEX_ACK_NOT_FOUND,        //!< 見つからず
  EH_RULE_SORTED_INDEX_ACK_FULL,             //!< これ以上登録できない
  EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE,   //!< ルールの上書きになってしまう（すでに同じ ID にルールが登録されているため棄却）
  EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL    //!< 重複上限まで重複してしまっている
} EH_RULE_SORTED_INDEX_ACK;

/**
 * @enum   EH_CKECK_RULE_ACK
 * @brief  EH_check_rule 系関数の返り値
 * @note   uint8_t を想定
 */
typedef enum
{
  EH_CKECK_RULE_ACK_MATCH = 0,          //!< 該当 EH_Rule にマッチした
  EH_CKECK_RULE_ACK_NOT_MATCH           //!< 該当 EH_Rule にマッチしなかった
} EH_CKECK_RULE_ACK;


/**
 * @brief  EH_RuleTable を全消去
 * @param  void
 * @return void
 */
static void EH_clear_rules_(void);

/**
 * @brief  EH 対応ログを全消去
 * @param  void
 * @return void
 */
static void EH_clear_log_(void);

/**
 * @brief  EL 側の EL_Event のカウンタをチェックし，整合性を確認する
 * @param  void
 * @retval EH_ACK_OK  : 正常終了
 * @retval EH_ACK_ERR : EL と EH のカウンタの不整合が発生し， EH カウンタリセット
 */
static EH_ACK EH_check_el_event_counter_(void);

/**
 * @brief  EH_Rule が存在するかチェックするための次の EL_Event (EL_ERROR_LEVEL_EH を除く) を返す
 * @note   見つからなかった場合， NULL を返す
 * @param  void
 * @return 取得した EL_Event
 */
static const EL_Event* EH_get_event_to_check_rule_(void);

/**
 * @brief  EL_Event に対応する EH_Rule が存在するかチェックし，対応する
 * @note   多段の EH 対応のために，再帰呼出しの起点となる
 * @param[in] event: チェックする EL_Event
 * @return 対応した EH_Rule 数（多段対応も考慮）
 */
static uint8_t EH_check_event_and_respond_(const EL_Event* event);

/**
 * @brief  EH の対応条件をチェックし， EL_Event に対応する
 *
 *         対応条件にマッチした場合， EL_CORE_GROUP_EH_MATCH_RULE イベントを発行し，多段の EH 対応を問い合わせる
 * @note   引数はアサーション済みを仮定する
 * @note   再帰呼出しされる
 * @param[in] rule_id: 対応条件をチェックし，対応する EH_RULE_ID
 * @param[in] event:   発生した EL_Event
 * @return 対応した EH_Rule 数（多段対応も考慮）
 */
static uint8_t EH_check_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  上位の EH の対応条件をチェックし，対応する
 * @note   引数はアサーション済みを仮定する
 * @note   再帰呼出しされる
 * @param[in] rule_id: 下位となる可能性がある EH_RULE_ID
 * @return 対応した EH_Rule 数（多段対応も考慮）
 */
static uint8_t EH_check_higher_level_rule_and_respond_(EH_RULE_ID rule_id);

/**
 * @brief  EH の対応条件をチェックする
 * @note   引数はアサーション済みを仮定する
 * @param[in] rule_id: 対応条件をチェックする EH_RULE_ID
 * @param[in] event:   発生した EL_Event
 * @return EH_CKECK_RULE_ACK
 */
static EH_CKECK_RULE_ACK EH_check_rule_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH の対応条件をチェックする (EH_RESPONSE_CONDITION_SINGLE)
 * @note   引数はアサーション済みを仮定する
 * @note   上位の EH_Rule についてはここではみない
 * @param[in] rule_id: 対応条件をチェックする EH_RULE_ID
 * @param[in] event:   発生した EL_Event
 * @return EH_CKECK_RULE_ACK
 */
static EH_CKECK_RULE_ACK EH_check_single_rule_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH の対応条件をチェックする (EH_RESPONSE_CONDITION_CONTINUOUS)
 * @note   引数はアサーション済みを仮定する
 * @note   上位の EH_Rule についてはここではみない
 * @param[in] rule_id: 対応条件をチェックする EH_RULE_ID
 * @param[in] event:   発生した EL_Event
 * @return EH_CKECK_RULE_ACK
 */
static EH_CKECK_RULE_ACK EH_check_continuous_rule_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH の対応条件をチェックする (EH_RESPONSE_CONDITION_CUMULATIVE)
 * @note   引数はアサーション済みを仮定する
 * @note   上位の EH_Rule についてはここではみない
 * @param[in] rule_id: 対応条件をチェックする EH_RULE_ID
 * @param[in] event:   発生した EL_Event
 * @return EH_CKECK_RULE_ACK
 */
static EH_CKECK_RULE_ACK EH_check_cumulative_rule_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH 対応を実施
 * @note   引数はアサーション済みを仮定する
 * @note   is_active はこの関数ではみない（上流でチェックしてることを想定）
 * @param[in] rule_id: 対応する EH_RULE_ID
 * @return void
 */
static void EH_respond_(EH_RULE_ID rule_id);

/**
 * @brief  EH 対応のログを残す
 * @note   引数はアサーション済みを仮定する
 * @param[in] rule_id:        対応した EH_RULE_ID
 * @param[in] deploy_cmd_ack: 対応 BC 展開コマンドの実行結果
 * @return void
 */
static void EH_record_responded_log_(EH_RULE_ID rule_id, CCP_EXEC_STS deploy_cmd_ack);

/**
 * @brief  まだ処理していない最も古い EL_Event (EL_ERROR_LEVEL_EH を除く) を返す
 * @note   見つからなかった場合， NULL を返す
 * @param  void
 * @return 見つけた EL_Event
 */
static const EL_Event* EH_get_oldest_event_excluding_eh_error_level_(void);

/**
 * @brief  EH_RuleSortedIndex から，目的の EL_Event の idx を検索する
 * @note   引数はアサーション済みを仮定する
 * @param[in]  group: 検索する EL_Event.group
 * @param[in]  local: 検索する EL_Event.local
 * @param[out] found_ids: 見つかった EH_RULE_ID 一覧
 * @param[out] found_sorted_idxes: 見つかった EH_RuleSortedIndex の index
 * @param[out] found_id_num: 見つかった idx の数
 * @retval EH_RULE_SORTED_INDEX_ACK_NOT_FOUND: 見つからず
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: 正常に探索完了
 */
static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_(EL_GROUP group,
                                                            uint32_t local,
                                                            EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint8_t* found_id_num);

/**
 * @brief  EH_search_rule_table_index_ での bsearch 用の EH_RuleSortedIndex 比較関数
 * @note   duplicate_id が 0 であるものを見つける想定
 * @param[in]  key:  bsearch で検索する EH_RuleSortedIndex
 * @param[in]  elem: bsearch 検索対象の EH_RuleSortedIndex 配列要素
 * @retval 1:  key > elem
 * @retval 0:  key == elem
 * @retval -1: key < elem
 */
static int EH_compare_sorted_index_for_bsearch_(const void* key, const void* elem);

/**
 * @brief  EH_RuleSortedIndex から，指定した EL_GROUP の EH_Rule を指す最も若い EH_RuleSortedIndex を取得する
 * @note   引数はアサーション済みを仮定する
 * @note   by_event_group 関数用
 * @param[in]  group: 検索する EL_Event.group
 * @param[out] least_found_sorted_idx: 見つかった EH_RuleSortedIndex の index のうち最小のもの
 * @param[out] found_sorted_idx_num: 見つかった EH_RuleSortedIndex の index の数
 * @retval EH_RULE_SORTED_INDEX_ACK_NOT_FOUND: 見つからず
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: 正常に探索完了
 */
static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_by_event_group_(EL_GROUP group,
                                                                           uint16_t* least_found_sorted_idx,
                                                                           uint16_t* found_sorted_idx_num);

/**
 * @brief  EH_search_rule_table_index_by_event_group_ での bsearch 用の EH_RuleSortedIndex 比較関数
 * @note   group 指定では最も若いものを見つけることができないので，これを用いて bsearch した場合，同じ group が複数ある時，どれを返すかは未規定となることに注意
 * @note   by_event_group 関数用
 * @param[in]  key:  bsearch で検索する EH_RuleSortedIndex
 * @param[in]  elem: bsearch 検索対象の EH_RuleSortedIndex 配列要素
 * @retval 1:  key > elem
 * @retval 0:  key == elem
 * @retval -1: key < elem
 */
static int EH_compare_sorted_index_by_event_group_for_bsearch_(const void* key, const void* elem);

/**
 * @brief  EH_Rule を EH_RuleTable と EH_RuleSortedIndex に挿入する
 * @note   引数は rule に関してはアサーション済みを仮定する
 * @note   すでに登録された id に対しての上書き登録はエラー (EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE) を返す
 * @param[in] id: 挿入する EH_Rule の EH_RULE_ID
 * @param[in] rule: 挿入する EH_Rule
 * @retval EH_RULE_SORTED_INDEX_ACK_FULL: ルール登録上限に到達済み
 * @retval EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID: 不正な EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE: すでに同じ ID にルールが登録されているため棄却
 * @retval EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL: 重複上限まで重複してしまっている
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: 正常に挿入完了
 */
static EH_RULE_SORTED_INDEX_ACK EH_insert_rule_table_(EH_RULE_ID id, const EH_Rule* rule);

/**
 * @brief  EH_Rule を EH_RuleTable と EH_RuleSortedIndex から削除する
 * @param[in] id: 削除する EH_Rule の EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID: 不正な EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_NOT_FOUND: 削除対象が見つからず or 登録されてない
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: 正常に削除完了
 */
static EH_RULE_SORTED_INDEX_ACK EH_delete_rule_table_(EH_RULE_ID id);

/**
 * @brief  EH_RULE_ID のルール登録状況を調べる
 * @param[in] id: 調べる EH_RULE_ID
 * @return EH_CHECK_RULE_ACK
 */
static EH_CHECK_RULE_ACK EH_check_rule_id_(EH_RULE_ID id);

/**
 * @brief  by_event_group 関数を実行する主体
 * @param[in] group: 操作する EL_GROUP
 * @param[in] func:  実行する関数
 * @return void
 */
static void EH_exec_func_by_event_group_(EL_GROUP group,
                                         EH_CHECK_RULE_ACK (*func)(EH_RULE_ID));


static EventHandler event_handler_;
const EventHandler* const event_handler = &event_handler_;


void EH_initialize(void)
{
  memset(&event_handler_, 0x00, sizeof(EventHandler));

  EH_clear_rules_();
  EH_clear_log_();

  EH_match_event_counter_to_el();
  event_handler_.exec_settings.max_response_num = EH_MAX_RESPONSE_NUM_DEFAULT;
  event_handler_.exec_settings.max_check_event_num = EH_MAX_CHECK_EVENT_NUM_DEFAULT;
  event_handler_.exec_settings.max_multi_level_num = EH_MAX_MULTI_LEVEL_NUM_DEFAULT;
  event_handler_.exec_info.current_multi_level = 0;

  EH_load_default_rules();
}


static void EH_clear_rules_(void)
{
  int i;
  memset(&event_handler_.rule_table, 0x00, sizeof(EH_RuleTable));
  // 現時点で EL_CORE_GROUP_NULL == 0 であるため，以下は不要．
  // for (i = 0; i < (int)EH_RULE_MAX; ++i)
  // {
  //   event_handler_.rule_table.rules[i].settings.event.group = (EL_GROUP)EL_CORE_GROUP_NULL;
  // }

  // EH_RuleSortedIndex もクリア
  memset(event_handler_.sorted_idxes, 0x00, sizeof(EH_RuleSortedIndex) * EH_RULE_MAX);
  for (i = 0; i < (int)EH_RULE_MAX; ++i)
  {
    // 現時点で EL_CORE_GROUP_NULL == 0 であるため，以下は不要．
    // event_handler_.sorted_idxes[i].group = (EL_GROUP)EL_CORE_GROUP_NULL;
    event_handler_.sorted_idxes[i].rule_id = EH_RULE_MAX;
  }
}


static void EH_clear_log_(void)
{
  int i;
  memset(&event_handler_.log_table, 0x00, sizeof(EH_LogTable));

  for (i = 0; i < EH_LOG_MAX; ++i)
  {
    event_handler_.log_table.logs[i].rule_id = EH_RULE_MAX;
  }
}


void EH_execute(void)
{
  uint8_t responded_count = 0;
  EH_ACK ack;
  uint16_t i;

  ack = EH_check_el_event_counter_();
  if (ack != EH_ACK_OK) return;

  for (i = 0; i < event_handler_.exec_settings.max_check_event_num; ++i)
  {
    const EL_Event* event = EH_get_event_to_check_rule_();
    if (event == NULL)
    {
      // もうチェックすべきイベントはなし
      break;
    }

    event_handler_.exec_info.current_multi_level = 0;
    responded_count += EH_check_event_and_respond_(event);
    if (responded_count >= event_handler_.exec_settings.max_response_num) break;
  }
}


static EH_ACK EH_check_el_event_counter_(void)
{
  int32_t delta_counter = event_logger->statistics.record_counter_total - event_handler_.el_event_counter.counter_total;
  int32_t delta_counters[EL_ERROR_LEVEL_MAX];
  uint8_t err_level;
  int32_t subtotal = 0;

  // カウンタ不整合を調べる．
  // エラー等は定期的にリセットされているはずなので，オーバーフローは考慮していない
  if (delta_counter < 0)
  {
    // 不整合
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_EL_TOTAL_COUNTER_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)(-delta_counter));
    // 仕方がないので強制的に合わせる
    EH_match_event_counter_to_el();
    return EH_ACK_ERR;
  }

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    delta_counters[err_level] = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    if (delta_counters[err_level] < 0)
    {
      // 不整合
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_EL_COUNTER_ERR,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)(err_level));
      // 仕方がないので強制的に合わせる
      EH_match_event_counter_to_el();
      return EH_ACK_ERR;
    }

    subtotal += delta_counters[err_level];
  }

  if (delta_counter != (int32_t)subtotal)
  {
    // 不整合
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_EL_COUNTER_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    EL_ERROR_LEVEL_MAX);    // ここの note は少し考える？
    // 仕方がないので強制的に合わせる
    EH_match_event_counter_to_el();
    return EH_ACK_ERR;
  }

  return EH_ACK_OK;
}


static const EL_Event* EH_get_event_to_check_rule_(void)
{
  const EL_Event* event = EH_get_oldest_event_excluding_eh_error_level_();
  if (event == NULL) return NULL;

  // 処理する EL_Event が見つかったので，カウントアップ
  event_handler_.el_event_counter.counter_total++;
  event_handler_.el_event_counter.counters[event->err_level]++;

  return event;
}

static uint8_t EH_check_event_and_respond_(const EL_Event* event)
{
  EH_RULE_SORTED_INDEX_ACK search_ack;
  EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint8_t found_id_num;
  uint8_t i;
  uint8_t responded_num = 0;

  search_ack = EH_search_rule_table_index_(event->group,
                                           event->local,
                                           found_ids,
                                           found_sorted_idxes,
                                           &found_id_num);

  if (search_ack == EH_RULE_SORTED_INDEX_ACK_NOT_FOUND)
  {
    // 対応する EH_Rule なし
    return 0;
  }
  if (search_ack != EH_RULE_SORTED_INDEX_ACK_OK)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_SEARCH_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)search_ack);
    return 0;
  }

  // ルールチェック & 対応
  for (i = 0; i < found_id_num; ++i)
  {
    responded_num += EH_check_rule_and_respond_(found_ids[i], event);
  }

  return responded_num;
}


static uint8_t EH_check_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_RuleSettings* rule_settings = &event_handler_.rule_table.rules[rule_id].settings;
  uint8_t responded_num = 0;

  if (rule_settings->event.group != event->group || rule_settings->event.local != event->local)
  {
    // 何かがおかしい（ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)rule_id);
    return 0;
  }

  if (EH_check_rule_(rule_id, event) != EH_CKECK_RULE_ACK_MATCH)
  {
    return 0;
  }

  // ここまで来たら引数に取った EH_RULE_ID は EH_Rule にマッチした
  EL_record_event((EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE,
                  (uint32_t)rule_id,
                  EL_ERROR_LEVEL_EH,
                  (uint32_t)event->err_level);

  // 上位の EH_Rule にマッチしないか問い合わせ
  responded_num = EH_check_higher_level_rule_and_respond_(rule_id);

  if (responded_num > 0)
  {
    // 上位で対応された
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EH_RESPOND_WITH_HIGHER_LEVEL_RULE,
                    (uint32_t)rule_id,
                    EL_ERROR_LEVEL_EH,
                    (uint32_t)responded_num);
    return responded_num;
  }

  // このルールで対応する
  EH_respond_(rule_id);
  return 1;
}


static uint8_t EH_check_higher_level_rule_and_respond_(EH_RULE_ID rule_id)
{
  const EL_Event* higher_level_trigger_event = EL_get_the_nth_tlog_from_the_latest(EL_ERROR_LEVEL_EH, 0);
  int32_t delta_counter = event_logger->statistics.record_counters[EL_ERROR_LEVEL_EH] -
                          event_handler_.el_event_counter.counters[EL_ERROR_LEVEL_EH];

  event_handler_.exec_info.current_multi_level++;

  if (event_handler_.exec_info.current_multi_level >= event_handler_.exec_settings.max_multi_level_num)
  {
    // これ以上の再帰回数は認められない
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_RECURSION_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    0);
    return 0;
  }

  if (delta_counter < 1)
  {
    // 何かがおかしい（ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    0);
    return 0;
  }
  if (higher_level_trigger_event->group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE || higher_level_trigger_event->local != rule_id)
  {
    // 何かがおかしい（ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    0);
    return 0;
  }

  // EL_Event を処理したのでカウントアップ
  event_handler_.el_event_counter.counter_total += delta_counter;
  event_handler_.el_event_counter.counters[EL_ERROR_LEVEL_EH] += delta_counter;

  // 上位の EH_Rule があるか検索して対応（再帰）
  return EH_check_event_and_respond_(higher_level_trigger_event);
}


static EH_CKECK_RULE_ACK EH_check_rule_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_RuleSettings* rule_settings = &event_handler_.rule_table.rules[rule_id].settings;

  if (!(rule_settings->is_active))
  {
    return EH_CKECK_RULE_ACK_NOT_MATCH;
  }
  if (rule_settings->should_match_err_level)
  {
    if (rule_settings->event.err_level != event->err_level)
    {
      return EH_CKECK_RULE_ACK_NOT_MATCH;
    }
  }

  switch (rule_settings->condition.type)
  {
  case EH_RESPONSE_CONDITION_SINGLE:
    return EH_check_single_rule_(rule_id, event);
  case EH_RESPONSE_CONDITION_CONTINUOUS:
    return EH_check_continuous_rule_(rule_id, event);
  case EH_RESPONSE_CONDITION_CUMULATIVE:
    return EH_check_cumulative_rule_(rule_id, event);
  default:
    // 何かがおかしい（ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)rule_id);
    return EH_CKECK_RULE_ACK_NOT_MATCH;
  }
}


static EH_CKECK_RULE_ACK EH_check_single_rule_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  rule->last_event_time = event->time;
  return EH_CKECK_RULE_ACK_MATCH;
}


static EH_CKECK_RULE_ACK EH_check_continuous_rule_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  uint32_t delta_time_ms = OBCT_diff_in_msec(&rule->last_event_time, &event->time);
  rule->last_event_time = event->time;

  if (delta_time_ms > rule->settings.condition.time_threshold_ms)
  {
    // 連続ではなくなった
    EH_clear_rule_counter(rule_id);
  }

  rule->counter++;

  if (rule->counter < rule->settings.condition.count_threshold)
  {
    return EH_CKECK_RULE_ACK_NOT_MATCH;
  }

  EH_clear_rule_counter(rule_id);
  return EH_CKECK_RULE_ACK_MATCH;
}


static EH_CKECK_RULE_ACK EH_check_cumulative_rule_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  rule->last_event_time = event->time;
  rule->counter++;

  if (rule->counter < rule->settings.condition.count_threshold)
  {
    return EH_CKECK_RULE_ACK_NOT_MATCH;
  }

  EH_clear_rule_counter(rule_id);
  return EH_CKECK_RULE_ACK_MATCH;
}


static void EH_respond_(EH_RULE_ID rule_id)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  CCP_CmdRet cmd_ret;

  cmd_ret = CCP_form_and_exec_block_deploy_cmd(TLCD_ID_DEPLOY_BC, rule->settings.deploy_bct_id);
  if (cmd_ret.exec_sts != CCP_EXEC_SUCCESS)
  {
    uint32_t note = ((0x0000ffff & cmd_ret.exec_sts) << 16) | (0x0000ffff & cmd_ret.err_code);
    if ((PL_ACK)cmd_ret.err_code == PL_BC_TIME_ADJUSTED)
    {
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_FAIL_TO_RESPOND,
                      EL_ERROR_LEVEL_LOW,
                      note);
    }
    else
    {
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_FAIL_TO_RESPOND,
                      EL_ERROR_LEVEL_HIGH,
                      note);
    }
  }

  EH_inactivate_rule_for_multi_level(rule_id);

  EH_record_responded_log_(rule_id, cmd_ret.exec_sts);
}


// FIXME: CCP_EXEC_STS -> CCP_CmdRet にしてもいいかも？
static void EH_record_responded_log_(EH_RULE_ID rule_id, CCP_EXEC_STS deploy_cmd_ack)
{
  EH_LogTable* log_table = &event_handler_.log_table;

  log_table->respond_counter++;

  log_table->logs[log_table->log_wp].rule_id = rule_id;
  log_table->logs[log_table->log_wp].respond_time_in_master_cycle = TMGR_get_master_total_cycle();
  log_table->logs[log_table->log_wp].deploy_cmd_ack = deploy_cmd_ack;

  log_table->log_wp++;
  if (log_table->log_wp >= EH_LOG_MAX)
  {
    log_table->log_wp = 0;
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_LOG_TABLE_FULL,
                    EL_ERROR_LEVEL_HIGH,
                    0);
  }
}


static const EL_Event* EH_get_oldest_event_excluding_eh_error_level_(void)
{
  uint8_t err_level;      // for で回すので u8 で
  ObcTime oldest_time = OBCT_get_max();
  const EL_Event* oldest_event = NULL;

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    // EH 実行中にも EL のイベント登録は発生するため，ここで最新の情報を元に処理する
    int32_t delta_counter = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    const uint16_t tlog_capacity = event_logger->tlogs[err_level].log_capacity;
    const EL_Event* event;

    if (err_level == EL_ERROR_LEVEL_EH) continue;
    if (delta_counter <= 0) continue;

    // キャパを溢れていたら，諦めて情報を修正する
    if (delta_counter > tlog_capacity)
    {
      const uint16_t keep_tlogs_num = (uint16_t)(tlog_capacity / 2);
      uint32_t drop_tlog_num;

      // イベントが発生しすぎて，TLogが失われている
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_TOO_MANY_EVENT,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)(err_level));

      // 溢れかえっているということなので，またすぐ溢れることを考慮し
      // tlog_capacity の半分ぐらいまでのこし，後は諦める
      drop_tlog_num = (uint32_t)(delta_counter - keep_tlogs_num);

      event_handler_.el_event_counter.counter_total += drop_tlog_num;
      event_handler_.el_event_counter.counters[err_level] += drop_tlog_num;

      // カウンタを変えたので，更新
      delta_counter = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    }

    event = EL_get_the_nth_tlog_from_the_latest((EL_ERROR_LEVEL)err_level, (uint16_t)(delta_counter - 1));
    if (OBCT_compare(&event->time, &oldest_time) == 1)
    {
      oldest_event = event;
      oldest_time = event->time;
    }
  }

  return oldest_event;
}


static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_(EL_GROUP group,
                                                            uint32_t local,
                                                            EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint8_t* found_id_num)
{
  // idx: 0 ~ rule_table.registered_rule_num の間で二分探索する
  // 重複もあり得ることを考慮する (duplicate_id は異なる)

  uint16_t found_idx = EH_RULE_MAX;
  const EH_RuleSortedIndex* p_searched_sorted_idx = NULL;
  EH_RuleSortedIndex target_sorted_idx;
  uint16_t i;
  uint16_t possible_num_of_id_duplicates;

  if (event_handler_.rule_table.registered_rule_num == 0)
  {
    return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  }

  memset(&target_sorted_idx, 0x00, sizeof(EH_RuleSortedIndex));
  target_sorted_idx.group = group;
  target_sorted_idx.local = local;
  p_searched_sorted_idx = (EH_RuleSortedIndex*)bsearch(&target_sorted_idx,
                                                       event_handler_.sorted_idxes,
                                                       event_handler_.rule_table.registered_rule_num,
                                                       sizeof(EH_RuleSortedIndex),
                                                       EH_compare_sorted_index_for_bsearch_);
  if (p_searched_sorted_idx == NULL) return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  found_idx = (uint16_t)(p_searched_sorted_idx - (&event_handler_.sorted_idxes[0]));

  // 見つかった．後は，いくつあるか？
  *found_id_num = 0;
  possible_num_of_id_duplicates = EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES;
  if (possible_num_of_id_duplicates > event_handler_.rule_table.registered_rule_num - found_idx)
  {
    possible_num_of_id_duplicates = event_handler_.rule_table.registered_rule_num - found_idx;
  }
  for (i = 0; i < possible_num_of_id_duplicates; ++i)
  {
    EH_RuleSortedIndex* p_idx = &event_handler_.sorted_idxes[found_idx + i];
    if ( !(p_idx->group == group && p_idx->local == local) )
    {
      break;
    }
    found_sorted_idxes[i] = (uint16_t)(found_idx + i);
    found_ids[i] = p_idx->rule_id;
    (*found_id_num)++;
  }

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


static int EH_compare_sorted_index_for_bsearch_(const void* key, const void* elem)
{
  const EH_RuleSortedIndex* p_key  = (const EH_RuleSortedIndex*)key;
  const EH_RuleSortedIndex* p_elem = (const EH_RuleSortedIndex*)elem;

  if (p_elem->group == p_key->group)
  {
    if (p_elem->local == p_key->local)
    {
      if (p_elem->duplicate_id == 0)
      {
        // 探してたもの
        return 0;
      }
      else
      {
        // もっと手前にあるはず
        return -1;
      }
    }
    else if (p_elem->local < p_key->local)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (p_elem->group < p_key->group)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}


static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_by_event_group_(EL_GROUP group,
                                                                           uint16_t* least_found_sorted_idx,
                                                                           uint16_t* found_sorted_idx_num)
{
  // idx: 0 ~ rule_table.registered_rule_num の間で二分探索する
  // 重複もあり得ることを考慮する (duplicate_id は異なる)

  uint16_t found_idx = EH_RULE_MAX;
  uint16_t last_found_sorted_idx = 0;
  const EH_RuleSortedIndex* p_searched_sorted_idx = NULL;
  EH_RuleSortedIndex target_sorted_idx;
  uint16_t i;

  if (event_handler_.rule_table.registered_rule_num == 0)
  {
    return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  }

  memset(&target_sorted_idx, 0x00, sizeof(EH_RuleSortedIndex));
  target_sorted_idx.group = group;
  p_searched_sorted_idx = (EH_RuleSortedIndex*)bsearch(&target_sorted_idx,
                                                       event_handler_.sorted_idxes,
                                                       event_handler_.rule_table.registered_rule_num,
                                                       sizeof(EH_RuleSortedIndex),
                                                       EH_compare_sorted_index_by_event_group_for_bsearch_);
  if (p_searched_sorted_idx == NULL) return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  found_idx = (uint16_t)(p_searched_sorted_idx - (&event_handler_.sorted_idxes[0]));
  *least_found_sorted_idx = found_idx;

  // ひとまず見つかったので，最も若いものを探す
  for (i = found_idx; i >= 0; --i)
  {
    if (event_handler_.sorted_idxes[i].group == group)
    {
      *least_found_sorted_idx = i;
    }
    else
    {
      break;
    }
  }

  // 最も後ろのものを探す
  for (i = found_idx; i < event_handler_.rule_table.registered_rule_num; ++i)
  {
    if (event_handler_.sorted_idxes[i].group == group)
    {
      last_found_sorted_idx = i;
    }
    else
    {
      break;
    }
  }

  *found_sorted_idx_num = (uint16_t)(last_found_sorted_idx - *least_found_sorted_idx + 1);
  return EH_RULE_SORTED_INDEX_ACK_OK;
}


static int EH_compare_sorted_index_by_event_group_for_bsearch_(const void* key, const void* elem)
{
  const EH_RuleSortedIndex* p_key  = (const EH_RuleSortedIndex*)key;
  const EH_RuleSortedIndex* p_elem = (const EH_RuleSortedIndex*)elem;

  if (p_elem->group == p_key->group)
  {
    return 0;       // 注意: 同じ group のものが複数あった時，どれを bsearch が返すかは未規定
  }
  else if (p_elem->group < p_key->group)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}


static EH_RULE_SORTED_INDEX_ACK EH_insert_rule_table_(EH_RULE_ID id, const EH_Rule* rule)
{
  // insert は，めんどくさい & 頻繁には起きないので，二分探索せずにやっていく
  EH_RuleTable* p_rule_table = &event_handler_.rule_table;
  EH_RuleSortedIndex* p_sorted_idxes = event_handler_.sorted_idxes;
  const EL_GROUP insert_group = rule->settings.event.group;
  const uint32_t insert_local = rule->settings.event.local;
  uint16_t i;
  uint8_t j;
  uint16_t insert_idx;
  uint8_t duplicate_id;

  EH_CHECK_RULE_ACK check_ack = EH_check_rule_id_(id);
  if (check_ack == EH_CHECK_RULE_ACK_INVALID_RULE_ID) return EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID;
  if (check_ack == EH_CHECK_RULE_ACK_OK) return EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE;

  if (p_rule_table->registered_rule_num >= EH_RULE_MAX)
  {
    return EH_RULE_SORTED_INDEX_ACK_FULL;
  }

  insert_idx = p_rule_table->registered_rule_num;
  duplicate_id = 0;
  for (i = 0; i < p_rule_table->registered_rule_num; ++i)
  {
    EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[i];
    uint8_t break_flag = 0;

    if (p_sorted_idx->group < insert_group) continue;

    if (p_sorted_idx->group > insert_group)
    {
      // 挿入する場所はここ
      insert_idx = i;
      break;
    }

    // ここまできたら p_sorted_idx->group == insert_group
    if (p_sorted_idx->local < insert_local) continue;

    if (p_sorted_idx->local > insert_local)
    {
      // 挿入する場所はここ
      insert_idx = i;
      break;
    }

    // ここまできたら
    // - p_sorted_idx->group == insert_group
    // - p_sorted_idx->local == insert_local

    // 重複してる or する
    for (j = 1; j < EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES; ++j)
    {
      uint16_t idx = (uint16_t)(i + j);
      EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[idx];

      if (idx >= p_rule_table->registered_rule_num)
      {
        // 末端まできたので，ここが挿入する場所
        insert_idx = idx;
        duplicate_id = j;
        break_flag = 1;
        break;
      }

      if (p_sorted_idx->group == insert_group && p_sorted_idx->local == insert_local)
      {
        // まだ重複している
        continue;
      }

      // 重複しなくなったので，ここが挿入する場所
      insert_idx = idx;
      duplicate_id = j;
      break_flag = 1;
      break;
    }
    if (break_flag) break;

    // 重複上限まで重複してしまっている
    return EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL;
  }

  // ここまで来たら，挿入できる
  // insert_idx, duplicate_id が出揃っているはず
  memmove(&p_sorted_idxes[insert_idx + 1],
          &p_sorted_idxes[insert_idx],
          sizeof(EH_RuleSortedIndex) * (EH_RULE_MAX - insert_idx - 1));
  p_sorted_idxes[insert_idx].group        = insert_group;
  p_sorted_idxes[insert_idx].local        = insert_local;
  p_sorted_idxes[insert_idx].duplicate_id = duplicate_id;
  p_sorted_idxes[insert_idx].rule_id      = id;

  p_rule_table->rules[id] = *rule;
  p_rule_table->registered_rule_num++;

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


static EH_RULE_SORTED_INDEX_ACK EH_delete_rule_table_(EH_RULE_ID id)
{
  // delete は，めんどくさい & 頻繁には起きないので，二分探索せずにやっていく
  EH_RuleTable* p_rule_table = &event_handler_.rule_table;
  EH_RuleSortedIndex* p_sorted_idxes = event_handler_.sorted_idxes;
  EL_GROUP delete_group;
  uint32_t delete_local;
  uint16_t i;
  uint8_t j;
  uint16_t delete_idx;

  EH_CHECK_RULE_ACK check_ack = EH_check_rule_id_(id);
  if (check_ack == EH_CHECK_RULE_ACK_INVALID_RULE_ID) return EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID;
  if (check_ack == EH_CHECK_RULE_ACK_UNREGISTERED) return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;

  // この２個，const にしたい．．．
  delete_group = p_rule_table->rules[id].settings.event.group;
  delete_local = p_rule_table->rules[id].settings.event.local;

  delete_idx = EH_RULE_MAX;
  for (i = 0; i < p_rule_table->registered_rule_num; ++i)
  {
    if (id == p_sorted_idxes[i].rule_id)
    {
      // 見つかった
      delete_idx = i;
      break;
    }
  }

  if (delete_idx == EH_RULE_MAX)
  {
    // 本当はこれはありえないはず
    // （ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    0xffffffff);
    return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  }

  // ここまで来たら削除できる
  memmove(&p_sorted_idxes[delete_idx],
          &p_sorted_idxes[delete_idx + 1],
          sizeof(EH_RuleSortedIndex) * (EH_RULE_MAX - delete_idx - 1));
  memset(&p_sorted_idxes[EH_RULE_MAX - 1], 0x00, sizeof(EH_RuleSortedIndex));
  p_sorted_idxes[EH_RULE_MAX - 1].group = (EL_GROUP)EL_CORE_GROUP_NULL;
  p_sorted_idxes[EH_RULE_MAX - 1].rule_id = EH_RULE_MAX;

  // 重複IDの処理
  for (j = 0; j < (EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES - 1); ++j)
  {
    uint16_t idx = (uint16_t)(delete_idx + j);
    EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[idx];

    if (p_sorted_idx->group == delete_group && p_sorted_idx->local == delete_local)
    {
      p_sorted_idx->duplicate_id--;
    }
    else
    {
      break;
    }
  }

  memset(&p_rule_table->rules[id], 0x00, sizeof(EH_Rule));
  p_rule_table->rules[id].settings.event.group = (EL_GROUP)EL_CORE_GROUP_NULL;
  p_rule_table->registered_rule_num--;

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


EH_REGISTER_ACK EH_register_rule(EH_RULE_ID id, const EH_RuleSettings* settings)
{
  EH_RULE_SORTED_INDEX_ACK ack;
  EH_Rule rule;

  if (settings->event.group >= EL_GROUP_MAX)                 return EH_REGISTER_ACK_ILLEGAL_GROUP;
  if (settings->event.group <= (EL_GROUP)EL_CORE_GROUP_NULL) return EH_REGISTER_ACK_ILLEGAL_GROUP;
  if (settings->event.err_level < 0)                         return EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL;
  if (settings->event.err_level >= EL_ERROR_LEVEL_MAX)       return EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL;
  if (settings->should_match_err_level != 0 && settings->should_match_err_level != 1)
  {
    return EH_REGISTER_ACK_ILLEGAL_MATCH_FLAG;
  }
  if (settings->condition.type >= EH_RESPONSE_CONDITION_TYPE_MAX) return EH_REGISTER_ACK_ILLEGAL_CONDITION_TYPE;
  if (settings->condition.type != EH_RESPONSE_CONDITION_SINGLE)
  {
    if (settings->condition.count_threshold == 0) return EH_REGISTER_ACK_ILLEGAL_COUNT_THRESHOLD;
  }
  if (settings->deploy_bct_id >= BCT_MAX_BLOCKS) return EH_REGISTER_ACK_ILLEGAL_BCT_ID;
  if (settings->is_active != 0 && settings->is_active != 1)
  {
    return EH_REGISTER_ACK_ILLEGAL_ACTIVE_FLAG;
  }
  if (settings->event.err_level == EL_ERROR_LEVEL_EH)
  {
    // 多段対応以外で EL_ERROR_LEVEL_EH の設定は不可
    if (settings->event.group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;
  }
  if (settings->event.group == (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE)
  {
    // 多段の時は EL_ERROR_LEVEL_EH が必須
    if (settings->event.err_level != EL_ERROR_LEVEL_EH) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;
    if (settings->should_match_err_level != 1) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;
  }
  if (settings->event.group == (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE)
  {
    // 多段設定の最大値を超えてないかチェック
    uint8_t multi_level_num;
    EH_RULE_ID lower_level_rule = (EH_RULE_ID)(settings->event.local);

    // 1段以下なら，そもそも多段が組めない
    if (event_handler_.exec_settings.max_multi_level_num <= 1) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;

    // 無限ループ回避のために EH_RULE_MAX で抑える
    for (multi_level_num = 0; multi_level_num < EH_RULE_MAX; ++multi_level_num)
    {
      EH_RuleSettings* rule_settings;
      EH_CHECK_RULE_ACK ack;

      ack = EH_check_rule_id_(lower_level_rule);
      if (ack == EH_CHECK_RULE_ACK_INVALID_RULE_ID) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;
      if (ack == EH_CHECK_RULE_ACK_UNREGISTERED) break;

      rule_settings = &event_handler_.rule_table.rules[lower_level_rule].settings;

      if (rule_settings->event.group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE) break;

      // 更に多段になっている
      lower_level_rule = (EH_RULE_ID)rule_settings->event.local;

      if (multi_level_num >= (event_handler_.exec_settings.max_multi_level_num - 2)) return EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL;
    }
  }

  rule.settings = *settings;
  rule.counter = 0;
  OBCT_clear(&rule.last_event_time);

  ack = EH_insert_rule_table_(id, &rule);

  switch (ack)
  {
  case EH_RULE_SORTED_INDEX_ACK_OK:
    return EH_REGISTER_ACK_OK;
  case EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID:
    return EH_REGISTER_ACK_ILLEGAL_RULE_ID;
  case EH_RULE_SORTED_INDEX_ACK_FULL:
    return EH_REGISTER_ACK_ERR_FULL;
  case EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE:
    return EH_REGISTER_ACK_ERR_RULE_OVERWRITE;
  case EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL:
    return EH_REGISTER_ACK_ERR_DUPLICATE_FULL;
  default:
    return EH_REGISTER_ACK_UNKNOWN_ERR;
  }
}


static EH_CHECK_RULE_ACK EH_check_rule_id_(EH_RULE_ID id)
{
  if (id < 0) return EH_CHECK_RULE_ACK_INVALID_RULE_ID;
  if (id >= EH_RULE_MAX) return EH_CHECK_RULE_ACK_INVALID_RULE_ID;
  if (event_handler_.rule_table.rules[id].settings.event.group == (EL_GROUP)EL_CORE_GROUP_NULL)
  {
    return EH_CHECK_RULE_ACK_UNREGISTERED;
  }
  return EH_CHECK_RULE_ACK_OK;
}


static void EH_exec_func_by_event_group_(EL_GROUP group,
                                         EH_CHECK_RULE_ACK (*func)(EH_RULE_ID))
{
  uint16_t least_found_sorted_idx = 0;
  uint16_t found_sorted_idx_num = 0;
  EH_RULE_SORTED_INDEX_ACK search_ack;
  uint16_t i;

  search_ack = EH_search_rule_table_index_by_event_group_(group,
                                                          &least_found_sorted_idx,
                                                          &found_sorted_idx_num);
  if (search_ack == EH_RULE_SORTED_INDEX_ACK_NOT_FOUND)
  {
    // 操作すべき EH_Rule はなし
    return;
  }
  if (search_ack != EH_RULE_SORTED_INDEX_ACK_OK || found_sorted_idx_num == 0)
  {
    // 何かがおかしい（ありえないが，安全のため入れている．問題なさそうなら消してよし）
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)group);
    return;
  }

  for (i = 0; i < found_sorted_idx_num; ++i)
  {
    func(event_handler_.sorted_idxes[least_found_sorted_idx + i].rule_id);
  }
}


EH_CHECK_RULE_ACK EH_init_rule(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  EH_activate_rule(id);
  EH_clear_rule_counter(id);
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_init_rule_for_multi_level(EH_RULE_ID id)
{
  int i;
  EH_RULE_ID next_rule_id = id;
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  // 無限ループ回避のため for で
  for (i = 0; i < (int)EH_RULE_MAX; ++i)
  {
    if (EH_activate_rule(next_rule_id) != EH_CHECK_RULE_ACK_OK) break;
    EH_clear_rule_counter(next_rule_id);
    if (event_handler_.rule_table.rules[next_rule_id].settings.event.group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE)
    {
      break;
    }
    next_rule_id = (EH_RULE_ID)event_handler_.rule_table.rules[next_rule_id].settings.event.local;
  }

  return EH_CHECK_RULE_ACK_OK;
}


void EH_init_rule_by_event_group(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_init_rule);
}


void EH_init_rule_by_event_group_for_multi_level(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_init_rule_for_multi_level);
}


EH_CHECK_RULE_ACK EH_activate_rule(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  event_handler_.rule_table.rules[id].settings.is_active = 1;
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_inactivate_rule(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  event_handler_.rule_table.rules[id].settings.is_active = 0;
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_activate_rule_for_multi_level(EH_RULE_ID id)
{
  int i;
  EH_RULE_ID next_rule_id = id;
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  // 無限ループ回避のため for
  for (i = 0; i < (int)EH_RULE_MAX; ++i)
  {
    if (EH_activate_rule(next_rule_id) != EH_CHECK_RULE_ACK_OK) break;
    if (event_handler_.rule_table.rules[next_rule_id].settings.event.group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE)
    {
      break;
    }
    next_rule_id = (EH_RULE_ID)event_handler_.rule_table.rules[next_rule_id].settings.event.local;
  }

  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_inactivate_rule_for_multi_level(EH_RULE_ID id)
{
  int i;
  EH_RULE_ID next_rule_id = id;
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  // 無限ループ回避のため for
  for (i = 0; i < (int)EH_RULE_MAX; ++i)
  {
    if (EH_inactivate_rule(next_rule_id) != EH_CHECK_RULE_ACK_OK) break;
    if (event_handler_.rule_table.rules[next_rule_id].settings.event.group != (EL_GROUP)EL_CORE_GROUP_EH_MATCH_RULE)
    {
      break;
    }
    next_rule_id = (EH_RULE_ID)event_handler_.rule_table.rules[next_rule_id].settings.event.local;
  }

  return EH_CHECK_RULE_ACK_OK;
}


void EH_activate_rule_by_event_group(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_activate_rule);
}


void EH_inactivate_rule_by_event_group(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_inactivate_rule);
}


void EH_activate_rule_by_event_group_for_multi_level(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_activate_rule_for_multi_level);
}


void EH_inactivate_rule_by_event_group_for_multi_level(EL_GROUP group)
{
  EH_exec_func_by_event_group_(group, EH_inactivate_rule_for_multi_level);
}


uint8_t EH_get_rule_is_active(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return 0;

  return event_handler_.rule_table.rules[id].settings.is_active;
}

EH_CHECK_RULE_ACK EH_set_rule_counter(EH_RULE_ID id, uint16_t counter)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  event_handler_.rule_table.rules[id].counter = counter;
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_clear_rule_counter(EH_RULE_ID id)
{
  return EH_set_rule_counter(id, 0);
}


void EH_clear_rule_counter_by_event(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level)
{
  EH_RULE_SORTED_INDEX_ACK search_ack;
  EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint8_t found_id_num;
  uint8_t i;

  search_ack = EH_search_rule_table_index_(group,
                                           local,
                                           found_ids,
                                           found_sorted_idxes,
                                           &found_id_num);

  if (search_ack == EH_RULE_SORTED_INDEX_ACK_NOT_FOUND)
  {
    // 対応する EH_Rule なし
    return;
  }
  if (search_ack != EH_RULE_SORTED_INDEX_ACK_OK)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_SEARCH_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)search_ack);
    return;
  }

  // 見つかったルールに対して処理
  for (i = 0; i < found_id_num; ++i)
  {
    EH_RULE_ID id = found_ids[i];
    EH_RuleSettings* rule_settings = &event_handler_.rule_table.rules[id].settings;

    if (rule_settings->should_match_err_level)
    {
      if (rule_settings->event.err_level != err_level) continue;
    }
    EH_clear_rule_counter(id);
  }
}


void EH_match_event_counter_to_el(void)
{
  uint8_t err_level;
  event_handler_.el_event_counter.counter_total = event_logger->statistics.record_counter_total;
  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    event_handler_.el_event_counter.counters[err_level] = event_logger->statistics.record_counters[err_level];
  }
}


const EH_Log* EH_get_the_nth_log_from_the_latest(uint16_t n)
{
  uint16_t idx;

  if (n >= EH_LOG_MAX)
  {
    // 仕方がないので，最新のものを
    idx = 0;
  }
  else
  {
    idx = (uint16_t)((event_handler->log_table.log_wp - 1 - n + EH_LOG_MAX) % EH_LOG_MAX);
  }

  return &event_handler->log_table.logs[idx];
}


CCP_CmdRet Cmd_EH_INIT(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_initialize();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_CLEAR_ALL_RULE(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_clear_rules_();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_LOAD_DEFAULT_RULE(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_load_default_rules();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_REGISTER_RULE_EVENT_PARAM(const CommonCmdPacket* packet)
{
  // 登録する瞬間にしかわからないので，ここでは値のアサーションはせず，
  // Cmd_EH_REGISTER_RULE でアサーションする
  event_handler_.reg_from_cmd.rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  event_handler_.reg_from_cmd.settings.event.group = (EL_GROUP)CCP_get_param_from_packet(packet, 1, uint32_t);
  event_handler_.reg_from_cmd.settings.event.local = CCP_get_param_from_packet(packet, 2, uint32_t);
  event_handler_.reg_from_cmd.settings.event.err_level = (EL_ERROR_LEVEL)CCP_get_param_from_packet(packet, 3, uint8_t);
  event_handler_.reg_from_cmd.settings.should_match_err_level = CCP_get_param_from_packet(packet, 4, uint8_t);
  event_handler_.reg_from_cmd.settings.deploy_bct_id = CCP_get_param_from_packet(packet, 5, bct_id_t);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_REGISTER_RULE_CONDITION_PARAM(const CommonCmdPacket* packet)
{
  // 登録する瞬間にしかわからないので，ここでは値のアサーションはせず，
  // Cmd_EH_REGISTER_RULE でアサーションする
  event_handler_.reg_from_cmd.settings.condition.type = (EH_RESPONSE_CONDITION_TYPE)CCP_get_param_from_packet(packet, 0, uint8_t);
  event_handler_.reg_from_cmd.settings.condition.count_threshold = CCP_get_param_from_packet(packet, 1, uint16_t);
  event_handler_.reg_from_cmd.settings.condition.time_threshold_ms = CCP_get_param_from_packet(packet, 2, uint32_t);
  event_handler_.reg_from_cmd.settings.is_active = CCP_get_param_from_packet(packet, 3, uint8_t);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_REGISTER_RULE(const CommonCmdPacket* packet)
{
  (void)packet;
  event_handler_.reg_from_cmd.register_ack =
      EH_register_rule(event_handler_.reg_from_cmd.rule_id, &event_handler_.reg_from_cmd.settings);

  switch (event_handler_.reg_from_cmd.register_ack)
  {
  case EH_REGISTER_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_REGISTER_ACK_ILLEGAL_RULE_ID:
  case EH_REGISTER_ACK_ILLEGAL_GROUP:
  case EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL:
  case EH_REGISTER_ACK_ILLEGAL_MATCH_FLAG:
  case EH_REGISTER_ACK_ILLEGAL_CONDITION_TYPE:
  case EH_REGISTER_ACK_ILLEGAL_COUNT_THRESHOLD:
  case EH_REGISTER_ACK_ILLEGAL_BCT_ID:
  case EH_REGISTER_ACK_ILLEGAL_ACTIVE_FLAG:
  case EH_REGISTER_ACK_ILLEGAL_MULTI_LEVEL:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);    // 正確にはこのコマンドのパラメタではないが．．．
  case EH_REGISTER_ACK_ERR_FULL:
  case EH_REGISTER_ACK_ERR_RULE_OVERWRITE:
  case EH_REGISTER_ACK_ERR_DUPLICATE_FULL:
  case EH_REGISTER_ACK_UNKNOWN_ERR:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_DELETE_RULE(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_RULE_SORTED_INDEX_ACK ack = EH_delete_rule_table_(rule_id);

  switch (ack)
  {
  case EH_RULE_SORTED_INDEX_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_RULE_SORTED_INDEX_ACK_NOT_FOUND:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_INIT_RULE(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_init_rule(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_INIT_RULE_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_init_rule_for_multi_level(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_ACTIVATE_RULE(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_activate_rule(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_INACTIVATE_RULE(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_inactivate_rule(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_ACTIVATE_RULE_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_activate_rule_for_multi_level(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_INACTIVATE_RULE_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_inactivate_rule_for_multi_level(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_SET_RULE_COUNTER(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  uint16_t counter = CCP_get_param_from_packet(packet, 1, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_set_rule_counter(rule_id, counter);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_CLEAR_RULE_COUNTER(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_CHECK_RULE_ACK ack = EH_clear_rule_counter(rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_EH_CLEAR_RULE_COUNTER_BY_EVENT(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  uint32_t local = (EL_GROUP)CCP_get_param_from_packet(packet, 1, uint32_t);
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_from_packet(packet, 2, uint8_t);

  EH_clear_rule_counter_by_event(group, local, err_level);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_CLEAR_LOG(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_clear_log_();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_MAX_RESPONSE_NUM(const CommonCmdPacket* packet)
{
  event_handler_.exec_settings.max_response_num = CCP_get_param_from_packet(packet, 0, uint8_t);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_MAX_CHECK_EVENT_NUM(const CommonCmdPacket* packet)
{
  event_handler_.exec_settings.max_check_event_num = CCP_get_param_from_packet(packet, 0, uint16_t);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_MAX_MULTI_LEVEL_NUM(const CommonCmdPacket* packet)
{
  event_handler_.exec_settings.max_multi_level_num = CCP_get_param_from_packet(packet, 0, uint8_t);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_PAGE_OF_RULE_TABLE_FOR_TLM(const CommonCmdPacket* packet)
{
  uint8_t page = CCP_get_param_from_packet(packet, 0, uint8_t);
  if (page >= EH_RULE_TLM_PAGE_MAX) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  event_handler_.tlm_info.rule.page_no = page;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_PAGE_OF_RULE_SORTED_IDX_FOR_TLM(const CommonCmdPacket* packet)
{
  uint8_t page = CCP_get_param_from_packet(packet, 0, uint8_t);
  if (page >= EH_RULE_TLM_PAGE_MAX) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  event_handler_.tlm_info.rule_sorted_index.page_no = page;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_PAGE_OF_LOG_TABLE_FOR_TLM(const CommonCmdPacket* packet)
{
  uint8_t page = CCP_get_param_from_packet(packet, 0, uint8_t);
  if (page >= EH_LOG_TLM_PAGE_MAX) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  event_handler_.tlm_info.log.page_no = page;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_SET_TARGET_ID_OF_RULE_TABLE_FOR_TLM(const CommonCmdPacket* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);

  if (EH_check_rule_id_(rule_id) == EH_CHECK_RULE_ACK_INVALID_RULE_ID)
  {
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  }

  event_handler_.tlm_info.rule.target_rule_id = rule_id;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_MATCH_EVENT_COUNTER_TO_EL(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_match_event_counter_to_el();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_INIT_RULE_BY_EVENT_GROUP(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_init_rule_by_event_group(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_INIT_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_init_rule_by_event_group_for_multi_level(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_ACTIVATE_RULE_BY_EVENT_GROUP(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_activate_rule_by_event_group(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_INACTIVATE_RULE_BY_EVENT_GROUP(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_inactivate_rule_by_event_group(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_ACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_activate_rule_by_event_group_for_multi_level(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_EH_INACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL(const CommonCmdPacket* packet)
{
  EL_GROUP group = (EL_GROUP)CCP_get_param_from_packet(packet, 0, uint32_t);
  EH_inactivate_rule_by_event_group_for_multi_level(group);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#endif  // EL_IS_ENABLE_TLOG

#pragma section
