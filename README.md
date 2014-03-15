RTL-SDR and liquid-dsp library - examples
=========================================

Configuration and build process
-------------------------------

```sh
git clone https://github.com/mryndzionek/sdr_rec.git
cd sdr_rec
git submodule init
git submodule update
cd external/liquid-dsp
./bootstrap.sh
./configure --prefix=$PWD/build
make
make install
cd ../rtl-sdr
mkdir project
cd project
cmake -DCMAKE_INSTALL_PREFIX=$PWD/../build ..
make
make install
```

-- UPDATING FROM EXISTING CHECKOUT --

```sh
cd sdr_rec
git pull origin master
git submodule update
```

Implemented examples
--------------------

* rtl_asgram - [asgram_rx](https://github.com/jgaeddert/liquid-usrp/blob/master/src/asgram_rx.cc) converted to rtl-sdr

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
