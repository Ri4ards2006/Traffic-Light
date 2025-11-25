This example demonstrates how to use the ExtendedTouchEvent library, especially usage of the methods **setRotation()**, 
**isInPieSlice()**, **registerOnTouchClick** and **registerOnTouchLong**. 

A small pie chart is drawn. Clicking inside one of the slices highlights the clicked one, clicking anywhere outside
the chart undoes. A long touch anywhere changes orientation of the screen (cycles through all 4 possibilities) and redraws 
the srceen with the new orientation.

It uses a 2.4" TFT display (with an ILI9341 controler), if **USE_ILI9488** is defined a 3.5" TFT display 
(with an ILI9488 controler) is used instead. Both operate with a resistive touch screen (XPT2046). 

The module works together with an ESP32 but any other arduino compatible processor should be fine, too.

For the used pins see definitions in the code (board_setup.h) - **don't forget to to check whether they fit to your board**
