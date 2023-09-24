echo "building server.c";
eval "gcc -o server server.c" 2>&1;
echo "building client.c";
eval "gcc -o client client.c" 2>&1;
