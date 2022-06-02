#!/bin/bash

# cd /var/lib/jenkins/jobs/redemption-future/workspace
cd $(realpath -m "$0/../..")

if ! [[ $PWD =~ ^/var/lib/jenkins/jobs/* ]]; then
  echo 'Please, run script on Jenkins.'
  exit 1
fi

set -ex

typeset -i fast=0
if [ "$1" = fast ]; then
    fast=1
fi

if [ $fast -eq 0 ]; then
    ./tools/c++-analyzer/redemption-analyzer.sh
fi


#These following packages MUST be installed. See README of redemption project
#aptitude install build-essential bjam boost-build libboost-program-options-dev libboost-test-dev libssl-dev locales cmake

# BJAM Build Test
echo -e "using gcc : 9.0 : g++-9 -DREDEMPTION_DISABLE_NO_BOOST_PREPROCESSOR_WARNING ;\nusing clang : : clang++ -DREDEMPTION_DISABLE_NO_BOOST_PREPROCESSOR_WARNING ;" > project-config.jam
valgrind_compiler=gcc-9
toolset_gcc=toolset=gcc-9
toolset_clang=toolset=clang
export FFMPEG_INC_PATH=/usr/local/include/ffmpeg/
export FFMPEG_LIB_PATH=/usr/local/lib/ffmpeg
export FFMPEG_LINK_MODE=static

export LSAN_OPTIONS=exitcode=0 # re-trace by valgrind

export BOOST_TEST_COLOR_OUTPUT=0

if [ $fast -eq 0 ]; then
    rm -rf bin
fi

# export REDEMPTION_LOG_PRINT=1
export REDEMPTION_LOG_PRINT=0
export cxx_color=never

export BOOST_TEST_RANDOM=$RANDOM
echo random seed = $BOOST_TEST_RANDOM

build()
{
    bjam -q "$@" || {
        local e=$?
        export REDEMPTION_LOG_PRINT=1
        bjam -q "$@"
        exit $e
    }
}

rootlist()
{
    ls -1
}

# implicitly created by bjam
mkdir -p bin
beforerun=$(rootlist)

# release for -Warray-bounds and not assert
# build $toolset_gcc cxxflags=-g
# multi-thread
big_mem='exe libs
  tests/capture
  tests/lib
  tests/server
  tests/client_mods
  tests/mod/rdp.norec
  tests/mod/vnc.norec'
build $toolset_gcc cxxflags=-g -j2 ocr_tools
build $toolset_gcc cxxflags=-g $big_mem
build $toolset_gcc cxxflags=-g -j2

dirdiff=$(diff <(echo "$beforerun") <(rootlist)) || {
  echo 'New file(s):'
  echo "$dirdiff"
  exit 1
}

#bjam -a -q toolset=clang-8 -sNO_FFMPEG=1 san
build $toolset_clang -sNO_FFMPEG=1 san -j3 ocr_tools -s FAST_CHECK=1
build $toolset_clang -sNO_FFMPEG=1 san $big_mem -s FAST_CHECK=1
build $toolset_clang -sNO_FFMPEG=1 san -j2 -s FAST_CHECK=1


if [ $fast -eq 0 ]; then
    # debug with coverage
    build $toolset_gcc debug -scoverage=on covbin=gcov-7 -s FAST_CHECK=1


    # cppcheck
    # ./tools/c++-analyzer/cppcheck-filtered 2>&1 1>/dev/null


    # extract TODO, BUG, etc
    find \
      src \
      tests \
      projects/ClientQtGraphicAPI/src/ \
      projects/redemption_configs/ \
      \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' \) \
      -exec ./tools/c++-analyzer/todo_extractor '{}' +


    #set -o pipefail

    # clang analyzer
    CLANG_TIDY=clang-tidy ./tools/c++-analyzer/clang-tidy \
      | sed -E '/^(.+\/|)modules\//,/\^/d'


    # valgrind
    #find ./bin/$gcc/release/tests/ -type d -exec \
    #  ./tools/c++-analyzer/valgrind -qd '{}' \;
    find ./bin/$valgrind_compiler/release/tests/ -type d -exec \
      parallel -j2 ./tools/c++-analyzer/valgrind -qd ::: '{}' +
fi
