set -ex
make clean
make
./mail_server ./users.txt
