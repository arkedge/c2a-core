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
[C2A DevTools](https://github.com/arkedge/gaia/tree/v0.6.1/tmtc-c2a/devtools_frontend) が有効な [Gaia](https://github.com/arkedge/gaia) (tmtc-c2a) のビルド (現状では `npm install` 時に内部で行われる) に必要．

インストール: https://classic.yarnpkg.com/lang/en/docs/install/

### Rye
Rye は Python 版 rustup である．
PyPI のパッケージだけでなく，Python インタプリタそれ自体のバージョンも管理可能．

インストール: https://rye-up.com/guide/installation/
