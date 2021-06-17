// Copyright (c) 2021 Electra protocol
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef APPLOCKER_H
#define APPLOCKER_H

#include <QCloseEvent>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QIntValidator>
#include <QMessageBox>
#include <QPasswordDigestor>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class AppLocker; }
QT_END_NAMESPACE

class AppLocker : public QWidget
{
    Q_OBJECT

public:
    AppLocker(QWidget *parent = nullptr);
    bool isWalletLocked(){return walletLocked;}
    ~AppLocker();

private:
    Ui::AppLocker *ui;
    qint64 salt = 0;
    QByteArray hashLocker;
    bool walletLocked = false;
    void setLock();

Q_SIGNALS:
    void quitAppFromWalletLocker();

public Q_SLOTS:
    void showLocker();

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // APPLOCKER_H
