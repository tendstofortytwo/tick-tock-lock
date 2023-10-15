# tick-tock-lock

a 3DS homebrew app to store and compute TOTPs (time-based one-time passwords). work-in-progress, not security-audited, mainly created so I could try making a 3DS app, inspired by [Drew DeVault's blog post](https://drewdevault.com/2022/10/18/TOTP-is-easy.html).

## building

note that since the QR code decoding library `quirc` is pulled in as a git submodule, you need to use `git clone --recurse-submodules git@github.com:tendstofortytwo/tick-tock-lock` to clone the repository properly.

1. install the [devkitARM](https://devkitpro.org/wiki/Getting_Started) toolchain and the 3ds-mbedtls package for it (`sudo (dkp-)pacman -S 3ds-mbedtls`). 
2. build and install `quirc` into your 3DS prefix by running `./install_quirc.sh`. this will run `sudo` to ask for root access to copy the files into the right location.
3. run `make` to generate your `3dsx` file.

## license

MIT license; see LICENSE.md. thanks to [devkitpro 3ds-examples](https://github.com/devkitPro/3ds-examples/) for the makefile template.
