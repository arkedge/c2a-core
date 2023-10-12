# Development Environment
次のような開発環境を想定する．

## OS
- Linux
- Windows (WSL2)

## ソフトウェア
### rustup
rustup は Rust ツールチェーンのインストーラーである．
Rust 自体のインストール・バージョンアップや，組み込み向けのクロスコンパイラをインストールすることなどができる．

インストール: https://www.rust-lang.org/learn/get-started

### nvm
nvm は Node.js のインストーラー・バージョンマネージャである．

インストール: https://github.com/nvm-sh/nvm#installing-and-updating

### yarn
C2A 開発そのものには不要であるが，[c2a-devtools](https://github.com/arkedge/c2a-devtools) の実行に必要である．

インストール: https://classic.yarnpkg.com/lang/en/docs/install/

### Rye
Rye は Python 版 rustup である．
PyPI のパッケージだけでなく，Python インタプリタそれ自体のバージョンも管理可能．

インストール: https://rye-up.com/guide/installation/
