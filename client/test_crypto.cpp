#include <QtTest>
#include "crypto.h"

class TestCrypto : public QObject {
    Q_OBJECT

private slots:
    void testEncryptionDecryption();
    void testEmptyString();
};

void TestCrypto::testEncryptionDecryption() {
    QByteArray originalText = "Привет, это сообщение для проверки шифрования";
    QByteArray key = "КлючШифрования123";

    QByteArray encryptedText = Crypto::encryptDecrypt(originalText, key);

    QVERIFY(encryptedText != originalText);

    QByteArray decryptedText = Crypto::encryptDecrypt(encryptedText, key);

    QCOMPARE(decryptedText, originalText);
}

void TestCrypto::testEmptyString() {
    QByteArray originalText = "";
    QByteArray key = "Key";

    QByteArray encrypted = Crypto::encryptDecrypt(originalText, key);
    QCOMPARE(encrypted, originalText);
}

QTEST_APPLESS_MAIN(TestCrypto)

#include "test_crypto.moc"
