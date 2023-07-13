# Linux environment for me

## vim
```vim
set nu
set ai
set ts=4
set title
set ruler
syntax on
filetype on
set hlsearch
set mouse-=a
set noexpandtab
```

## bash
```bash
alias diff='diff -y --suppress-common-lines'
export REPO_URL='https://mirrors.bfsu.edu.cn/git/git-repo'
```

## adb
```bash
wget https://dl.google.com/android/repository/platform-tools-latest-linux.zip
```

## config
```bash
sudo dpkg-reconfigure dash
sudo update-alternatives --config editor
sudo dpkg --add-architecture i386
```
