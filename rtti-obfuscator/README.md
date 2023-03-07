# C++ RTTI Obfuscator
Obfuscates all RTTI (Run-time type information) inside a binary so AC's can't catch you on it.

Credits to https://github.com/koemeet/rtti-obfuscator

The original intention of this project was because the effectiveness of the Koemeet project was not good enough and it required reliance on the Boost library. Therefore, I traversed the .rdata section of the PE and generated random strings.

### Usage

```
$ rtti-obfuscator.exe xxx.dll xxx.rtti.dll "\?(AV|AU).+@"
```

### Preview



