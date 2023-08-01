# C2A SUB OBC USER SAMPLE for CORE DEVELOPMENT

## 概要
C2A 間通信の開発のために， [`examples/mobc`](/examples/mobc) とセットで実行し，それと通信し合うことを目的とした sub OBC (非 MOBC) の User Sample.

## 注意
- sub OBC という名前だが，コード上は AOBC を想定している．（APID など）
- あくまで C2A 間通信開発のためなので，最小限の Tlm, Cmd などしか登録されてないなど，大幅に省略している．
- したがって，C2A の User のサンプルとしては， [`examples/mobc`](/examples/mobc) を参照するほうが望ましい．
- 一方で， MOBC との通信部分や， sub OBC としての振る舞いについてはこちらを参照すること．
- C2A 間通信については， [`docs/Driver/Communication with Components`](/docs/driver/communication_with_components.md) も参照すること．
