// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2020 The Neoxa developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "utilitydialog.h"
#include "walletmodel.h"
#include "validation.h"
#include "assetfilterproxy.h"
#include "assettablemodel.h"

#include "smartnode/smartnode-sync.h"
#include "privatesend/privatesend-client.h"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QDesktopServices>
#include <QSettings>
#include <QTimer>
#include <QMouseEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#define QTversionPreFiveEleven
#endif

#define ICON_OFFSET 16
#define DECORATION_SIZE 54
#define NUM_ITEMS 5
#define NUM_ITEMS_ADV 7

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(const PlatformStyle *_platformStyle, QObject *parent=nullptr):
        QAbstractItemDelegate(), unit(BitcoinUnits::NEOX),
        platformStyle(_platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
        QRect mainRect = option.rect;
        mainRect.moveLeft(ICON_OFFSET);
        QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 8;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace - ICON_OFFSET, halfheight);
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon = platformStyle->SingleColorIcon(icon);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(value.canConvert<QBrush>())
        {
            QBrush brush = qvariant_cast<QBrush>(value);
            foreground = brush.color();
        }

        painter->setPen(foreground);
        QRect boundingRect;
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address, &boundingRect);

        if (index.data(TransactionTableModel::WatchonlyRole).toBool())
        {
            QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
            QRect watchonlyRect(boundingRect.right() + 5, mainRect.top()+ypad+halfheight, 16, halfheight);
            iconWatchonly.paint(painter, watchonlyRect);
        }

        if(amount < 0)
        {
            foreground = GUIUtil::getThemedQColor(GUIUtil::ThemedColor::NEGATIVE);
        }
        else if(!confirmed)
        {
            foreground = GUIUtil::getThemedQColor(GUIUtil::ThemedColor::UNCONFIRMED);
        }
        else
        {
            foreground = option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QString amountText = BitcoinUnits::floorWithUnit(unit, amount, true, BitcoinUnits::separatorAlways);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;
    const PlatformStyle *platformStyle;

};

class AssetViewDelegate : public QAbstractItemDelegate
{
Q_OBJECT
public:
    explicit AssetViewDelegate(const PlatformStyle *_platformStyle, QObject *parent=nullptr):
            QAbstractItemDelegate(parent), unit(BitcoinUnits::NEOX),
            platformStyle(_platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        /** Get the icon for the administrator of the asset */
        QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
        QPixmap ipfspixmap = qvariant_cast<QPixmap>(index.data(AssetTableModel::AssetIPFSHashDecorationRole));

        bool admin = index.data(AssetTableModel::AdministratorRole).toBool();

        /** Need to know the heigh to the pixmap. If it is 0 we don't we dont own this asset so dont have room for the icon */
        int nIconSize = admin ? pixmap.height() : 0;
        int nIPFSIconSize = ipfspixmap.height();
        int extraNameSpacing = 12;
        if (nIconSize)
            extraNameSpacing = 0;

        /** Get basic padding and half height */
        QRect mainRect = option.rect;
        int xspace = nIconSize + 32;
        int ypad = 2;

        // Create the gradient rect to draw the gradient over
        QRect gradientRect = mainRect;
        gradientRect.setTop(gradientRect.top() + 2);
        gradientRect.setBottom(gradientRect.bottom() - 2);
        gradientRect.setRight(gradientRect.right() - 20);

        int halfheight = (gradientRect.height() - 2*ypad)/2;

        /** Create the three main rectangles  (Icon, Name, Amount) */
        QRect assetAdministratorRect(QPoint(20, gradientRect.top() + halfheight/2 - 3*ypad), QSize(nIconSize, nIconSize));
        QRect assetNameRect(gradientRect.left() + xspace - extraNameSpacing, gradientRect.top()+ypad+(halfheight/2), gradientRect.width() - xspace, halfheight + ypad);
        QRect amountRect(gradientRect.left() + xspace, gradientRect.top()+ypad+(halfheight/2), gradientRect.width() - xspace - 24, halfheight);
        QRect ipfsLinkRect(QPoint(gradientRect.right() - nIconSize/2, gradientRect.top() + halfheight/1.5), QSize(nIconSize/2, nIconSize/2));

        // Create the gradient for the asset items
        QLinearGradient gradient(mainRect.topLeft(), mainRect.bottomRight());

        // Select the color of the gradient
        if (admin) {
            /*if (darkModeEnabled) {
                gradient.setColorAt(0, COLOR_ADMIN_CARD_DARK);
                gradient.setColorAt(1, COLOR_ADMIN_CARD_DARK);
            } else {*/
                gradient.setColorAt(0, QColor("#25301f"));
                gradient.setColorAt(1, QColor("#161b13"));
            //}
        } else {
            /*if (darkModeEnabled) {
                gradient.setColorAt(0, COLOR_REGULAR_CARD_LIGHT_BLUE_DARK_MODE);
                gradient.setColorAt(1, COLOR_REGULAR_CARD_DARK_BLUE_DARK_MODE);
            } else {*/
                gradient.setColorAt(0, QColor("#2f3f27"));
                gradient.setColorAt(1, QColor("#25301f"));
            //}
        }

        // Using 4 are the radius because the pixels are solid
        QPainterPath path;
        path.addRoundedRect(gradientRect, 4, 4);

        // Paint the gradient
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillPath(path, gradient);

        /** Draw asset administrator icon */
        if (nIconSize)
            painter->drawPixmap(assetAdministratorRect, pixmap);

        if (nIPFSIconSize)
            painter->drawPixmap(ipfsLinkRect, ipfspixmap);

        /** Create the font that is used for painting the asset name */
        QFont nameFont;
#if !defined(Q_OS_MAC)
        nameFont.setFamily("Open Sans");
#endif
        nameFont.setPixelSize(14);
        nameFont.setWeight(QFont::Weight::Normal);
        nameFont.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, -0.4);

        /** Create the font that is used for painting the asset amount */
        QFont amountFont;
#if !defined(Q_OS_MAC)
        amountFont.setFamily("Open Sans");
#endif
        amountFont.setPixelSize(14);
        amountFont.setWeight(QFont::Weight::Normal);
        amountFont.setLetterSpacing(QFont::SpacingType::AbsoluteSpacing, -0.3);

        /** Get the name and formatted amount from the data */
        QString name = index.data(AssetTableModel::AssetNameRole).toString();
        QString amountText = index.data(AssetTableModel::FormattedAmountRole).toString();
        
        // Setup the pens
        QColor textColor = QColor(255, 255, 255);
        //if (darkModeEnabled)
        //    textColor = COLOR_TOOLBAR_SELECTED_TEXT_DARK_MODE;

        QPen penName(textColor);

        /** Start Concatenation of Asset Name */
        // Get the width in pixels that the amount takes up (because they are different font,
        // we need to do this before we call the concatenate function
        painter->setFont(amountFont);
        //#ifndef QTversionPreFiveEleven
        //	int amount_width = painter->fontMetrics().horizontalAdvance(amountText);
		//#else
			int amount_width = painter->fontMetrics().width(amountText);
		//#endif
        // Set the painter for the font used for the asset name, so that the concatenate function estimated width correctly
        painter->setFont(nameFont);

        GUIUtil::concatenate(painter, name, amount_width, assetNameRect.left(), amountRect.right());

        /** Paint the asset name */
        painter->setPen(penName);
        painter->drawText(assetNameRect, Qt::AlignLeft|Qt::AlignVCenter, name);

        /** Paint the amount */
        painter->setFont(amountFont);
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(42, 42);
    }

    int unit;
    const PlatformStyle *platformStyle;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    timer(nullptr),
    ui(new Ui::OverviewPage),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    currentWatchOnlyBalance(-1),
    currentWatchUnconfBalance(-1),
    currentWatchImmatureBalance(-1),
    cachedNumISLocks(-1),
    txdelegate(new TxViewDelegate(platformStyle, this)),
    assetdelegate(new AssetViewDelegate(platformStyle, this))
{
    ui->setupUi(this);

    // Recent transactions
    ui->listTransactions->setItemDelegate(txdelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    // Note: minimum height of listTransactions will be set later in updateAdvancedPSUI() to reflect actual settings
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));

    // init "out of sync" warning labels
    ui->labelWalletStatus->setText("(" + tr("out of sync") + ")");
    ui->labelPrivateSendSyncStatus->setText("(" + tr("out of sync") + ")");
    ui->labelTransactionsStatus->setText("(" + tr("out of sync") + ")");
    ui->labelAssetStatus->setText("(" + tr("out of sync") + ")");

    // hide PS frame (helps to preserve saved size)
    // we'll setup and make it visible in updateAdvancedPSUI() later if we are not in litemode
    ui->framePrivateSend->setVisible(false);

    /** assets */
    /** Create the list of assets */
    ui->listAssets->setItemDelegate(assetdelegate);
    ui->listAssets->setIconSize(QSize(42, 42));
    ui->listAssets->setMinimumHeight(5 * (42 + 2));

    // Delay before filtering assetes in ms
    static const int input_filter_delay = 200;

    QTimer *asset_typing_delay;
    asset_typing_delay = new QTimer(this);
    asset_typing_delay->setSingleShot(true);
    asset_typing_delay->setInterval(input_filter_delay);
    connect(ui->assetSearch, SIGNAL(textChanged(QString)), asset_typing_delay, SLOT(start()));
    connect(asset_typing_delay, SIGNAL(timeout()), this, SLOT(assetSearchChanged()));

    ui->listAssets->viewport()->installEventFilter(this);
    /** Create the search bar for assets */
    ui->assetSearch->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->assetSearch->setAlignment(Qt::AlignVCenter);
    QFont font = ui->assetSearch->font();
    font.setPointSize(12);
    ui->assetSearch->setFont(font);

    QFontMetrics fm = QFontMetrics(ui->assetSearch->font());
    ui->assetSearch->setFixedHeight(fm.height()+ 5);

    // Trigger the call to show the assets table if assets are active
    showAssets();

    // context menu actions
    sendAction = new QAction(tr("Send Asset"), this);
    QAction *copyAmountAction = new QAction(tr("Copy Amount"), this);
    QAction *copyNameAction = new QAction(tr("Copy Name"), this);
    copyHashAction = new QAction(tr("Copy Hash"), this);
    issueSub = new QAction(tr("Issue Sub Asset"), this);
    issueUnique = new QAction(tr("Issue Unique Asset"), this);
    reissue = new QAction(tr("Reissue Asset"), this);
    openURL = new QAction(tr("Open IPFS in Browser"), this);


    sendAction->setObjectName("Send");
    issueSub->setObjectName("Sub");
    issueUnique->setObjectName("Unique");
    reissue->setObjectName("Reissue");
    copyNameAction->setObjectName("Copy Name");
    copyAmountAction->setObjectName("Copy Amount");
    copyHashAction->setObjectName("Copy Hash");
    openURL->setObjectName("Browse");

    // context menu
    contextMenu = new QMenu(this);
    contextMenu->addAction(sendAction);
    contextMenu->addAction(issueSub);
    contextMenu->addAction(issueUnique);
    contextMenu->addAction(reissue);
    contextMenu->addSeparator();
    contextMenu->addAction(openURL);
    contextMenu->addAction(copyHashAction);
    contextMenu->addSeparator();
    contextMenu->addAction(copyNameAction);
    contextMenu->addAction(copyAmountAction);
    // context menu signals
    /** assets end */

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);

    if(!privateSendClient.fEnablePrivateSend) return;

    // Disable any PS UI for smartnode or when autobackup is disabled or failed for whatever reason
    if(fSmartnodeMode || nWalletBackups <= 0){
        DisablePrivateSendCompletely();
        if (nWalletBackups <= 0) {
            ui->labelPrivateSendEnabled->setToolTip(tr("Automatic backups are disabled, no mixing available!"));
        }
    } else {
        if(!privateSendClient.fPrivateSendRunning){
            ui->togglePrivateSend->setText(tr("Start Mixing"));
        } else {
            ui->togglePrivateSend->setText(tr("Stop Mixing"));
        }
        // Disable privateSendClient builtin support for automatic backups while we are in GUI,
        // we'll handle automatic backups and user warnings in privateSendStatus()
        privateSendClient.fCreateAutoBackups = false;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(privateSendStatus()));
        timer->start(1000);
    }
}

bool OverviewPage::eventFilter(QObject *object, QEvent *event)
{
    // If the asset viewport is being clicked
    if (object == ui->listAssets->viewport() && event->type() == QEvent::MouseButtonPress) {

        // Grab the mouse event
        QMouseEvent * mouseEv = static_cast<QMouseEvent*>(event);

        // Select the current index at the mouse location
        QModelIndex currentIndex = ui->listAssets->indexAt(mouseEv->pos());

        // Open the menu on right click, direct url on left click
        if (mouseEv->buttons() & Qt::RightButton ) {
            handleAssetRightClicked(currentIndex);
        } else if (mouseEv->buttons() & Qt::LeftButton) {
            openIPFSForAsset(currentIndex);
        }
    }

    return QWidget::eventFilter(object, event);
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        Q_EMIT transactionClicked(filter->mapToSource(index));
}

void OverviewPage::handleAssetRightClicked(const QModelIndex &index)
{
    if(assetFilter) {
        // Grab the data elements from the index that we need to disable and enable menu items
        QString name = index.data(AssetTableModel::AssetNameRole).toString();
        QString ipfshash = index.data(AssetTableModel::AssetIPFSHashRole).toString();

        if (IsAssetNameAnOwner(name.toStdString())) {
            name = name.left(name.size() - 1);
            sendAction->setDisabled(true);
        } else {
            sendAction->setDisabled(false);
        }

        // If the ipfs hash isn't there or doesn't start with Qm, disable the action item
        if (ipfshash.count() > 0 && ipfshash.indexOf("Qm") == 0) {
            openURL->setDisabled(false);
        } else {
            openURL->setDisabled(true);
        }

        if (ipfshash.count() > 0) {
            copyHashAction->setDisabled(false);
        } else {
            copyHashAction->setDisabled(true);
        }

        if (!index.data(AssetTableModel::AdministratorRole).toBool()) {
            issueSub->setDisabled(true);
            issueUnique->setDisabled(true);
            reissue->setDisabled(true);
        } else {
            issueSub->setDisabled(false);
            issueUnique->setDisabled(false);
            reissue->setDisabled(true);
            CNewAsset asset;
            auto currentActiveAssetCache = GetCurrentAssetCache();
            if (currentActiveAssetCache && currentActiveAssetCache->GetAssetMetaDataIfExists(name.toStdString(), asset))
                if (asset.nReissuable)
                    reissue->setDisabled(false);

        }

        QAction* action = contextMenu->exec(QCursor::pos());

        if (action) {
            if (action->objectName() == "Send")
                Q_EMIT assetSendClicked(assetFilter->mapToSource(index));
            else if (action->objectName() == "Sub")
                Q_EMIT assetIssueSubClicked(assetFilter->mapToSource(index));
            else if (action->objectName() == "Unique")
                Q_EMIT assetIssueUniqueClicked(assetFilter->mapToSource(index));
            else if (action->objectName() == "Reissue")
                Q_EMIT assetReissueClicked(assetFilter->mapToSource(index));
            else if (action->objectName() == "Copy Name")
                GUIUtil::setClipboard(index.data(AssetTableModel::AssetNameRole).toString());
            else if (action->objectName() == "Copy Amount")
                GUIUtil::setClipboard(index.data(AssetTableModel::FormattedAmountRole).toString());
            else if (action->objectName() == "Copy Hash")
                GUIUtil::setClipboard(ipfshash);
            else if (action->objectName() == "Browse") {
                QDesktopServices::openUrl(QUrl::fromUserInput("https://cloudflare-ipfs.com/ipfs/" + ipfshash));
            }
        }
    }
}

void OverviewPage::handleOutOfSyncWarningClicks()
{
    Q_EMIT outOfSyncWarningClicked();
}

OverviewPage::~OverviewPage()
{
    if(timer) disconnect(timer, SIGNAL(timeout()), this, SLOT(privateSendStatus()));
    delete ui;
}

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    currentAnonymizedBalance = anonymizedBalance;
    currentWatchOnlyBalance = watchOnlyBalance;
    currentWatchUnconfBalance = watchUnconfBalance;
    currentWatchImmatureBalance = watchImmatureBalance;
    ui->labelBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance, false, BitcoinUnits::separatorAlways));
    ui->labelUnconfirmed->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, unconfirmedBalance, false, BitcoinUnits::separatorAlways));
    ui->labelImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, immatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelAnonymized->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, anonymizedBalance, false, BitcoinUnits::separatorAlways));
    ui->labelTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance + unconfirmedBalance + immatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchAvailable->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchPending->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchUnconfBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchImmatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcoinUnits::separatorAlways));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    bool showImmature = immatureBalance != 0;
    bool showWatchOnlyImmature = watchImmatureBalance != 0;

    // for symmetry reasons also show immature label when the watch-only one is shown
    ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance

    updatePrivateSendProgress();

    if (walletModel) {
        int numISLocks = walletModel->getNumISLocks();
        if(cachedNumISLocks != numISLocks) {
            cachedNumISLocks = numISLocks;
            ui->listTransactions->update();
        }
    }
}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
    ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
    ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
    ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
    ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
    ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
    ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance

    if (!showWatchOnly){
        ui->labelWatchImmature->hide();
    }
    else{
        ui->labelBalance->setIndent(20);
        ui->labelUnconfirmed->setIndent(20);
        ui->labelImmature->setIndent(20);
        ui->labelTotal->setIndent(20);
    }
}

void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // update the display unit, to not use the default ("NEOX")
        updateDisplayUnit();
        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(), model->getAnonymizedBalance(),
                   model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
        updateWatchOnlyLabels(model->haveWatchOnly());
        connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));

        // explicitly update PS frame and transaction list to reflect actual settings
        updateAdvancedPSUI(model->getOptionsModel()->getShowAdvancedPSUI());

        if(!privateSendClient.fEnablePrivateSend) return;

        connect(model->getOptionsModel(), SIGNAL(privateSendRoundsChanged()), this, SLOT(updatePrivateSendProgress()));
        connect(model->getOptionsModel(), SIGNAL(privateSentAmountChanged()), this, SLOT(updatePrivateSendProgress()));
        connect(model->getOptionsModel(), SIGNAL(advancedPSUIChanged(bool)), this, SLOT(updateAdvancedPSUI(bool)));

        connect(ui->togglePrivateSend, SIGNAL(clicked()), this, SLOT(togglePrivateSend()));

        // privatesend buttons will not react to spacebar must be clicked on
        ui->togglePrivateSend->setFocusPolicy(Qt::NoFocus);
    }
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance, currentAnonymizedBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = nDisplayUnit;

        ui->listTransactions->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    ui->labelWalletStatus->setVisible(fShow);
    ui->labelPrivateSendSyncStatus->setVisible(fShow);
    ui->labelTransactionsStatus->setVisible(fShow);
    if (AreAssetsDeployed()) {
        ui->labelAssetStatus->setVisible(fShow);
    }
}

void OverviewPage::updatePrivateSendProgress()
{
    if(!smartnodeSync.IsBlockchainSynced() || ShutdownRequested()) return;

    if(vpwallets.empty()) return;

    QString strAmountAndRounds;
    QString strPrivateSendAmount = BitcoinUnits::formatHtmlWithUnit(nDisplayUnit, privateSendClient.nPrivateSendAmount * COIN, false, BitcoinUnits::separatorAlways);

    if(currentBalance == 0)
    {
        ui->privateSendProgress->setValue(0);
        ui->privateSendProgress->setToolTip(tr("No inputs detected"));

        // when balance is zero just show info from settings
        strPrivateSendAmount = strPrivateSendAmount.remove(strPrivateSendAmount.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = strPrivateSendAmount + " / " + tr("%n Rounds", "", privateSendClient.nPrivateSendRounds);

        ui->labelAmountRounds->setToolTip(tr("No inputs detected"));
        ui->labelAmountRounds->setText(strAmountAndRounds);
        return;
    }

    CAmount nAnonymizableBalance = vpwallets[0]->GetAnonymizableBalance(false, false);

    CAmount nMaxToAnonymize = nAnonymizableBalance + currentAnonymizedBalance;

    // If it's more than the anon threshold, limit to that.
    if(nMaxToAnonymize > privateSendClient.nPrivateSendAmount*COIN) nMaxToAnonymize = privateSendClient.nPrivateSendAmount*COIN;

    if(nMaxToAnonymize == 0) return;

    if(nMaxToAnonymize >= privateSendClient.nPrivateSendAmount * COIN) {
        ui->labelAmountRounds->setToolTip(tr("Found enough compatible inputs to anonymize %1")
                                          .arg(strPrivateSendAmount));
        strPrivateSendAmount = strPrivateSendAmount.remove(strPrivateSendAmount.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = strPrivateSendAmount + " / " + tr("%n Rounds", "", privateSendClient.nPrivateSendRounds);
    } else {
        QString strMaxToAnonymize = BitcoinUnits::formatHtmlWithUnit(nDisplayUnit, nMaxToAnonymize, false, BitcoinUnits::separatorAlways);
        ui->labelAmountRounds->setToolTip(tr("Not enough compatible inputs to anonymize <span style='color:red;'>%1</span>,<br>"
                                             "will anonymize <span style='color:red;'>%2</span> instead")
                                          .arg(strPrivateSendAmount)
                                          .arg(strMaxToAnonymize));
        strMaxToAnonymize = strMaxToAnonymize.remove(strMaxToAnonymize.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = "<span style='" + GUIUtil::getThemedStyleQString(GUIUtil::ThemedStyle::TS_ERROR) + "'>" +
                QString(BitcoinUnits::factor(nDisplayUnit) == 1 ? "" : "~") + strMaxToAnonymize +
                " / " + tr("%n Rounds", "", privateSendClient.nPrivateSendRounds) + "</span>";
    }
    ui->labelAmountRounds->setText(strAmountAndRounds);

    if (!fShowAdvancedPSUI) return;

    CAmount nDenominatedConfirmedBalance;
    CAmount nDenominatedUnconfirmedBalance;
    CAmount nNormalizedAnonymizedBalance;
    float nAverageAnonymizedRounds;

    nDenominatedConfirmedBalance = vpwallets[0]->GetDenominatedBalance();
    nDenominatedUnconfirmedBalance = vpwallets[0]->GetDenominatedBalance(true);
    nNormalizedAnonymizedBalance = vpwallets[0]->GetNormalizedAnonymizedBalance();
    nAverageAnonymizedRounds = vpwallets[0]->GetAverageAnonymizedRounds();

    // calculate parts of the progress, each of them shouldn't be higher than 1
    // progress of denominating
    float denomPart = 0;
    // mixing progress of denominated balance
    float anonNormPart = 0;
    // completeness of full amount anonymization
    float anonFullPart = 0;

    CAmount denominatedBalance = nDenominatedConfirmedBalance + nDenominatedUnconfirmedBalance;
    denomPart = (float)denominatedBalance / nMaxToAnonymize;
    denomPart = denomPart > 1 ? 1 : denomPart;
    denomPart *= 100;

    anonNormPart = (float)nNormalizedAnonymizedBalance / nMaxToAnonymize;
    anonNormPart = anonNormPart > 1 ? 1 : anonNormPart;
    anonNormPart *= 100;

    anonFullPart = (float)currentAnonymizedBalance / nMaxToAnonymize;
    anonFullPart = anonFullPart > 1 ? 1 : anonFullPart;
    anonFullPart *= 100;

    // apply some weights to them ...
    float denomWeight = 1;
    float anonNormWeight = privateSendClient.nPrivateSendRounds;
    float anonFullWeight = 2;
    float fullWeight = denomWeight + anonNormWeight + anonFullWeight;
    // ... and calculate the whole progress
    float denomPartCalc = ceilf((denomPart * denomWeight / fullWeight) * 100) / 100;
    float anonNormPartCalc = ceilf((anonNormPart * anonNormWeight / fullWeight) * 100) / 100;
    float anonFullPartCalc = ceilf((anonFullPart * anonFullWeight / fullWeight) * 100) / 100;
    float progress = denomPartCalc + anonNormPartCalc + anonFullPartCalc;
    if(progress >= 100) progress = 100;

    ui->privateSendProgress->setValue(progress);

    QString strToolPip = ("<b>" + tr("Overall progress") + ": %1%</b><br/>" +
                          tr("Denominated") + ": %2%<br/>" +
                          tr("Partially mixed") + ": %3%<br/>" +
                          tr("Mixed") + ": %4%<br/>" +
                          tr("Denominated inputs have %5 of %n rounds on average", "", privateSendClient.nPrivateSendRounds))
            .arg(progress).arg(denomPart).arg(anonNormPart).arg(anonFullPart)
            .arg(nAverageAnonymizedRounds);
    ui->privateSendProgress->setToolTip(strToolPip);
}

void OverviewPage::updateAdvancedPSUI(bool fShowAdvancedPSUI) {
    this->fShowAdvancedPSUI = fShowAdvancedPSUI;
    int nNumItems = (!privateSendClient.fEnablePrivateSend || !fShowAdvancedPSUI) ? NUM_ITEMS : NUM_ITEMS_ADV;
    SetupTransactionList(nNumItems);

    if (!privateSendClient.fEnablePrivateSend) return;

    ui->framePrivateSend->setVisible(true);
    ui->labelCompletitionText->setVisible(fShowAdvancedPSUI);
    ui->privateSendProgress->setVisible(fShowAdvancedPSUI);
    ui->labelSubmittedDenomText->setVisible(fShowAdvancedPSUI);
    ui->labelSubmittedDenom->setVisible(fShowAdvancedPSUI);
    ui->labelPrivateSendLastMessage->setVisible(fShowAdvancedPSUI);
}

void OverviewPage::privateSendStatus()
{
    if(!smartnodeSync.IsBlockchainSynced() || ShutdownRequested()) return;

    static int64_t nLastDSProgressBlockTime = 0;
    int nBestHeight = clientModel->getNumBlocks();

    // We are processing more then 1 block per second, we'll just leave
    if(((nBestHeight - privateSendClient.nCachedNumBlocks) / (GetTimeMillis() - nLastDSProgressBlockTime + 1) > 1)) return;
    nLastDSProgressBlockTime = GetTimeMillis();

    QString strKeysLeftText(tr("keys left: %1").arg(vpwallets[0]->nKeysLeftSinceAutoBackup));
    if(vpwallets[0]->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING) {
        strKeysLeftText = "<span style='" + GUIUtil::getThemedStyleQString(GUIUtil::ThemedStyle::TS_ERROR) + "'>" + strKeysLeftText + "</span>";
    }
    ui->labelPrivateSendEnabled->setToolTip(strKeysLeftText);

    if (!privateSendClient.fPrivateSendRunning) {
        if (nBestHeight != privateSendClient.nCachedNumBlocks) {
            privateSendClient.nCachedNumBlocks = nBestHeight;
            updatePrivateSendProgress();
        }

        ui->labelPrivateSendLastMessage->setText("");
        ui->togglePrivateSend->setText(tr("Start Mixing"));

        QString strEnabled = tr("Disabled");
        // Show how many keys left in advanced PS UI mode only
        if (fShowAdvancedPSUI) strEnabled += ", " + strKeysLeftText;
        ui->labelPrivateSendEnabled->setText(strEnabled);

        return;
    }

    // Warn user that wallet is running out of keys
    // NOTE: we do NOT warn user and do NOT create autobackups if mixing is not running
    if (nWalletBackups > 0 && vpwallets[0]->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING) {
        QSettings settings;
        if(settings.value("fLowKeysWarning").toBool()) {
            QString strWarn =   tr("Very low number of keys left since last automatic backup!") + "<br><br>" +
                                tr("We are about to create a new automatic backup for you, however "
                                   "<span style='color:red;'> you should always make sure you have backups "
                                   "saved in some safe place</span>!") + "<br><br>" +
                                tr("Note: You can turn this message off in options.");
            ui->labelPrivateSendEnabled->setToolTip(strWarn);
            LogPrint(BCLog::PRIVATESEND, "OverviewPage::privateSendStatus -- Very low number of keys left since last automatic backup, warning user and trying to create new backup...\n");
            QMessageBox::warning(this, tr("PrivateSend"), strWarn, QMessageBox::Ok, QMessageBox::Ok);
        } else {
            LogPrint(BCLog::PRIVATESEND, "OverviewPage::privateSendStatus -- Very low number of keys left since last automatic backup, skipping warning and trying to create new backup...\n");
        }

        std::string strBackupWarning;
        std::string strBackupError;
        if(!AutoBackupWallet(vpwallets[0], "", strBackupWarning, strBackupError)) {
            if (!strBackupWarning.empty()) {
                // It's still more or less safe to continue but warn user anyway
                LogPrint(BCLog::PRIVATESEND, "OverviewPage::privateSendStatus -- WARNING! Something went wrong on automatic backup: %s\n", strBackupWarning);

                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("WARNING! Something went wrong on automatic backup") + ":<br><br>" + strBackupWarning.c_str(),
                    QMessageBox::Ok, QMessageBox::Ok);
            }
            if (!strBackupError.empty()) {
                // Things are really broken, warn user and stop mixing immediately
                LogPrint(BCLog::PRIVATESEND, "OverviewPage::privateSendStatus -- ERROR! Failed to create automatic backup: %s\n", strBackupError);

                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("ERROR! Failed to create automatic backup") + ":<br><br>" + strBackupError.c_str() + "<br>" +
                    tr("Mixing is disabled, please close your wallet and fix the issue!"),
                    QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }

    QString strEnabled = privateSendClient.fPrivateSendRunning ? tr("Enabled") : tr("Disabled");
    // Show how many keys left in advanced PS UI mode only
    if(fShowAdvancedPSUI) strEnabled += ", " + strKeysLeftText;
    ui->labelPrivateSendEnabled->setText(strEnabled);

    if(nWalletBackups == -1) {
        // Automatic backup failed, nothing else we can do until user fixes the issue manually
        DisablePrivateSendCompletely();

        QString strError =  tr("ERROR! Failed to create automatic backup") + ", " +
                            tr("see debug.log for details.") + "<br><br>" +
                            tr("Mixing is disabled, please close your wallet and fix the issue!");
        ui->labelPrivateSendEnabled->setToolTip(strError);

        return;
    } else if(nWalletBackups == -2) {
        // We were able to create automatic backup but keypool was not replenished because wallet is locked.
        QString strWarning = tr("WARNING! Failed to replenish keypool, please unlock your wallet to do so.");
        ui->labelPrivateSendEnabled->setToolTip(strWarning);
    }

    // check privatesend status and unlock if needed
    if(nBestHeight != privateSendClient.nCachedNumBlocks) {
        // Balance and number of transactions might have changed
        privateSendClient.nCachedNumBlocks = nBestHeight;
        updatePrivateSendProgress();
    }

    QString strStatus = QString(privateSendClient.GetStatuses().c_str());

    QString s = tr("Last PrivateSend message:\n") + strStatus;

    if(s != ui->labelPrivateSendLastMessage->text())
        LogPrint(BCLog::PRIVATESEND, "OverviewPage::privateSendStatus -- Last PrivateSend message: %s\n", strStatus.toStdString());

    ui->labelPrivateSendLastMessage->setText(s);

    ui->labelSubmittedDenom->setText(QString(privateSendClient.GetSessionDenoms().c_str()));
}

void OverviewPage::togglePrivateSend(){
    QSettings settings;
    // Popup some information on first mixing
    QString hasMixed = settings.value("hasMixed").toString();
    if(hasMixed.isEmpty()){
        QMessageBox::information(this, tr("PrivateSend"),
                tr("If you don't want to see internal PrivateSend fees/transactions select \"Most Common\" as Type on the \"Transactions\" tab."),
                QMessageBox::Ok, QMessageBox::Ok);
        settings.setValue("hasMixed", "hasMixed");
    }
    if(!privateSendClient.fPrivateSendRunning){
        const CAmount nMinAmount = CPrivateSend::GetSmallestDenomination() + CPrivateSend::GetMaxCollateralAmount();
        if(currentBalance < nMinAmount){
            QString strMinAmount(BitcoinUnits::formatWithUnit(nDisplayUnit, nMinAmount));
            QMessageBox::warning(this, tr("PrivateSend"),
                tr("PrivateSend requires at least %1 to use.").arg(strMinAmount),
                QMessageBox::Ok, QMessageBox::Ok);
            return;
        }

        // if wallet is locked, ask for a passphrase
        if (walletModel->getEncryptionStatus() == WalletModel::Locked)
        {
            WalletModel::UnlockContext ctx(walletModel->requestUnlock(true));
            if(!ctx.isValid())
            {
                //unlock was cancelled
                privateSendClient.nCachedNumBlocks = std::numeric_limits<int>::max();
                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("Wallet is locked and user declined to unlock. Disabling PrivateSend."),
                    QMessageBox::Ok, QMessageBox::Ok);
                LogPrint(BCLog::PRIVATESEND, "OverviewPage::togglePrivateSend -- Wallet is locked and user declined to unlock. Disabling PrivateSend.\n");
                return;
            }
        }

    }

    privateSendClient.fPrivateSendRunning = !privateSendClient.fPrivateSendRunning;
    privateSendClient.nCachedNumBlocks = std::numeric_limits<int>::max();

    if(!privateSendClient.fPrivateSendRunning){
        ui->togglePrivateSend->setText(tr("Start Mixing"));
        privateSendClient.ResetPool();
    } else {
        ui->togglePrivateSend->setText(tr("Stop Mixing"));
    }
}

void OverviewPage::SetupTransactionList(int nNumItems) {
    ui->listTransactions->setMinimumHeight(nNumItems * (DECORATION_SIZE + 2));

    if(walletModel && walletModel->getOptionsModel()) {
        // Set up transaction list
        filter.reset(new TransactionFilterProxy());
        filter->setSourceModel(walletModel->getTransactionTableModel());
        filter->setLimit(nNumItems);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);

        ui->listTransactions->setModel(filter.get());
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        assetFilter.reset(new AssetFilterProxy());
        assetFilter->setSourceModel(walletModel->getAssetTableModel());
        assetFilter->sort(AssetTableModel::AssetNameRole, Qt::DescendingOrder);
        ui->listAssets->setModel(assetFilter.get());
        ui->listAssets->setAutoFillBackground(false);
    }
}

void OverviewPage::DisablePrivateSendCompletely() {
    ui->togglePrivateSend->setText("(" + tr("Disabled") + ")");
    ui->framePrivateSend->setEnabled(false);
    if (nWalletBackups <= 0) {
        ui->labelPrivateSendEnabled->setText("<span style='" + GUIUtil::getThemedStyleQString(GUIUtil::ThemedStyle::TS_ERROR) + "'>(" + tr("Disabled") + ")</span>");
    }
    privateSendClient.fPrivateSendRunning = false;
}

void OverviewPage::showAssets()
{
    if (AreAssetsDeployed()) {
        ui->assetFrame->show();
    } else {
        ui->assetFrame->hide();
    }
}

void OverviewPage::assetSearchChanged()
{
    if (!assetFilter)
        return;
    assetFilter->setAssetNameContains(ui->assetSearch->text());
}

void OverviewPage::openIPFSForAsset(const QModelIndex &index)
{
    // Get the ipfs hash of the asset clicked
    QString ipfshash = index.data(AssetTableModel::AssetIPFSHashRole).toString();

    // If the ipfs hash isn't there or doesn't start with Qm, disable the action item
    if (ipfshash.count() > 0 && ipfshash.indexOf("Qm") == 0) {
        QDesktopServices::openUrl(QUrl::fromUserInput("https://cloudflare-ipfs.com/ipfs/" + ipfshash));
    }
}
