# Synergy Core

This is the open source core component of Synergy, a keyboard and mouse sharing tool.

## Recommended

Things most people will need.

- [Download](https://symless.com/synergy/download) - Get the compiled version of Synergy 3 and Synergy 1.
- [Contact Support](https://symless.com/contact-support) - Open a support ticket and talk directly to the Synergy team.
- [Help Guides](https://symless.com/help) - Self-help guides and information for when you don't want to talk to people.
- [Symless Blog](https://symless.com/blog/) - Find out what's happening at Symless and with Synergy development.

## Advanced Users

For people who want to contribute to the development of Synergy.

- [Getting Started](https://github.com/symless/synergy-core/wiki/Getting-Started) - How to checkout the code from git and use the right branch.
- [Compiling](https://github.com/symless/synergy-core/wiki/Compiling) - Instructions on how to compile Synergy Core from source.
- [Text Config](https://github.com/symless/synergy-core/wiki/Text-Config) - Write a text config file when running Synergy Core manually.
- [Command Line](https://github.com/symless/synergy-core/wiki/Command-Line) - Go full manual and run Synergy Core from the command line.

## Synergy Vintage

For vintage computer enthusiasts, [Synergy Vintage](https://github.com/nbolton/synergy-vintage) aims to keep the origins of Synergy alive.
You can use Synergy Vintage on operating systems available from 1995 to 2006.

## Build for macos arm64

cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ..

cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ..

## Run build

`cd build/bin`

./synergy_flutter -wi 1080 -hi 1920 -n macbook -c /Users/rohitsangwan/Drive/Devlopment/c++/synergy_core_clean/synergy.conf
