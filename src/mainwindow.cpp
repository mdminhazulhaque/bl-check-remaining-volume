/*****************************************************************************
blion-network-reply, Check remaining usage without any browser
Copyright (C) 2012 MD: Minhazul Haque;
mdminhazulhaque@gmail.com.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager();

    // Banglalion
    req.setUrl(QUrl("http://care.banglalionwimax.com/radauth.php"));
    // Qubee
    //req.setUrl(QUrl("http://usage.qubee.com.bd/doLogin.jsp"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/x-www-form-urlencoded");
    connect (manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    ui->statusBar->showMessage("Ready to use ...");
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300)
        {
            QString replyText = reply->readAll();
            //qDebug() << replyText;
            int pos = replyText.indexOf("<TD>Your Volume Remaining:</TD>");
            QString toParse = replyText.mid(pos-12,70);
            QDomDocument doc;
            doc.setContent(toParse);
            QString data = doc.elementsByTagName("TD").at(1).childNodes().at(0).toElement().text();
            if(data == "")
                QMessageBox::critical(this, "Arrgh!", "User doesn't' exist or Wrong UserID/Password!");
            else
                QMessageBox::information(this, "Information", QString("Remaining volume: %1").arg(data));
            ui->statusBar->clearMessage();
        }
        else if (v >= 300 && v < 400)
        {
            ui->statusBar->showMessage("Server found! Parsing ...");
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            newUrl = reply->url().resolved(newUrl);
            QNetworkAccessManager *manager = reply->manager();
            QNetworkRequest redirection(newUrl);
            manager->get(redirection);
            return;
        }
    }
    else
        QMessageBox::critical(this, "Duh!", "There was an error!");
    return;
}

void MainWindow::on_check_clicked()
{
    if(ui->inuser->text() == "" or ui->inpasswd->text() == "")
    {
        QMessageBox::warning(this, "Oops!", "Please fill both input boxes!");
        return;
    }
    postData.clear();
    //postData.append("sUserName=" + ui->inuser->text() + "&sPwd=" + ui->inpasswd->text() + "&Submit=Login");
    // Banglalion
    postData.append("login=" + ui->inuser->text() + "&");
    postData.append("pass=" + ui->inpasswd->text());
    manager->post(req, postData);
    ui->statusBar->showMessage("Fetching data for " + ui->inuser->text());
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::information(this, "About me!", tr("Created by <b>MD Minhazul Haque © 2012</b> Licensed under GPLv3"));
}
