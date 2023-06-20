// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_OVERVIEWPAGE_H
#define BITCOIN_QT_OVERVIEWPAGE_H

#include "amount.h"

#include <QWidget>
#include <QMenu>
#include <memory>

class ClientModel;
class TransactionFilterProxy;
class TxViewDelegate;
class PlatformStyle;
class WalletModel;
class AssetFilterProxy;

class AssetViewDelegate;

namespace Ui {
    class OverviewPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Overview ("home") page widget */
class OverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~OverviewPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void showOutOfSyncWarning(bool fShow);
    void showAssets();

    bool eventFilter(QObject *object, QEvent *event);
    void openIPFSForAsset(const QModelIndex &index);

public Q_SLOTS:
    void privateSendStatus();
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

Q_SIGNALS:
    void transactionClicked(const QModelIndex &index);
    void outOfSyncWarningClicked();

    void assetSendClicked(const QModelIndex &index);
    void assetIssueSubClicked(const QModelIndex &index);
    void assetIssueUniqueClicked(const QModelIndex &index);
    void assetReissueClicked(const QModelIndex &index);

private:
    QTimer *timer;
    Ui::OverviewPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    CAmount currentBalance;
    CAmount currentUnconfirmedBalance;
    CAmount currentImmatureBalance;
    CAmount currentAnonymizedBalance;
    CAmount currentWatchOnlyBalance;
    CAmount currentWatchUnconfBalance;
    CAmount currentWatchImmatureBalance;
    int nDisplayUnit;
    bool fShowAdvancedPSUI;
    int cachedNumISLocks;

    TxViewDelegate *txdelegate;
    std::unique_ptr<TransactionFilterProxy> filter;
    /** assets */
    std::unique_ptr<AssetFilterProxy> assetFilter;
    AssetViewDelegate *assetdelegate;
    QMenu *contextMenu;
    QAction *sendAction;
    QAction *issueSub;
    QAction *issueUnique;
    QAction *reissue;
    QAction *openURL;
    QAction *copyHashAction;
    /** assets end */

    void SetupTransactionList(int nNumItems);
    void DisablePrivateSendCompletely();

private Q_SLOTS:
    void togglePrivateSend();
    void updateDisplayUnit();
    void updatePrivateSendProgress();
    void updateAdvancedPSUI(bool fShowAdvancedPSUI);
    void handleTransactionClicked(const QModelIndex &index);
    void updateAlerts(const QString &warnings);
    void updateWatchOnlyLabels(bool showWatchOnly);
    void handleOutOfSyncWarningClicks();
    //assets
    void assetSearchChanged();
    void handleAssetRightClicked(const QModelIndex &index);
};

#endif // BITCOIN_QT_OVERVIEWPAGE_H
