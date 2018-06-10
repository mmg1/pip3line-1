/**
Released as open source by Gabriel Caudrelier

Developed by Gabriel Caudrelier, gabriel dot caudrelier at gmail dot com

https://github.com/metrodango/pip3line

Released under AGPL see LICENSE for more information
**/

#include "screeniodevice.h"
#include <QDebug>

ScreenIODevice::ScreenIODevice(QPlainTextEdit *outWidget, QObject *parent) :
    QIODevice(parent)
{
    widget = outWidget;
    connect(this, &ScreenIODevice::packet, this, &ScreenIODevice::processPacket);
}

bool ScreenIODevice::atEnd() const
{
    return widget == nullptr || pos() >= widget->toPlainText().toUtf8().size();
}

qint64 ScreenIODevice::bytesAvailable() const
{
    return widget != nullptr && widget->toPlainText().toUtf8().size() - pos();
}

qint64 ScreenIODevice::bytesToWrite() const
{
    return 0;
}

bool ScreenIODevice::canReadLine() const
{
    return widget != nullptr &&  (!atEnd() || QIODevice::canReadLine());
}

bool ScreenIODevice::seek(qint64 pos)
{
    if (widget == nullptr)
        return false;

    if (pos >= widget->toPlainText().toUtf8().size()) {
        return false;
    }
    return QIODevice::seek(pos);
}

qint64 ScreenIODevice::size() const
{
    if (widget == nullptr)
        return 0;
    else
        return widget->toPlainText().toUtf8().size();
}

bool ScreenIODevice::waitForBytesWritten(int /* Unused */)
{
    return widget != nullptr;
}

bool ScreenIODevice::waitForReadyRead(int /* Unused */)
{
    return widget != nullptr;
}

qint64 ScreenIODevice::readData(char *dest, qint64 maxSize)
{
    if (widget == nullptr)
        return -1;
    if ( maxSize < 1)
        return 0;

    int position = pos();
    int byteToRead = 0;
    if (position + maxSize > size())
        byteToRead = size() - position;
    else {
        byteToRead = maxSize;
    }
    QByteArray rdata = widget->toPlainText().toUtf8().mid(position,byteToRead);
    seek(position + byteToRead);

    memcpy(dest,rdata.constData(), byteToRead);

    return byteToRead;
}

qint64 ScreenIODevice::writeData(const char *src, qint64 maxSize)
{
    if (widget == nullptr)
        return -1;


    if ( maxSize < 1)
        return 0;

    QByteArray rdata(src,maxSize);
    emit packet(QString::fromUtf8(rdata));

    return maxSize;
}

void ScreenIODevice::setWidget(QPlainTextEdit *outWidget)
{
    widget = outWidget;
}

void ScreenIODevice::processPacket(QString packet)
{
    widget->insertPlainText(packet);
}

