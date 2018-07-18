# Cassandra
curl https://www.apache.org/dist/cassandra/KEYS | sudo apt-key add -
echo "deb http://www.apache.org/dist/cassandra/debian 39x main" | sudo tee -a /etc/apt/sources.list.d/cassandra.sources.list
echo "deb-src http://www.apache.org/dist/cassandra/debian 39x main" | sudo tee -a /etc/apt/sources.list.d/cassandra.sources.list
sudo apt install cassandra

# Core Apache PHP
sudo apt install php libapache2-mod-php
# Dependencias - tenha certeza que a versao do libuv >= 1
sudo apt install php-pear php-dev libgmp-dev libpcre3-dev g++ make cmake libssl-dev openssl php7.0-dev libuv1 libuv1-dev

# PHP Driver
cd /tmp
git clone https://github.com/datastax/php-driver.git
cd php-driver
git submodule update --init
cd ext
sudo ./install.sh

sudo su
echo -e "; DataStax PHP Driver\nextension=cassandra.so" >> `php --ini | grep "Loaded Configuration" | sudo sed -e "s|.*:\s*||"`
exit
