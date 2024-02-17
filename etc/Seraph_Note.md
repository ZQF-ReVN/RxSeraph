## Structures

### File Structures

```c
struct Seraph_Dat_Entry
{
	uint32_t uiDataFOA;
    // data_size = uiDataFOA - next_entry_FOA
};

struct Seraph_Dat_Data
{
	uint32_t uiCompressFlag; //zlib
	uint8_t aData[data_size];
};

struct Seraph_Dat_Segment_File_Entry
{
    uint32_t uiDataOffsetInSeg; // -> FOA = SegmengtOffset + uiSeraphDataOffsetInSeg
    // data_size = uiSeraphDataFOA - next_entry_Offset
};

struct Seraph_Dat_Segment_Entry
{
    uint32_t uiSegmentFOA;
    uint32_t uiFileCount;
};

// ArchPac.dat: 0x05EBB40D
struct Seraph_Dat_Sengmet_HDR
{
    uint32_t uiSegmentCount;
    uint32_t uiFileCount;
};

struct Seraph_Dat_Type0
{
	uint32_t uiFileCount;
	Seraph_Dat_Entry aEntryList[uiFileCount + 1];// last entry is pack_file_size - 1
	Seraph_Dat_Data aDataList[uiFileCount]
};

struct Seraph_Dat_Type1
{
    Seraph_Dat_Sengmet_HDR HDR; //<- 雛鳥の堕ちる音: ArchPac.dat: 0x05EBB40D
    Seraph_Dat_Segment_Entry aSegEntry[HDR.uiSegmentCount];
    Seraph_Dat_Segment_File_Entry aSegFilesIndex[aSegEntry[-1].uiFileCount +1];
    Seraph_Dat_Segment_File_Entry aSegFilesIndex[aSegEntry[-2].uiFileCount + 1];
    Seraph_Dat_Segment_File_Entry aSegFilesIndex[aSegEntry[-3].uiFileCount + 1];
    ...;
    Seraph_Dat_Segment_File_Entry aSegFilesIndex[aSegEntry[0].uiFileCount + 1]
};
```

### Memory Structures

```c
Seraph_Arch_Entry* sg_ArchIndexPtr = 雛鳥の堕ちる音: 0x0045EF9C;

struct Seraph_Arch_Entry
{
  uint32_t uiIsCash;
  Seraph_Dat_Entry Entry;
};

struct Seraph_Dat_Entry
{
  uint32_t uiDataFOA;
  uint32_t uiDataSize;
};
```

## Command

### ExpressionCommand

```c
0x08 [+]
0x09 [-]
0x0A [*]
0x0B [/]
0x0C [%]
0x0D [&]
0x0E [|]
0x0F [^]
0x11 [==]
0x12 [!=]
0x13 [<=]
0x14 [>=]
0x15 [<]
0x16 [>]
0x17 [& == 0]
0x18 [| == 0]
```

### SystemCommand

```
0x5
	05 00000000 FF
	[byte:type][??][0xFF]
0x19 
    [byte:op][dword:new pc]
0x47 [switch to msg command func]
    47
0x0F [call script 2 xxx]
    0F 091F0000 -> jmp to script2 ,set pc to 0x1F09
0x0E [jmp]
    
0x1F [set packstream param]
    1F 01 E900 020A00050100000009FF
    [byte:op][byte:pack_stream_mod][word:seq][param[1]:seq]
0x56 [set_select]
	56 [Param[5]][val...][text_0][text_1]
0x57 [get_pressed_key]
	57
0x5B [get_click_rect]
	5B
0x5C [get_click_rect]
	5C
0x60 [load imge]
    60 01 E500 Parame[4][byte:flag,if flag == 0xFF, add Parame[2]]
    [byte:op][byte:pack_stream_mod][word:pack_stream_seq][Param[4]][byte:flag,if flag == 0xFF, add Parame[2]]
    60 FE Parame[4][byte:flag,if flag == 0xFF, add Parame[2]]
    [byte:op][byte:0xFE,use before pack_stream_param][Param[4]][byte:flag,if flag == 0xFF, add Parame[2]]
0x71 [load un]
    71 01 E400
    [byte:op][byte:ps_mod][word:ps_seq][Param[1]]
    71 FE 
    [byte:op][byte:0xFE][word:ps_seq][Param[1]]
0x74 [unknow]
    74
```

### SecnarioCommand

```c
0x00 [push_text]
    00 815C815C815C81778E8497A792E99650816982C482A282D982A4816A8A778980817800
0x01 [format_text]
    01 Param[1]
    [byte:op][Param[1]]
0x02 [set_font_color]
    02 00
    [byte:op][byte:color]
0x03 [unknow]
    03 00
    [byte:op][byte:un]
0x04 [unknow]
    04 00
    [byte:op][byte:un]
0x05 [set_font_width]
    05 00
    [byte:op][byte:font_width]
0x06 [set_font_heigh]
    06 00
    [byte:op][byte:font_heigh]
0x07 [not_used]
    07 00
    [byte:op][byte:un]
0x08 [wait time]
    08 0B -> wait (0xB * 10) ms
0x09 [unknow_draw_text_flag]
    09 00
    [byte:op][byte:un]
0x0A [unknow]
    0A 00
    [byte:op][byte:un]
0x0B [unknow_draw_text_flag]
    0B 00
    [byte:op][byte:un]
0x0E [unknow]
    0E 00
    [byte:op][byte:un]
0x0F [play sound]
    0F 00
    [byte:op][byte:sound seq]
0x10 [unknow]
    10 00
    [byte:op][byte:un]
0x11 [unknow]
    11 00
    [byte:op][byte:un]
0x14 [new line]
    14
0x15 [render text and wait input]
    15
0x16 [text indent]
    16
0x17 [next page]
    17
0x18 [voice play]
    18 F0040000
    [byte:op][dword:voice file seq]
0x19 [shake_screen]
    19
    [byte:op]
0xFF [end]
    FF
```



## CD验证

### 参数检测

`0x00412BB3`

游戏启动的时候会传递两个参数
从它安装后创建的图标里可以看到参数设置

```asm
C:\Hinadori\Seraph.exe Hinadori 雛鳥の堕ちる音
```

第一个参数是游戏的英文名或者说罗马音表示，第二个是日文名

两个字符串进入WinMain后会被sscanf读取到两个全局的变量中，英文名主要用于查找游戏的配置INI文件，日文名主要用于设置游戏标题

如果启动缺少这两个参数，会在sscnf的返回值检测出来，从而退出程序

### INI子项检测

`0x00412C00`

紧接着游戏通过 GetPrivateProfileStringA 来从 Assemblage.ini 读取在游戏英文名为section的ini中的key - val，由于 GetPrivateProfileStringA 的设置，不传递完整路径从 Windows 目录读取，所以游戏安装的时候会把 Assemblage.ini 写到Windows目录里，如果Windows目录里没有该文件，或者不存在section中的InstDIR键值则程序退出

### CD检测

`0x00412CE1`

继续往下来到一个cd检测函数，该函数内部主要调用 `0x004129F0` 这个函数，这个函数会循环枚举 a b c d e ... 等盘符来检查盘符名是否为游戏启动时输入的英文名参数，如果检测失败则会提示重试