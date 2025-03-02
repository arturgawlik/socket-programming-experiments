
build: build-server build-client build-test

build-server:
	clang -g -O0 ./server.c -o ./out/server

build-client:
	clang -g -O0 ./client.c -o ./out/client

build-test:
	clang -g -O0 ./test.c -o ./out/test
