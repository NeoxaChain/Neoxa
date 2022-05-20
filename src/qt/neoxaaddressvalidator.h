// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Neoxa Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NEOXA_QT_NEOXAADDRESSVALIDATOR_H
#define NEOXA_QT_NEOXAADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class NeoxaAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit NeoxaAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** neoxa address widget validator, checks for a valid neoxa address.
 */
class NeoxaAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit NeoxaAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // NEOXA_QT_NEOXAADDRESSVALIDATOR_H
