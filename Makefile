
build: build-sock-stream-server build-sock-stream-client build-dgram-server build-dgram-client build-test build-download_full_response


build-sock-stream-server:
	clang -g -O0 ./sock_stream_server.c -o ./out/sock_stream_server

build-sock-stream-client:
	clang -g -O0 ./sock_stream_client.c -o ./out/sock_stream_client


build-dgram-server:
	clang -g -O0 ./dgram_server.c -o ./out/dgram_server

build-dgram-client:
	clang -g -O0 ./dgram_client.c -o ./out/dgram_client

build-download_full_response:
	clang -g -O0 ./download_full_response.c -o ./out/download_full_response

build-test:
	clang -g -O0 ./test.c -o ./out/test
