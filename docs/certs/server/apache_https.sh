
openssl genrsa -des3 -out server.key 1024
openssl req -new -key server.key -out server.csr
cp server.key server.key.orgopenssl rsa -in server.key.org -out server.key
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
sudo mkdir /etc/apache2/ssl
sudo cp server.crt /etc/apache2/ssl/server.crt

sudo cp server.key /etc/apache2/ssl/server.key
sudo a2enmod ssl
sudo ln -s /etc/apache2/sites-available/default-ssl.conf /etc/apache2/sites-enabled/000-default-ssl.conf

# Change /etc/apache2/sites-enabled/000-default-ssl.conf 
#  SSLCertificateFile /etc/apache2/ssl/server.crt
#  SSLCertificateKeyFile /etc/apache2/ssl/server.key

sudo systemctl restart apache2.service
