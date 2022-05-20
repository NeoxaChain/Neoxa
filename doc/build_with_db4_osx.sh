echo 'Please read  https://github.com/NeoxaChain/Neoxa/blob/main/doc/build-osx.md '
echo 'Run this script under macOS  '

NEOXA_ROOT=$(pwd)
# build db4
CFLAGS="-Wno-error=implicit-function-declaration"  ./contrib/install_db4.sh .

BDB_PREFIX="${NEOXA_ROOT}/db4"

# Configure Neoxa Core to use our own-built instance of BDB
cd $NEOXA_ROOT
./autogen.sh
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" --enable-cxx --disable-shared --disable-tests --disable-gui-tests
make -j4
make deploy
