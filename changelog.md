# LIXIE for ARDUINO CHANGE LOG:
(Most recent at top, please!)

Updated documentation (1/6/17 - dmadison)
-----------------------------------------------------------

Along with all of the other changes in this pull request, I updated the keywords.txt, README.md, and changelog.md (Hi!).

The keywords list was reorganized to match the header file, and I also added a few missing function keywords to the list. The README was updated with all of the new functions that have been added in the past few weeks and the 'uint32_t' datatypes were renamed as 'int' for comprehension purposes. I also changed the wording from "Additonal Funtions" to "*Advanced* Functions", and added another section for "Debug Functions".


Reorganized header function order (1/6/17 - dmadison)
-----------------------------------------------------------

This is an aesthetic change, but I moved around some of the functions in the header file to make a more logical order. Like functions are grouped, with more important functions higher in the list.

Changed 'maxed_out' function (1/6/17 - dmadison)
-----------------------------------------------------------

I was struggling with the inaccuracy of Arduino's floats when I was rewriting the 'write' function, so I thought it was best to get rid of them here as well. The function now takes an integer rather than a float and does the calculation using the 'get_size' function. I also flipped the output, so if the value *does* 'max out' the displays the function will return *true*, rather than false.


Added 'print_current' debug function (1/6/17 - dmadison)
-----------------------------------------------------------

While I was messing with rewriting the 'write' functions, I thought it would be useful to have a function that returned the current values of the displays, based on the led_states array. This function prints values as integers to the serial monitor in the same order they are inputted (i.e. opposite of the data order sent to the Lixies), and requires the Serial class be initialized.


Rewrote integer version of 'write' function (1/6/17 - dmadison)
-----------------------------------------------------------

Previously the integer version of the 'write' function used a temporary character array and sprintf. I rewrote the function to work with pure integer math, which improves performance by ~10% on my setup (ignoring push_digit). It also uses less memory.


Removed overloaded versions of the 'write' function (1/6/17 - dmadison)
-----------------------------------------------------------

Connor had added a few extra functions that overloaded the 'write' function to handle outside cases such as floats and smaller integers. This was a good idea, but in practice the overloaded methods just explicitly invoked a type conversion without doing any additional data transformation. The compiler actually catches these cases automatically, so they did nothing but add a bit of complexity to the library and confuse the compiler (e.g. "call of overloaded 'write(uint16_t&)' is ambiguous"). I removed the extra versions - smaller integers will still fit in the larger uint32_t datatype and floats are automatically type converted by the compiler.


Added input checks to digit write functions (1/6/17 - dmadison)
-----------------------------------------------------------

The lower-level 'write_digit' and 'push_digit' functions now have input sanitization. This should prevent accidentally reading and writing outside of allocated memory.


Cleaned up 'push_digit' function (1/6/17 - dmadison)
-----------------------------------------------------------

I added comments to make the function a bit less obscure, and I removed the redundant 'clear' call because the led_states for the first digit needs to be cleared regardless.


Fixed get_size function bug (1/6/17 - dmadison)
-----------------------------------------------------------

Previously the function would report that any input of '0' had a size of 0. Because the '0' digit still requires one display, the function will return 1 by default and is bounded to 9 rather than 0.


Added brightness control (1/5/17 - dmadison)
-----------------------------------------------------------

With the instance-specific FastLED controllers set up, now seemed like the perfect time to implement brightness control. Brightness control is isolated per-instance, so you can adjust the brightness of each Lixie group independently. You can adjust brightness by calling:

    brightness(255);

Where the parameter is an integer between 0 - 255.

Changed color balance functions to use FastLED (1/5/17 - dmadison)
-----------------------------------------------------------

I rewrote the color balance functionality so that it uses FastLED's color temperature implementation and CRGB objects. This has a number of benefits:

- It's faster: the calibration math is done during the FastLED show() call, and I believe it's also done between bit-bang clock cycles. This mean's adjusting calibration on the fly is quicker.
- It's nondestructive: colors in the color-setting arrays are not changed, which makes gradual color changes easier.
- It's cleaner: I like pretty code.

Created instance-specific FastLED controller (1/5/17 - dmadison)
-----------------------------------------------------------

Each instance now has its own FastLED controller, so things like brightness control, color balance, and show() calls can be isolated to specific displays rather than driven globally.


Added array bounds check to color setting by index (1/5/17 - dmadison)
-----------------------------------------------------------

The color setting functions that allow the user to set a color for a specific digit now check to see if the digit exists before writing to the array. This should prevent memory addressing problems.


Removed 'config.h' and added support for multiple instances (1/4/17)
-----------------------------------------------------------

I removed the config file and changed the class to configure the data pin and number of Lixies in the constructor. This should make initial setup easier, at the cost of dynamic data allocation at runtime.

This update also adds support for multiple Lixie groups! You can now drive multiple Lixie sets independently from the same microcontroller.


Moved color balance math from show() to color()/color_off()
-----------------------------------------------------------

Since we're dealing with floating points, I moved the color balance adjustment math to the color() & color_off() functions, instead of being in show(). This way, the floating point math is potentially only done once, when the color is set.

Shortened "color_on()" to "color()" (1/4/17)
--------------------------------------

To make things a bit quicker to write, I changed the color_on() function to simply: color(). The two color setting functions are now:

    color(r,g,b);
    color_off(r,g,b);

Added color_balance(r_adj, g_adj, b_adj) function: (1/4/17)
-----------------------------------------------------------

By default, CRGB(255,255,255) on a WS2812B produces a very blue-shifted, cold white. I don't think you can call it white if it's light blue! I've added these default color balance values:

    output_r = input_r * 1.00;
    output_g = input_g * 0.94;
    output_b = input_b * 0.59;
    
This equates to a very nice - not hot, not cold - balanced white. But let's say you wanted to change this default to a warm white. Just call **lix.color_balance(1.0, 0.7, 0.3)**.

These values need to be a floating point number between 0 and 1.

Added max_power(volts, milliamps) function: (1/4/17)
---------------------------------------------------

I discovered that FastLED has an option to limit global strip brightness based on the max wattage you'd like to consume. This is amazing! By default, the Lixie library will limit the current consumption to 5W, or 1A @ 5V. Under normal use, each digit will only consume a maximum ~120mA by having two LEDs lighting a digit with full white. You could run 8 digits without hitting the 1A cap.
		
  But some people might want to make the displays flash a color using all LEDs, as in the end of a countdown clock or IFTTT notifications. To protect your power supply (probably USB) from being overloaded, this default 5W cap is placed when lix.begin() is called. To change the maximum consumption allowed, enter your power supply specs into this new function:

    lix.max_power(volts, milliamps);

Be sure to leave some modest headroom for other things like the microcontroller doing all the work!

Started changelog, should have done this sooner! (1/4/17)
---------------------------------------------------------
