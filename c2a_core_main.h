#ifndef C2A_CORE_MAIN_H_
#define C2A_CORE_MAIN_H_

void C2A_core_init(void);
void C2A_core_main(void);

// C2A の バージョンは Semantic Versioning を採用する
// これらの番号は，リリース時に手動で合わせる
// 詳細: docs/general/release.md
#define C2A_CORE_VER_MAJOR (4)
#define C2A_CORE_VER_MINOR (5)
#define C2A_CORE_VER_PATCH (0)
#define C2A_CORE_VER_PRE   ("")

#endif
