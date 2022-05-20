## Please read https://github.com/NeoxaChain/Neoxa/blob/master/doc/build-windows.md

## STEP1: Setup dependency
# sudo apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl nsis
# sudo update-alternatives --config x86_64-w64-mingw32-g++
# sudo update-alternatives --config x86_64-w64-mingw32-gcc

## SETP2: Building for 64-bit Windows
## sudo apt-get install g++-mingw-w64-x86-64 mingw-w64-x86-64-dev

## STEP3: Run this script

NEOXA_ROOT=$(pwd)

# Pick some path to install BDB to, here we create a directory within the Neoxa directory
BDB_PREFIX="${NEOXA_ROOT}/db4"
mkdir -p $BDB_PREFIX

# Fetch the source and verify that it is not tampered with
wget -c 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
echo '12edc0df75bf9abd7f82f821795bcee50f42cb2e5f76a6a281b85732798364ef  db-4.8.30.NC.tar.gz' | sha256sum -c
# -> db-4.8.30.NC.tar.gz: OK
tar -xzvf db-4.8.30.NC.tar.gz

# copy patch to fix error in db4
chmod a+w ./db-4.8.30.NC/dbinc/atomic.h
cp ./depends/patches/atomic.h db-4.8.30.NC/dbinc/

# Build the library and install to our prefix
cd db-4.8.30.NC/build_unix/
#  Note: Do a static build so that it can be embedded into the executable, instead of having to find a .so at runtime
../dist/configure --enable-cxx --disable-shared --with-pic --prefix=$BDB_PREFIX
make
make install

# Configure Neoxa Core to use our own-built instance of BDB
cd $NEOXA_ROOT
PATH=$(echo "$PATH" | sed -e 's/:\/mnt.*//g') # strip out problematic Windows %PATH% imported var
cd depends
make HOST=x86_64-w64-mingw32
cd ..
./autogen.sh # not required when building from tarball
CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --prefix=/ LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" # (other args...)
make -j4
make deploy
