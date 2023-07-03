#!/bin/bash
# use testnet settings,  if you need mainnet,  use ~/.neoxacore/neoxad.pid file instead
neoxa_pid=$(<~/.neoxacore/testnet3/neoxad.pid)
sudo gdb -batch -ex "source debug.gdb" neoxad ${neoxa_pid}
