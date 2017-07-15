# uMatrix
uMatrix is a Matrix protocol client for Ubuntu Touch.

![](uMatrix/logo.svg)

To know more about Matrix go to https://matrix.org/

The application makes use of the libqmatrixclient developed by Felix Rohrbach kde@fxrh.de and others. The source code can be found in https://github.com/QMatrixClient/libqmatrixclient

This application also takes important parts of the code base of Tensor by David A. Roberts, Roman Plášil and others. The source code of tensor can be found in https://github.com/davidar/tensor

## Pre-requisites
Ubunu-SDK (this section will vary with the clickable environment inclusion).

## Building
First, libqmatrixclient has to be compiled. Follow the instructions from https://github.com/QMatrixClient/libqmatrixclient/blob/master/README.md

or in short, in the `uMatrix/lib` folder:

```
mkdir build_dir
cd build_dir
cmake .. # Pass -DCMAKE_PREFIX_PATH and -DCMAKE_INSTALL_PREFIX here if needed
cmake --build . --target all
```

Once the library has been compiled, the app code should be compiled too. This time in the ubuntu sdk. The building instructions for the clickable environment will be released soon.


## Installation
There are different ways to install this app in a phone.

- Use the Ubuntu-SDK to install the created click package.
- Send to yourself the click package over wire, or over telegram, and open the file with the OpenStore app.
-more soon...

## App development
This is the initial release of the uMatrix app. There are several things to improve, both in the usability as in the features side.

In this moment, you can not create a new user within the app. In order to do so, go to https://riot.im/app .
