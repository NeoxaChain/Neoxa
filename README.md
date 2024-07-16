# Neoxa Core <img src="https://github.com/0x00ASTRA/imgs/blob/da8ce370006ff3cf71a45b8f3bc0c744dccb573a/crypto/NEOX.png" width=30>

---

Visit our website:

[Neoxa.net](https://neoxa.net)

## Navigation 🧭

> [Introduction](#introduction-🚩)
>
> [Installation](#installation-⚙️)
>
>[License](#license-🔏)
>
>[Development](#development-👨‍💻)

---

## Introduction 🚩

Welcome to the Neoxa Project! Below you can find general information about the project including installation instructions and development insight. Thank you for supporting Neoxa!

### What is Neoxa?

Neoxa is a blockchain platform that combines gaming and cryptocurrency by allowing users to earn cryptocurrency while playing games. It operates on a Proof of Work mining system. The concept of "Play to Earn" enables gamers to engage with blockchain technology by playing games and receiving rewards. Neoxa aims to bridge the gap between the minable cryptocurrency and gaming communities, providing an opportunity for both to benefit from the blockchain. For more detailed information, please refer to the [Neoxa Whitepaper](https://www.neoxa.net/whitepaper).

### About Neoxa

Neoxa is dedicated to providing gamers with the opportunity to earn Neoxa cryptocurrency while indulging in their favorite games. Our team consists of experienced developers who have a deep passion for gaming, and we are excited to introduce innovative features within our gaming servers. Leveraging the flexibility of PC Gaming and even console platforms, we can utilize modding to incorporate the necessary monitoring functionalities to bring our vision to life.

In contrast to numerous blockchain projects that lack realistic goals or fail to follow through with their plans, Neoxa stands apart by delivering a Proof of Concept right from its launch. We believe in demonstrating the feasibility of our ideas and ensuring tangible progress, earning the trust and confidence of our community.

### NEOX | Neoxa Coin

Neoxa (NEOX) serves as a versatile currency catering to investors, gamers, and gaming developers alike. To enhance the gaming experience and provide earning opportunities, Neoxa has implemented innovative Play to Earn mechanisms within popular games such as Minecraft, Rust, and Grand Theft Auto. By participating in these games, our dedicated gamers can effortlessly earn Neoxa without requiring any financial investment. For more information visit the [Neoxa Website](https://neoxa.net).

---

## Installation ⚙️

### Neoxa Core Wallet

Below you will find instructions for installing the Neoxa Core and Neoxa QT GUI-based wallet application.

#### Install the latest precompiled executables

[Neoxa Releases](https://github.com/NeoxaChain/Neoxa/releases)

##### Windows 10/11

- Download `neoxad.x.x.x.x-win64.zip`.

- Extract the contents

For GUI Wallet:

- Download `neoxaqt.x.x.x.x-win64.zip`.

- Extract the contents and drag the `neoxa-qt` file into the `neoxad.x.x.x.x-win64/` folder.

- Run the `neoxa-qt` application.

##### macOS

- Download `neoxad.x.x.x.x-osx.zip`.

- Extract the contents

For GUI Wallet:

- Download `neoxaqt.x.x.x.x-osx.zip`.

- Extract the contents and drag the `neoxa-qt` file into the `neoxad.x.x.x.x-osx/` folder.

- Open a terminal in the `neoxad.x.x.x.x-osx/` folder.

- Make the files executable:
    `chmod +x *`

- Run the application:
    `./neoxa-qt`.

##### GNU/Linux

- Download `neoxad.x.x.x.x-linux64.zip`.

- Open a terminal in the `Downloads/` folder.

- Create a folder to store the application: binaries

     ~/Downloads $`mkdir Neoxa-Wallet`

Install `unzip` if needed:

Ubuntu:
    `sudo apt-get -y update && sudo apt install -y unzip`

Fedora:
    `sudo dnf -y install unzip`

Arch/Manjaro:
    `sudo pacman -S unzip`

- Extract the contents into the `Neoxa-Wallet/` folder:
    ~/Downloads $`unzip neoxad.x.x.x.x-linux64.zip -d ./Neoxa-Wallet`

For GUI Wallet:

- Download `neoxaqt.x.x.x.x-linux64.zip`.

- Extract the `neoxa-qt` file into the `Neoxa-Wallet/` folder:
    ~/Downloads $`unzip neoxaqt.x.x.x.x-linux64.zip -d ./Neoxa-Wallet`

- Make the files executable:
    `chmod +x *`

- Run the application:
    `./neoxa-qt`.

---

## License 🔏

Neoxa Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

---

## Development 👨‍💻

### Development Process

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/NeoxaChain/Neoxa/tags) are created
regularly to indicate new official, stable release versions of Neoxa Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

### Testing 🧪

Neoxa Core is dedicated to ensure all new updates that are released will be thoroughly tested before entering the mainnet, this is why we have a testnet running alongside our main chain. While the testnet is running we will regularly run unit_testing that is built into Neoxa Core and ensure all reports come back as PASSED.

#### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

---

### Appreciation 🙏

Thank you to the Bitcoin developers.
Thank you to the Ravencoin developers.
