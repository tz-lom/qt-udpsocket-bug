#include <QString>
#include <QtTest>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>

class tst_QUdpSocket : public QObject
{
    Q_OBJECT

public:
    tst_QUdpSocket();

private Q_SLOTS:
    void missedBroadcastHost();
    void empty_readyReadSlot();
};

tst_QUdpSocket::tst_QUdpSocket()
{
}

void tst_QUdpSocket::empty_readyReadSlot()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_QUdpSocket::missedBroadcastHost()
{
    QUdpSocket serverSocket;
    QVERIFY2(serverSocket.bind(QHostAddress(QHostAddress::AnyIPv4), 0), serverSocket.errorString().toLatin1().constData());
    quint16 port = serverSocket.localPort();

    QCOMPARE(serverSocket.state(), QUdpSocket::BoundState);

    connect(&serverSocket, SIGNAL(readyRead()), SLOT(empty_readyReadSlot()));

    QByteArray testMessage("Some test message");

    serverSocket.writeDatagram(testMessage, QHostAddress::Broadcast, port);

    QTestEventLoop::instance().enterLoop(15);

    QVERIFY(serverSocket.hasPendingDatagrams());

    //QNetworkDatagram dgram = serverSocket.receiveDatagram();  parsing datagram like this correctly works

    QByteArray datagram;
    datagram.resize(serverSocket.pendingDatagramSize());
    // datagram.resize(testMessage.size());  this method did not affect the result
    QHostAddress host;
    QCOMPARE(serverSocket.readDatagram(datagram.data(), datagram.size(), &host), testMessage.size());

    QCOMPARE(datagram, testMessage);
    QVERIFY(!host.isNull());
}

QTEST_MAIN(tst_QUdpSocket)
#include "tst_qudpsocket.moc"
