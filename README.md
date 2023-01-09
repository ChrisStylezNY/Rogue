This is an implementation of the classic Asteroids game for the [Flipper Zero](https://flipperzero.one/). Inside you will find a simple 2D engine that can be reused to implement other games. Note: This one is SimplyMinimal's fork of Antirez's version with several modifications.

Controls:
* Left/Right: rotate ship in the two directions.
* Ok, short press: Short burst bullets
* Ok, long press: Auto-fire bullets
* Up: accelerate
* Down: decelerates

Your high scores will automatically be saved. Go forth and compete!

This is a screenshot, but the game looks a lot better in the device itself:

![Asteroids for Flipper Zero screenshot](/images/Asteroids.jpg)

## Installing the binary file (no build needed)

Go to the releases and drop the `asteroids.fap` file into the
following Flipper Zero location:

    /ext/apps/Games

The `ext` part means that we are in the SD card. So if you don't want
to use the Android (or other) application to upload the file,
you can just take out the SD card, insert it in your computer,
copy the fine into `apps/Games`, and that's it.

## Installing the app from source

* Download the Flipper Zero dev kit and build it:
```
mkdir -p ~/flipperZero/official/
cd ~/flipperZero/official/
git clone --recursive  https://github.com/flipperdevices/flipperzero-firmware.git  ./
./fbt
```
* Copy this application folder in `official/application_user`.
* Connect your Flipper via USB.
* Build and install with: `./fbt launch_app APPSRC=asteroids`.

## License

BSD licensed. Enjoy.
