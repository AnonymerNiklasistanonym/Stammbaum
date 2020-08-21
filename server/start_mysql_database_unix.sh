#!/usr/bin/env bash

# Stop if an error occurs
set -e

echo -e "If it is the first time starting mysql (mariadb):"
echo -e "\t- Install maridab by running"
echo -e "\t  \$ sudo mariadb-install-db --user=mysql --basedir=/usr --datadir=/var/lib/mysql"
echo -e "\t  \$ sudo systemctl start mariadb.service"
echo -e "\t- You can change the mysql password by running"
echo -e "\t  \$ sudo mysql_secure_installation"
echo -e "\t- Start the mysql (mariadb) console by running"
echo -e "\t  \$ sudo mysql -u root -p"
echo -e "\t\t- Create a new database by running"
echo -e "\t\t  MariaDB [(none)]> CREATE DATABASE cpptestdb;"
echo -e "\t\t- Create a new user (with no password for testing) that has full access to this database by running"
echo -e "\t\t  MariaDB [(none)]> GRANT ALL ON cpptestdb.* TO cpptestuser@localhost IDENTIFIED BY '';"
echo -e "\t\t  MariaDB [(none)]> FLUSH privileges;"
