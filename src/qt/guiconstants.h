// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2020 The Neoxa developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_GUICONSTANTS_H
#define BITCOIN_QT_GUICONSTANTS_H

/* Milliseconds between model updates */
static const int MODEL_UPDATE_DELAY = 250;

/* AskPassphraseDialog -- Maximum passphrase length */
static const int MAX_PASSPHRASE_SIZE = 1024;

/* NeoxaGUI -- Size of icons in status bar */
static const int STATUSBAR_ICONSIZE = 16;

static const bool DEFAULT_SPLASHSCREEN = true;

/* Tooltips longer than this (in characters) are converted into rich text,
   so that they can be word-wrapped.
 */
static const int TOOLTIP_WRAP_THRESHOLD = 80;

/* Maximum allowed URI length */
static const int MAX_URI_LENGTH = 255;

/* QRCodeDialog -- size of exported QR Code image */
#define QR_IMAGE_SIZE 300

/* Number of frames in spinner animation */
#define SPINNER_FRAMES 36

#define QAPP_ORG_NAME "Neoxa"
#define QAPP_ORG_DOMAIN "neoxa.net"
#define QAPP_APP_NAME_DEFAULT "Neoxa-Qt-v2"
#define QAPP_APP_NAME_TESTNET "Neoxa-Qt-v2-testnet"
#define QAPP_APP_NAME_DEVNET "Neoxa-Qt-v2-%s"
#define QAPP_APP_NAME_REGTEST "Neoxa-Qt-v2-regtest"

#endif // BITCOIN_QT_GUICONSTANTS_H
