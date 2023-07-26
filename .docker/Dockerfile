FROM ubuntu:20.04

RUN apt-get update
RUN apt-get install -y make g++ cmake
RUN apt-get install -y llvm-dev libgmock-dev
RUN apt-get install -y git
RUN apt-get install -y libgtest-dev && cd /usr/src/gtest && cmake CMakeLists.txt && make && cp lib/*.a /usr/lib && ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a && ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a
RUN apt-get install -y bash-completion
COPY .bashrc /root/.bashrc
