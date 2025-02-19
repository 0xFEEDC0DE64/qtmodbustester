# qtmodbustester
Simple Qt modbus tester

[![Build Status](https://travis-ci.com/0xFEEDC0DE64/qtmodbustester.svg?branch=main)](https://travis-ci.com/0xFEEDC0DE64/qtmodbustester)

Currently only supports modbus-tcp and only reading registers (no writing yet)

![Screenshot](/screenshot.png)

## Installing

```bash

yay -S qtwebsockettester-git

```

## Build and Run
```
git clone git@github.com:0xFEEDC0DE64/qtmodbustester.git
cd qtmodbustester
mkdir build
cd build
qmake ..
make
./qtmodbustester
```
