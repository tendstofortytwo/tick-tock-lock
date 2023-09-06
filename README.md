# tick-tock-lock

a 3DS homebrew app to store and compute TOTPs (time-based one-time passwords). work-in-progress, not security-audited, mainly created so I could try making a 3DS app, inspired by [Drew DeVault's blog post](https://drewdevault.com/2022/10/18/TOTP-is-easy.html).

## building

install the [devkitARM](https://devkitpro.org/wiki/Getting_Started) toolchain and the 3ds-mbedtls package for it (`sudo (dkp-)pacman -S 3ds-mbedtls`). after that, run `make`.

## license

MIT license; see LICENSE.md. thanks to [devkitpro 3ds-examples](https://github.com/devkitPro/3ds-examples/) for the makefile template.
