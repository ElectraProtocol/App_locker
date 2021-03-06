// Copyright (c) 2021 Electra protocol
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "applocker.h"
#include "ui_applocker.h"

AppLocker::AppLocker(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppLocker)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Wallet locker"));
    this->setWindowModality(Qt::ApplicationModal);
    QIntValidator *validatorInt = new QIntValidator(1, 999999999, this);

    //lock view (index 1)
    ui->stackedWidget->setCurrentIndex(1);
    ui->headLabel->setText(tr("Set a PIN code to lock your wallet:\n"));
    ui->messageLabel->setText(tr("\n- PIN code should be at least a 6 digit number.\n- PIN is only valid for this session"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Lock"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::Key_Enter);
    ui->pinLineEdit->setValidator(validatorInt);
    ui->pinLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmLineEdit->setValidator(validatorInt);
    ui->confirmLineEdit->setEchoMode(QLineEdit::Password);

    //unlock view
    ui->lockLabel->setText(tr("Your wallet is locked.\n"));
    ui->unlocklabel->setText(tr("PIN"));
    ui->unlockLineEdit->setValidator(validatorInt);
    ui->unlockLineEdit->setEchoMode(QLineEdit::Password);

    connect(ui->unlockLineEdit, &QLineEdit::textChanged, [this]{if(ui->unlockLineEdit->text().size() > 5) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
                                                                else if(ui->stackedWidget->currentIndex() == 0) ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);});
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AppLocker::setLock);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AppLocker::close);
}

AppLocker::~AppLocker()
{
    delete ui;
}

void AppLocker::setLock()
{
    switch (ui->stackedWidget->currentIndex()) {
    case 0:
        if(QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, ui->unlockLineEdit->text().toUtf8(), QString::number(salt).toLocal8Bit(), 10000, quint64(1000)) == hashLocker){
            walletLocked = false;
            hashLocker.clear();
            salt = 0;
            ui->stackedWidget->setCurrentIndex(1);
            ui->unlockLineEdit->clear();
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Lock"));
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
            ui->unlockLineEdit->clear();
            Q_EMIT lockingApp(false);
        }else{
            QMessageBox::warning(this, tr("Error"), tr("PIN code is not correct"), QMessageBox::Ok);
        }
        break;
    case 1:
        salt = QDateTime::currentMSecsSinceEpoch();
        if(ui->pinLineEdit->text().isEmpty() || ui->confirmLineEdit->text().isEmpty()){
            QMessageBox::information(this, tr("Empty field"), tr("Please enter and confirm your pin code"), QMessageBox::Ok);
            return;
        }else if(ui->pinLineEdit->text().size() < 6){
            QMessageBox::information(this, tr("Error"), tr("PIN code must be at least 6 digits long"), QMessageBox::Ok);
            return;
        }else if(ui->pinLineEdit->text() != ui->confirmLineEdit->text()){
            QMessageBox::warning(this, tr("Error"), tr("PIN code doesn't match, please check again"), QMessageBox::Ok);
            return;
        }else{
            walletLocked = true;
            hashLocker = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, ui->pinLineEdit->text().toUtf8(), QString::number(salt).toLocal8Bit(), 10000, quint64(1000));
            ui->pinLineEdit->clear();
            ui->confirmLineEdit->clear();
            ui->stackedWidget->setCurrentIndex(0); // move to unlock view
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Unlock"));
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(false);
            ui->unlockLineEdit->setFocus();
        }
        break;
    }
}

void AppLocker::showLocker()
{
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center()); // move the appLocker to the center of the screen
    this->show();
}

void AppLocker::closeEvent(QCloseEvent *event)
{
    if(walletLocked){
        int ret =  QMessageBox::warning(this, tr("WARNING"), tr("Wallet application will exit, continue?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel){
            event->ignore();
        }else{
            Q_EMIT quitAppFromWalletLocker();
            event->accept();
        }
    }else if(ui->stackedWidget->currentIndex() == 1){
        ui->unlockLineEdit->clear();
        ui->pinLineEdit->clear();
        ui->confirmLineEdit->clear();
        event->accept();
    }else{
        event->ignore();
    }
}
