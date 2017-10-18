# Ex1.
[memo](guide_in_jp.md) を書いた

# Ex2.
IA-32 (i386) compatible な OS を QEMU
がエミュレートしている。
QEMU の BIOSは最初になにをやっているか？

boot/boot.S は、BIOS が、%cs=0, %ip=7c00
に飛ばすことで実行開始される。
それ以前の部分は、BIOS ROMのブログラムで、
カーネルが書き換えはできない。

```
The target architecture is assumed to be i8086
[f000:fff0]    0xffff0: ljmp   $0xf000,$0xe05b
```
電源が入ると、0xffff0 から実行が開始される。
```
[f000:e05b]    0xfe05b: cmpl   $0x0,%cs:0x6c48
[f000:e062]    0xfe062: jne    0xfd2e1
[f000:e066]    0xfe066: xor    %dx,%dx
[f000:e068]    0xfe068: mov    %dx,%ss
[f000:e06a]    0xfe06a: mov    $0x7000,%esp
[f000:e070]    0xfe070: mov    $0xf3691,%edx
[f000:e076]    0xfe076: jmp    0xfd165
[f000:d165]    0xfd165: mov    %eax,%ecx
[f000:d168]    0xfd168: cli
[f000:d169]    0xfd169: cld
[f000:d16a]    0xfd16a: mov    $0x8f,%eax
[f000:d170]    0xfd170: out    %al,$0x70
[f000:d172]    0xfd172: in     $0x71,%al
[f000:d174]    0xfd174: in     $0x92,%al
[f000:d176]    0xfd176: or     $0x2,%al
[f000:d178]    0xfd178: out    %al,$0x92
[f000:d17a]    0xfd17a: lidtw  %cs:0x6c38
[f000:d180]    0xfd180: lgdtw  %cs:0x6bf4
[f000:d186]    0xfd186: mov    %cr0,%eax
[f000:d189]    0xfd189: or     $0x1,%eax
[f000:d18d]    0xfd18d: mov    %eax,%cr0
[f000:d190]    0xfd190: ljmpl  $0x8,$0xfd198
The target architecture is assumed to be i386
```
QEMU の標準の BIOS は、SeaBIOS.
周辺機器のセットアップをしてから、
0x7c00 に飛ぶ。

# Ex3.
bootmain in boot/main.c
-> readsect
-> bootmain

0x60, 0x64 は、キーボードコントローラのポート。
いかに、Common ports list がある。
http://wiki.osdev.org/I/O_Ports

GDB commands:
- `x/Ni addr` - show instructions
  - `$eip` が現在のアドレスとして使える

1. %cr0 の下位ビットを1にすることによって、32bit protected mode に入る。
1. レジスタを初期化する。%esp (stack pointer) を、0x7c00 にセットする。
1. bootmain を呼ぶ
1. readsect を読んで、ATA harddisk からELFデータをメモリにコピーする
  ディスクからの連続的な読み込みは一命令で行われる。
    7cd7:	f2 6d                	repnz insl (%dx),%es:(%edi)
  repnz は、ZF フラグが立つか、%ecx 回, そのあとの string 系命令を実行する。
1. for loop の中で、Kernel の

- At what point does the processor start executing 32-bit code? What exactly causes the switch from 16- to 32-bit mode?
  - %cr0 の下位ビットを1にすることによって。の下位ビットを1にすることによって。
- What is the last instruction of the boot loader executed, and what is the first instruction of the kernel it just loaded?
  - `=> 0x7d6b:      call   *0x10018`
  - => `0x10000c:    movw   $0x1234,0x472`
- Where is the first instruction of the kernel?
  - kern/entry.S
- How does the boot loader decide how many sectors it must read in order to fetch the entire kernel from disk? Where does it find this information?
  - ELF 構造体に記述されている。

# Ex4
pointers.c にコメントした。

# Ex5
object ('.o') ファイルたちは、linker によって、
ELF binary image にされる (obj/kern/kernel)。

objdump -h obj/boot/boot.out すると、
.text が、0x7c00 から始まってることがわかる。(.text は、program executable が書き込まれる場所)
これは、ld に対する、-Ttext 0x7c00 オプションによって実現されている。

boot/Makefrag の 0x7c00 の値を変えたら、確かに起動しなくなった。

objdump -f obj/kern/kernel で start address をみると、確かに
bootloader が jump している先である、0x0010000c になっている。

# Ex6
bootloader が呼ばれた段階では、まだカーネルはロードされていないので、0x100000 - は、空になっている。kernel に入った段階では、すでにカーネルのコードがロードされている。

# Ex7
VMA (virtual memory address): link address
LMA (load memory address): load address

kernel は、VMA: f0100000 だけど、LMA: 00100000.
となっている。

後々、0x00000000 - 0x0fffffff は、0xf0000000 - 0xfffffff
にマップされる。

protected mode, write protect, pageing
`orl	$(CR0_PE|CR0_PG|CR0_WP), %eax`
https://en.wikipedia.org/wiki/Control_register#CR0

```
(gdb) b *0x100025
Breakpoint 1 at 0x100025
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0x100025:    mov    %eax,%cr0

Breakpoint 1, 0x00100025 in ?? ()
(gdb) x/10x 0x00100000
0x100000:       0x1badb002      0x00000000      0xe4524ffe      0x7205c766
0x100010:       0x34000004      0x0000b812      0x220f0011      0xc0200fd8
0x100020:       0x0100010d      0xc0220f80
(gdb) x/10x 0xf0100000
0xf0100000 <_start+4026531828>: 0x00000000      0x00000000      0x00000000      0x00000000
0xf0100010 <entry+4>:   0x00000000      0x00000000      0x00000000      0x00000000
0xf0100020 <entry+20>:  0x00000000      0x00000000
(gdb) si
=> 0x100028:    mov    $0xf010002f,%eax
0x00100028 in ?? ()
(gdb) x/10x 0x00100000
0x100000:       0x1badb002      0x00000000      0xe4524ffe      0x7205c766
0x100010:       0x34000004      0x0000b812      0x220f0011      0xc0200fd8
0x100020:       0x0100010d      0xc0220f80
(gdb) x/10x 0xf0100000
0xf0100000 <_start+4026531828>: 0x1badb002      0x00000000      0xe4524ffe      0x7205c766
0xf0100010 <entry+4>:   0x34000004      0x0000b812      0x220f0011      0xc0200fd8
0xf0100020 <entry+20>:  0x0100010d      0xc0220f80
(gdb)
```

cr3 を、ページエントリーのアドレスにセットすることによって、paging を設定している。
0x00000000 - 0x0400000,
0xf0000000 - 0xf400000
をともに 0x00000000 - 0x040000 にマップしている。
その後、cr0 に PG フラグをセットすることで、実際に, paging を enable している。
その後は、0xf010002f にジャンプして、二度とlower address には戻ってこない。
(0x10002d で jmpしている)

# Ex 8

primtfmt.c の octal support を実装した。
console.c の cputchar を printf は使っている。
lib/printfmt は単に、cputchar を与えられて、それを使うだけ。

console の cputchar の実装が本質。

- Explain the following from console.c:
  - Buffer overflow をふせぐため。

`cprintf("%d, %x, %d\n", 1, 3, 4);` としたとき
fmt は何を指しているか？
rodata 領域にある、文字列の先頭
ap は何を指しているか？
可変長引数が来る前の引数を与えられると、可変長引数の先頭を指すポインタを返す。
GCC の convention により、

<== 
fmt のポインタ
1
3
4

という感じで、逆順に stack に push されている (stack は上に伸びるとして書いている) から、fmt のポインタを渡すと、(fmt の型をマクロで取れば) 1 を指すポインタを計算することができて、それを ap としている。

`va_arg` は、`*ap` に対して型を渡すと、ちょうどその型のサイズだけポインタをすすめ、値を返す。

## 8.4
He110 World と表示される。
x86 は little endian なので、したのビットからメモリに割り当てる。
メモリアドレスは 1 bite 単位なので、
0x00646c72 は、
72
6c
64
00
と割り当てられ、&i は、72 のアドレスを指す。これを文字列として解釈すると、rld となる。
big endian のシステムでは、バイトの順序を逆にしないとおなじ結果にならない。

%x に関しては、そもそも引数が uint なので、big endian でも同じ引数で良い。

## 8.5
可変長引数がストアされている領域を行き過ぎてしまうから。

## 8.6

もし、最後の引数が最後に push されるようになったとしたら、

# Challenge
あまり興味ないのでとばしまーす。

