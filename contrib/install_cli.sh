 #!/usr/bin/env bash

 # Execute this file to install the neoxa cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Neoxa-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Neoxa-Qt.app/Contents/MacOS/neoxad /usr/local/bin/neoxad
 sudo ln -s ${LOCATION}/Neoxa-Qt.app/Contents/MacOS/neoxa-cli /usr/local/bin/neoxa-cli
