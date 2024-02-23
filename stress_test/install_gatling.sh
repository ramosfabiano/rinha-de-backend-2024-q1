#!/bin/bash


INSTALL_DIR="$HOME/bin/"
GATLING_VERSION="3.9.5"

if [ ! -d "$INSTALL_DIR" ]; then
    echo "Invalid install directory: $INSTALL_DIR"
    exit -1
fi

absolute_install_path=$(realpath "$INSTALL_DIR")"/gatling-$GATLING_VERSION/"

if [ -d "$absolute_install_path" ]; then
    echo "Gatling already installed in $absolute_install_path"
else
    tmp_directory=$(mktemp -d)
    curr_dir=$(pwd)

    cd "$tmp_directory" && \
    echo "Downloading Gatling ${GATLING_VERSION}" && \
    curl -fsSL "https://repo1.maven.org/maven2/io/gatling/highcharts/gatling-charts-highcharts-bundle/${GATLING_VERSION}/gatling-charts-highcharts-bundle-3.9.5-bundle.zip" > ./gatling.zip && \
    rm -rf ./gatling && \
    unzip gatling.zip && \
    mv gatling-charts-highcharts-bundle-3.9.5 gatling-$GATLING_VERSION && \
    mv gatling-$GATLING_VERSION $absolute_install_path && \
    rm gatling.zip && \
    cd $curr_dir && \
    rm -rf "$tmp_directory" && \
    echo "All done!" 
fi
