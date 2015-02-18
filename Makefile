

CURL_CFLAGS = $(shell curl-config --cflags)
CURL_LIBS = $(shell curl-config --libs)
GFLAGS_LIBS = -lgflags
JSONCPP_LIBS = -ljsoncpp
PROTOBUF_LIBS = -lprotobuf -lpthread
GTEST_SRC = /usr/src/gtest/src/gtest-all.cc
GTEST_MAIN = /usr/src/gtest/src/gtest_main.cc

PROTOC = protoc --cpp_out=./

COMPILE = g++ -c --std=c++0x -g -O0 $(CURL_CFLAGS)
LINK = g++ --std=c++0x -g -O0
LINK_FLAGS = $(CURL_LIBS) $(GFLAGS_LIBS) $(PROTOBUF_LIBS) $(JSONCPP_LIBS)

EXEC = http_util http_session_test json_utils_test reddit_agent_test file_utils_test

.PHONY: all test clean

all: $(EXEC)


gtest.o: $(GTEST_SRC)
	$(COMPILE) -I/usr/src/gtest $(GTEST_SRC) -o gtest.o

gtest_main.o: $(GTEST_MAIN)
	$(COMPILE) -I/usr/src/gtest,/usr/include $(GTEST_MAIN) -o gtest_main.o

GTEST_LIBS = gtest.o gtest_main.o


test: http_session_test json_utils_test file_utils_test reddit_agent_test
	./http_session_test
	./json_utils_test
	./file_utils_test
	./reddit_agent_test

http_session.o: http_session.cc
	$(COMPILE) http_session.cc

http_util.o: http_util.cc
	$(COMPILE) http_util.cc

http_util: http_util.o http_session.o
	$(LINK) http_util.o http_session.o $(LINK_FLAGS) -o http_util

http_session_test.o: http_session_test.cc
	$(COMPILE) http_session_test.cc

http_session_test: http_session_test.o http_session.o $(GTEST_LIBS)
	$(LINK) http_session_test.o http_session.o $(GTEST_LIBS) $(LINK_FLAGS) -o http_session_test

reddit_types.pb.h: reddit_types.proto
	$(PROTOC) reddit_types.proto

reddit_types.pb.cc: reddit_types.proto
	$(PROTOC) reddit_types.proto

json_utils.o: json_utils.cc reddit_types.pb.h
	$(COMPILE) json_utils.cc

json_utils_test.o: json_utils_test.cc reddit_types.pb.h
	$(COMPILE) json_utils_test.cc

json_utils_test: json_utils.o json_utils_test.o reddit_types.pb.o $(GTEST_LIBS)
	$(LINK) json_utils.o json_utils_test.o reddit_types.pb.o $(GTEST_LIBS) $(LINK_FLAGS) -o json_utils_test

file_utils.o: file_utils.cc
	$(COMPILE) file_utils.cc

file_utils_test.o: file_utils_test.cc
	$(COMPILE) file_utils_test.cc

file_utils_test: file_utils_test.o file_utils.o
	$(LINK) file_utils.o file_utils_test.o $(GTEST_LIBS) $(LINK_FLAGS) -o file_utils_test

reddit_agent.o: reddit_agent.cc reddit_types.pb.h
	$(COMPILE) reddit_agent.cc

reddit_agent_test.o: reddit_agent_test.cc reddit_types.pb.h
	$(COMPILE) reddit_agent_test.cc

reddit_agent_test: reddit_agent_test.o reddit_agent.o json_utils.o file_utils.o http_session.o reddit_types.pb.o $(GTEST_LIBS)
	$(LINK) reddit_agent_test.o reddit_agent.o json_utils.o file_utils.o http_session.o reddit_types.pb.o $(GTEST_LIBS) $(LINK_FLAGS) -o reddit_agent_test

clean:
	rm -f *.o
	rm -f *.pb.h *.pb.cc
	rm -f $(EXEC)
