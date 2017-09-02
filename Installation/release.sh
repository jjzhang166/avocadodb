#!/bin/bash
set -ex

SCRIPT_DIR=`dirname $0`
SRC_DIR="${SCRIPT_DIR}/../"
ENTERPRISE_SRC_DIR=${SRC_DIR}enterprise

FORCE_TAG=0
TAG=1
BOOK=1
BUILD=1
SWAGGER=1
EXAMPLES=1
LINT=1
PARALLEL=8

SED=sed
isMac=0
if test "$(uname)" == "Darwin"; then
    isMac=1
    SED=gsed
    OSNAME=darwin
fi

if flex --version; then
    echo "flex found."
else
    echo "flex missing from your system"
    exit 1
fi

if npm --version; then
    echo "npm found."
else
    echo "npm missing from your system"
    exit 1
fi

if grunt --version; then
    echo "grunt found."
else
    echo "grunt missing from your system"
    exit 1
fi

if gitbook --version; then
    echo "gitbook found."
else
    echo "gitbook missing from your system"
    exit 1
fi

if [ "$#" -lt 1 ];  then
    echo "usage: $0 <major>.<minor>.<revision>"
    exit 1
fi

while [ "$#" -gt 0 ];  do
    echo "$1"
    case "$1" in
        --parallel)
            shift
            PARALLEL=$1
            shift
            ;;
        
        --force-tag)
            shift
            FORCE_TAG=1
            ;;
        
        --no-lint)
            LINT=0
            shift
            ;;

        --no-build)
            BUILD=0
            shift
            ;;

        --recycle-build)
            BUILD=2
            shift
            ;;

        --no-swagger)
            SWAGGER=0
            shift
            ;;

        --no-examples)
            EXAMPLES=0
            shift
            ;;

        --no-commit)
            TAG=0
            shift
            ;;

        --no-book)
            BOOK=0
            shift
            ;;
        *)
            if test -n "${VERSION}"; then
                echo "we already have a version ${VERSION} aborting because of $1"
                exit 1
            fi
            VERSION="$1"
            shift
            ;;
    esac
done

if [ -d ${ENTERPRISE_SRC_DIR} ];  then
    echo "enterprise directory missing"
fi

if echo ${VERSION} | grep -q -- '-'; then
    echo "${VERSION} mustn't contain minuses! "
    exit 1
fi

if test "${FORCE_TAG}" == 0; then
    if git tag | grep -q "^v$VERSION$";  then
        echo "$0: version $VERSION already defined"
        exit 1
    fi
fi

if fgrep -q "v$VERSION" CHANGELOG;  then
    echo "version $VERSION defined in CHANGELOG" 
else
    echo "$0: version $VERSION not defined in CHANGELOG"
    exit 1
fi


GITSHA=`git log -n1 --pretty='%h'`
if git describe --exact-match --tags ${GITSHA}; then
    GITARGS=`git describe --exact-match --tags ${GITSHA}`
    echo "I'm on tag: ${GITARGS}"
else
    GITARGS=`git branch --no-color| grep '^\*' | sed "s;\* *;;"`
    if echo $GITARGS |grep -q ' '; then
        GITARGS=devel
    fi
    echo "I'm on Branch: ${GITARGS}"
fi
(cd enterprise; git checkout master; git fetch --tags; git pull --all; git checkout ${GITARGS}; git pull )



VERSION_MAJOR=`echo $VERSION | awk -F. '{print $1}'`
VERSION_MINOR=`echo $VERSION | awk -F. '{print $2}'`
VERSION_REVISION=`echo $VERSION | awk -F. '{print $3}'`
VERSION_PACKAGE="1"

cat CMakeLists.txt \
    | sed -e "s~set(ARANGODB_VERSION_MAJOR.*~set(ARANGODB_VERSION_MAJOR      \"$VERSION_MAJOR\")~" \
    | sed -e "s~set(ARANGODB_VERSION_MINOR.*~set(ARANGODB_VERSION_MINOR      \"$VERSION_MINOR\")~" \
    | sed -e "s~set(ARANGODB_VERSION_REVISION.*~set(ARANGODB_VERSION_REVISION   \"$VERSION_REVISION\")~" \
    | sed -e "s~set(ARANGODB_PACKAGE_REVISION.*~set(ARANGODB_PACKAGE_REVISION   \"$VERSION_PACKAGE\")~" \
          > CMakeLists.txt.tmp

mv CMakeLists.txt.tmp CMakeLists.txt

CMAKE_CONFIGURE="-DUSE_MAINTAINER_MODE=ON"

if [ `uname` == "Darwin" ];  then
    CMAKE_CONFIGURE="${CMAKE_CONFIGURE} -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11"
fi

if [ "$BUILD" != "0" ];  then
    echo "COMPILING COMMUNITY"
    rm -rf build && mkdir build
    (
        cd build
        cmake .. ${CMAKE_CONFIGURE}
        make clean_autogenerated_files
        cmake .. ${CMAKE_CONFIGURE}
        make -j ${PARALLEL}
    )

    echo "COMPILING ENTERPRISE"
    rm -rf build-enterprise && mkdir build-enterprise
    (
        cd build-enterprise
        cmake .. ${CMAKE_CONFIGURE} -DUSE_ENTERPRISE=ON
        make -j ${PARALLEL}
    )
fi

if [ "$LINT" == "1" ]; then
    echo "LINTING"
    ./utils/jslint.sh
fi

# we utilize https://developer.github.com/v3/repos/ to get the newest release of the arangodb starter:
curl -s https://api.github.com/repos/arangodb-helper/arangodb/releases | \
                         grep tag_name | \
                         head -n 1 | \
                         ${SED} -e "s;.*: ;;" -e 's;";;g' -e 's;,;;' > STARTER_REV

git add -f \
    README \
    arangod/Aql/tokens.cpp \
    arangod/Aql/grammar.cpp \
    arangod/Aql/grammar.h \
    lib/V8/v8-json.cpp \
    lib/Basics/voc-errors.h \
    lib/Basics/voc-errors.cpp \
    js/common/bootstrap/errors.js \
    CMakeLists.txt \
    STARTER_REV

if [ "$EXAMPLES" == "1" ];  then
    echo "EXAMPLES"
    ./utils/generateExamples.sh
fi

if [ "$SWAGGER" == "1" ];  then
    echo "SWAGGER"
    ./utils/generateSwagger.sh
fi

echo "GRUNT"
(
    cd js/apps/system/_admin/aardvark/APP
    rm -rf node_modules
    npm install
    grunt deploy
)

git add -f Documentation/Examples/*.generated

if [ "$BOOK" == "1" ];  then
    echo "DOCUMENTATION"
    (cd Documentation/Books; make)
fi

case "$TAG" in
    *-milestone*|*-alpha*|*-beta*|devel)
    ;;

    *)
        if test -f EXPERIMENTAL; then git rm -f EXPERIMENTAL; fi
        ;;
esac

if [ "$TAG" == "1" ];  then
    echo "COMMIT"

    git commit -m "release version $VERSION" -a
    git push

    if test "${FORCE_TAG}" == 0; then
        git tag "v$VERSION"
        git push --tags
    else
        git tag -f "v$VERSION"
        git push --tags -f
    fi        

    cd ${ENTERPRISE_SRC_DIR}
    git commit --allow-empty -m "release version $VERSION enterprise" -a
    git push

    if test "${FORCE_TAG}" == 0; then
        git tag "v$VERSION"
        git push --tags
    else
        git tag -f "v$VERSION"
        git push --tags -f
    fi
    
    echo
    echo "--------------------------------------------------"
    echo "Remember to update the VERSION in 'devel' as well."
    echo "--------------------------------------------------"
fi