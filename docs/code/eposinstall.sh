
# Install dependencies

sudo apt update && sudo apt -y install lib32stdc++6 libc6-i386 libc6-dev-i386 lib32ncurses5 lib32z1

# Install virtual host

sudo apt -y install qemu-system-arm 

# Install EPOS Mote toolchain

cd /tmp
wget -O arm-gcc-4.4.4.tar.gz epos.lisha.ufsc.br/dl88
sudo mkdir -p /usr/local/arm
sudo tar xfvz arm-gcc-4.4.4.tar.gz -C /usr/local/arm

# Clone repository

sudo apt -y install subversion
cd $HOME && mkdir -p epos && cd epos
svn checkout https://epos.lisha.ufsc.br/svn/epos2/branches/arm/

# Automated compilation test
cd arm && make APPLICATION=hello
