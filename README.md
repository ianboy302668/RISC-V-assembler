# RISC-V assembler simulator for RV32I

### 專案說明
此為計算機組織程式作業，目標為將輸入之 RV32I 系列指令轉為相對應的Machine code。

### 功能說明
* I -type
* R -type
* S -type 
* U -type 
* UJ-type
* SB-type 

以上六種類型指令皆可接受

### 使用說明
1. 輸入參數:檔案位置	
2. 確認輸出結果

### 輸入範例
		add x2,x2,x23
		addi x24,x24,2
		bne x24,x23,L2
		sw x27,0(x10)
		beq x0,x0,L1
	L2: sw x1,0(x2)
	L1: addi x24,x24,1

### 輸出範例
	Type R
	Machine code : 00000 00 10111 00010 000 00010 0110011
	Type I        11   7  5   0
	Machine code : 000000000010 11000 000 11000 0010011
	Type SB
	Machine code : 1 000100 10111 11000 001 0000 0 1100011
	Type S
	Machine code : 000000011011 01010 010 00000 0100011
	Type SB
	Machine code : 0 000110 00000 00000 000 0000 0 1100011
	Type S
	Machine code : 000000000001 00010 010 00000 0100011
	Type I        11   7  5   0
	Machine code : 000000000001 11000 000 11000 0010011
