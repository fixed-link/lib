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

## dev
```
sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm python3-subunit mesa-common-dev u-boot-tools mtd-utils android-sdk-ext4-utils rsync bc vim samba nfs-common lib32z1 lib32ncurses-dev
sudo apt-get install make python3-pip
sudo pip3 install sphinx sphinx_rtd_theme pyyaml
```
