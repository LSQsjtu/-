        AREA    |.text|, CODE, READONLY

		EXPORT Sum	; 声明子程序可被外部引用
Sum			
		MOV		R1, #0	; 初始化运算结果的值，参数在R0中
loop 
		ADD		R1, R0 	; R1=R1+R0 
		SUBS	R0, #1 	; R0=R0-1，并且根据结果更新标志 
		BNE		loop 	; if (R0≠0) 转 loop; 
		MOV		R0, R1	; 运算结果送R0
		BLX		LR 	; 返回

        END
