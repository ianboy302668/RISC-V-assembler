# RISC-V assembler simulator for RV32I

## 專案說明
此為計算機組織程式作業，目標為將輸入之 RV32I 系列指令轉為相對應的Machine code。

## 功能說明
* I -type
* R -type
* S -type 
* U -type 
* UJ-type
* SB-type 


**基本流程:** 
1. 輸入以上六種任一型態指令
2. 讀取整份文件，確認總行數
3. 紀錄具label的指令所在位置
4. 檢查每行指令所屬型態
5. 進入各型態的函式處理rd, rs 或 offet的轉換
6. 輸出結果
 

## 使用說明
1. 輸入參數:檔案位置	
2. 確認輸出結果

## 輸入範例
	   add x2,x2,x23
	   lui x2,18
	   addi x24,x24,2
	   bne x24,x23,L2
	   sw x27,0(x10)
	   beq x0,x0,L1
	L2: sw x1,0(x2)
	L1: addi x24,x24,1
	   jalr x0,0(x1)



## 輸出範例
	Type R
	Machine code : 00000 00 10111 00010 000 00010 0110011
	Type U
	Machine code : 00000000000000010010 00010 0110111
	Type I        11   7  5   0
	Machine code : 000000000010 11000 000 11000 0010011
	Type SB
	Machine code : 1 000101 10111 11000 001 0000 1 1100011
	Type S
	Machine code : 000000011011 01010 010 00000 0100011
	Type SB
	Machine code : 0 000111 00000 00000 000 0000 1 1100011
	Type S
	Machine code : 000000000001 00010 010 00000 0100011
	Type I        11   7  5   0
	Machine code : 000000000001 11000 000 11000 0010011
	Type I        11   7  5   0
	Machine code : 000000000000 00001 000 00000 1100111
