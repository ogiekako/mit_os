http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html
から抜粋した覚書き

AT&T syntaxの説明。
レジスタは%ではじまる。

e.g. `%eax`

op src, dst

という順番。

e.g. `movl %eax, %ebx`

即値は、$ をつけて書く

e.g. `movl $0xd00d, %ebx`

ドキュメントには書いてなかったけど、`*` がつくと、
jmp インストラクションの時に、PC relative でなく、絶対値でその番号に飛ぶことを[表す](https://sourceware.org/binutils/docs/as/i386_002dVariations.html#i386_002dVariations)。

e.g.  `jmp    *%edx`


Instruction には、b, w, l をつける。(byte, word, longword)
つけないとまちがって推測される恐れがあるので必ずつける。

e.g. `movw %ax, %bx`

グローバル変数には \_ が前につくとドキュメントには書いてあるけど、それは最近は違うらしい。
https://stackoverflow.com/questions/38272921/will-the-functions-and-variables-precede-with-an-when-compiled-using-gcc
実際、適当なコードをアセンブルしてみると\_ がついてなかった。

メモリアクセスは、immed32(basepointer, indexpointer, indexscale) という形を取る。
メモリ番地 immed32 + basepointer + indexpointer * indexscale の値を指す。

e.g.
- `global_v` - グローバル変数の値
- `array(, %eax, 4)` - グローバル変数にインデックスでアクセス
- `(%eax)`  - レジスタが指している値
- `1(%eax)` - `*(p + 1)` where p is `char *`


C でインラインアセンブリは、`__asm__("statement", output registers, input registers, clobberlist)` で書ける。

例
```
int y = 0;
__asm__("movl %1, %%eax\n\t"
    "movl %%eax, %0\n\t"
    : "=r" (y)  // output registers
    : "r" (x)   // input registers
    : "%eax"
    );
printf("%d\n", y);
```

詳細は、
http://d.hatena.ne.jp/wocota/20090628/1246188338
を見ると良い。
