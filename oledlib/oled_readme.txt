This OLED screen graphics library is modified from the uploader of bilibli.com 
called "one program edge" and other predecessors. At present, the graphics library 
is adapted to the 0.96 and 1.3 inch OLED screens on the stm32f1 and stm32f4 platforms, 
and will be considered later Further expand the versatility of the program.


To use this OLED screen graphics library, you need to complete the following steps to configure:
    1. Configure the MCU compilation environment and the communication methods involved 
        in the entire project through macro definitions in oledlib.h
    2. Configure the number of OLED screens used by macro definition in oled_driver.h
    3. Configure the relevant information of the OLED screen in the structure array of the part 2 of oled_driver.c
    4. Configure the OLED screen pins in the OLED_ioDef() function in part 4 of oled_driver.c
    5. Configure the OLED_Confi() function of part 7 of oled_driver.c and call it in the main function for initialization