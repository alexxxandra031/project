FROM ubuntu

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    qtbase5-dev-tools \
    build-essential

# установка библиотек Qt для работы с базой данных SQLite
RUN apt-get install -y \
    libqt5sql5 \
    libqt5sql5-sqlite

WORKDIR /root/
RUN mkdir server
WORKDIR /root/server/

COPY *.cpp /root/server/
COPY *.h /root/server/
COPY *.pro /root/server/
COPY *.db /root/server/

RUN qmake echoServer.pro
RUN make

ENTRYPOINT ["./echoServer"]
