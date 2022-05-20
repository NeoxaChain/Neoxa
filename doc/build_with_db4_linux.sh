## Build for linux version, see /doc/build-unix.md


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
cd depends
make HOST=x86_64-pc-linux-gnu

cd ..
./autogen.sh
#CONFIG_SITE=$PWD/depends/x86_64-pc-linux-gnu/share/config.site ./configure  --prefix=$PWD/depends/x86_64-pc-linux-gnu --enable-cxx --disable-shared --disable-tests --disable-gui-tests --enable-static=yes --with-pic LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" # (other args...) 
CONFIG_SITE=$PWD/depends/x86_64-pc-linux-gnu/share/config.site ./configure  --prefix=$PWD/depends/x86_64-pc-linux-gnu --enable-cxx --disable-shared --disable-tests --disable-gui-tests --with-pic LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" # (other args...) 
make -j4
make deploy
