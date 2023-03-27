Ruuncoin integration/staging tree
================================

WARNING: It works only with Ununtu 16. you can try your luck if you need it in latest ubuntu version
====================================================================================================

http://www.sites.google.com/view/ruuncoin/home

Copyright (c) 2023 Ruuncoin Developers

What is Ruuncoin?
----------------

Ruuncoin is a Forked Blockchain using scrypt as a proof-of-work algorithm.
 - 2.5 minute block targets
 - subsidy halves in 840k blocks (~2 years)
 - ~4 million total coins

The rest is the same as Bitcoin.
 - 20 coins per block
 - 2016 blocks to retarget difficulty

For more information, as well as an immediately useable, binary version of
the Ruuncoin client sofware, see http://www.sites.google.com/view/ruuncoin/home.


Development process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Ruuncoin
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the patch
submitter will be asked to start a discussion with the devs and community.

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions (see `doc/coding.txt`) or are
controversial.

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/ruuncoin-project/ruuncoin/tags) are created
regularly to indicate new official, stable release versions of Ruuncoin.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test. Please be patient and help out, and
remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code.

Unit tests for the core code are in `src/test/`. To compile and run them:

    cd src; make -f makefile.unix test

Unit tests for the GUI code are in `src/qt/test/`. To compile and run them:

    qmake BITCOIN_QT_TEST=1 -o Makefile.test bitcoin-qt.pro
    make -f Makefile.test
    ./ruuncoin-qt_test

=======
