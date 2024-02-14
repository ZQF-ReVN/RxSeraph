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

## OPCode

```c
0x00 [push text]
    00 815C815C815C81778E8497A792E99650816982C482A282D982A4816A8A778980817800
0x08 [wait time]
    08 0B -> wait (0xB * 10) ms
0x0F [jmp to script 2]
    0F 091F0000 -> jmp to script2 ,set pc to 0x1F09
0x14 [new line]
    14
0x16 [text-indent]
    16
0x15 [wait input]
    15
0x17 [next page]
    17
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