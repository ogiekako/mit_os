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
- Where is the first instruction of the kernel?
  - => `0x10000c:    movw   $0x1234,0x472`
- How does the boot loader decide how many sectors it must read in order to fetch the entire kernel from disk? Where does it find this information?
  - ELF 構造体に記述されている。

# Ex4
pointers.c にコメントした。

# Ex5
object ('.o') ファイルたちは、linker によって、
ELF binary image にされる (obj/kern/kernel)。

