XBright
=======
Extremely simple and lightweight alternative to xbacklight that uses /sys/class/backlight/... (Linux 2.6+ ONLY)

Usage:
------
```bash
# xbright [+-=[0-N]]
```
**NOTE**: N depends on the GPU and may vary. On my machine the range is 0-15, but I've seen the range 0-255 on machines with different GPU makes.

Installation:
-------------
The program is auto-configured during make
```bash
make && sudo make install
```

Uninstallation:
---------------
```bash
sudo make uninstall
```

