        AREA    |.text|, CODE, READONLY

		EXPORT Sum	; �����ӳ���ɱ��ⲿ����
Sum			
		MOV		R1, #0	; ��ʼ����������ֵ��������R0��
loop 
		ADD		R1, R0 	; R1=R1+R0 
		SUBS	R0, #1 	; R0=R0-1�����Ҹ��ݽ�����±�־ 
		BNE		loop 	; if (R0��0) ת loop; 
		MOV		R0, R1	; ��������R0
		BLX		LR 	; ����

        END
