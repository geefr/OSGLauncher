FROM debian:9
LABEL project="OSGLauncher"
LABEL MAINTAINER Gareth Francis (gfrancis.dev@gmail.com)
RUN apt-get update && apt-get install -y cmake gcc g++
RUN apt-get install -y libopenscenegraph-3.4-dev
RUN apt-get install -y pkg-config libtinyxml2-dev
