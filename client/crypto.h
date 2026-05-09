#ifndef CRYPTO_H
#define CRYPTO_H

#include <QByteArray>
#include <QCryptographicHash>

class Crypto {
public:
    static QByteArray encryptDecrypt(const QByteArray &data, const QByteArray &key) {
        QByteArray key_hash = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
        QByteArray result = data;
        for (int i = 0; i < result.size(); ++i) {
            result[i] = result[i] ^ key_hash[i % key_hash.size()];
        }
        return result;
    }

    static QByteArray toBase64(const QByteArray &data) {
        return data.toBase64();
    }

    static QByteArray fromBase64(const QByteArray &data) {
        return QByteArray::fromBase64(data);
    }
};

#endif // CRYPTO_H
