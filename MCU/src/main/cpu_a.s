;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        RSEG    CODE:CODE(3)

        PUBLIC  IRQDis
        PUBLIC  IRQEn

        PUBLIC  IDisable
        PUBLIC  IEnable

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)



IRQDis
        CPSID   I
        BX      LR


IRQEn
        CPSIE   I
        BX      LR


IDisable
        MRS     R0, PRIMASK
        CPSID   I
        BX      LR


IEnable
        MSR     PRIMASK, R0
        BX      LR



        END

