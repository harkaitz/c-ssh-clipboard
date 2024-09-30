SSH-CLIPBOARD (Not finished)
=============

A cross device personal clipboard that uses SSH as it's protocol.

## Requirements.

- OpenSSH

## Installation of a clipboard server in GNU/Linux.

1.- Install the program.

    $ git clone https://github.com/harkaitz/sh-ssh-clipboard sh-ssh-clipboard
    $ cd sh-ssh-clipboard
    $ sudo make install

2.- Open /etc/ssh/sshd_config as root user.

    $ sudo nano /etc/ssh/sshd_config

3.- Add the following line to it, better after "sftp" section.
    
    Subsystem    ssh-clipboard     /usr/local/bin/ssh-clipboard

## Installation in a Linux machine.

1.- Add your server's address in ~/.ssh/clipboard

    echo user@myserver > ~/.ssh/clipboard

## Usage

    m1$ ssh-copy -i hosts < /etc/hosts
    m2$ ssh-paste -i hosts

## Style guide

This project follows the OpenBSD kernel source file style guide (KNF).

Read the guide [here](https://man.openbsd.org/style).

## Collaborating

For making bug reports, feature requests and donations visit
one of the following links:

1. [gemini://harkadev.com/oss/](gemini://harkadev.com/oss/)
2. [https://harkadev.com/oss/](https://harkadev.com/oss/)
