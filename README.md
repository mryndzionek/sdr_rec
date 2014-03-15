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

* rtl_asgram - [asgram_rx](https://github.com/jgaeddert/liquid-usrp/blob/master/src/asgram_rx.cc) app converted to rtl-sdr
		Usage: rtl_asgram [OPTION]
		Run receiver, printing ascii spectrogram periodically

  		h     : help
  		f     : center frequency [Hz], default: 100 MHz
  		b     : bandwidth [Hz],        default: 800 kHz
  		G     : gain [dB],             default:  0 = auto
  		p     : ppm_error,             default:  0
  		n     : FFT size,              default:  64
  		o     : offset                 default: -65 dB
  		s     : samplerate,            default: 2048000 Hz)]
  		r     : FFT rate [Hz],         default:   10 Hz
 		L     : output file log size,  default: 4096 samples
  		F     : output filename,       default: 'asgram_rx.dat'
  		d     : device_index,          default: 0

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
