#ifndef CRYPTO_H
#define CRYPTO_H

#include <QByteArray>

class Crypto {
public:
    static QByteArray encryptDecrypt(const QByteArray &data, const QByteArray &key) {
        QByteArray result = data;
        for (int i = 0; i < result.size(); ++i) {
            result[i] = result[i] ^ key[i % key.size()];
        }

        return result;
    }
};

#endif // CRYPTO_H
