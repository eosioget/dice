# Sicbo

## Overview

Sicbo is a game that every player makes a bet on the banker. Before each bet, the banker placed three dice in a covered vessel and shook them. After players had finished the bet, the banker opened the vessel and dispatched the lottery. The most common game is the size of the number of dice to guess (the sum of dice is 3 to 10 called small, 11 to 18 for large, lose if any triple appears.)



## Fairness

The game uses EOS block chain technology, produces three random number by the banker, and uses the hash algorithm to open the lottery, everyone can verify the fairness of the game at the end of each game. Since neither block chain nor hash algorithm can be reversed by anyone, the result of the game can be guaranteed to be out of anyone's control.

## Algorithm
## Introduction
A random number (32 bytes) is generated and kept secret before the reveal of the bet, and the hash value of the number is publicly displayed as a signature. The signature will not change in this game to ensure that the banker can not cheat, and then the player begins to bet. When the bet is reveal, the seller provides the random number to the game. The game verifies the random number by signature, generates three dice with the random number, and then automatically distributes the lottery. The player can verify the randomness and the fairness of the signature afterwards.

### Pseudo-code

```
If (signature = = sha256 (random[32]) {
    Stock 1 = (random[0] + random[1] + random[2] + random[3] + random[4] + random[5]) % 6 + 1
    Stock 2 = (random[6] + random[7] + random[8] + random[9] + random[10] + random[11]) % 6 + 1
    Stock 3 = (random[12] + random[13] + random[14] + random[15] + random[16] + random[17]) % 6 + 1
} else {
    Verify, however, the game is wrong.
}

```

### Example
Player understanding algorithm needs some basic knowledge of C language, and the values are all hexadecimal.  
Before the game is reveal, players can get the signature that the banker shown.  
`763f42ad937c1a268a17dfbdbb3b8e8bce12bc7cef5ac94f3ab3110ee78f52ca`  
At the end of the game, the banker reveal the hash value (random number)   corresponding to the signature, which is as follows:  
`C6793186e4a0c748118a5070faf7a4ffdd07ea9d07d9f6ca25c4d59b88c6e58a`  
The above formula can be applied to calculate.

```
Commitment = 763f42ad937c1a268a17dfbdbb3b8e8bce12bc7cef5ac94f3ab3110ee78f52ca
Source = c6793186e4a0c748118a5070faf7a4ffdd07ea9d07d9f6ca25c4d59b88c6e58a
If (commitment = = sha256 (source)) {
    3 = (C6 + 79 + 31 + 86 + E4 + A0)% 6 + 1
    1 = (C7 + 48 + 11 + 8A + 50 + 70)% 6 + 1
    5 = (FA + F7 + A4 + FF + DD + 07)% 6 + 1
}

```
3, 1, 5 is the result of the lottery.

### Third party verification

A few public information can be passed on any third party website, such as:  
[https://bloks.io/](https://bloks.io/).

The signature can be inquired before the lottery.

Search - banker name - > Account Transactions - > Contract - > Tables - > Step 2 - Select Table - > game - > Load Table - > current game number - > commitment

The original price is open after the lottery.

Search - banker name - > Account Transactions - > Contract - > Tables - > Step 2 - Select Table - > game - > Load Table - > current game number - > source

Calculate sha256, take the Linux operating system as an example. Note that the calculation of sha256 is a plastic number instead of a string.

```bash
>> echo -n 'c6793186e4a0c748118a5070faf7a4ffdd07ea9d07d9f6ca25c4d59b88c6e58a'x xd-r-p | sha256sum-b | awk'{print $1}';

>> 763f42ad937c1a268a17dfbdbb3b8e8bce12bc7cef5ac94f3ab3110ee78f52ca
```