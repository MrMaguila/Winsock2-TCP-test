echo "building server..."
gcc -o server server.c -lws2_32 2>&1;

echo "building client...";
gcc -o client client.c -lws2_32 2>&1;

read -p "Press enter to continue";