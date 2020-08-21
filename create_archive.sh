#!/usr/bin/env bash

pip3 install git-archive-all
mkdir -p bin
git-archive-all --force-submodules bin/git_archive.tar.gz
