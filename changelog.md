LIXIE for ARDUINO CHANGE LOG:
----------------------------
# 1/4/17:
**- Started changelog, should have done this sooner!**

**- Added max_power(volts,milliamps) function:**

I discovered that FastLED has an option to limit global strip brightness based on the max wattage you'd like to consume. This is amazing! By default, the Lixie library will limit the current consumption to 5W, or 1A @ 5V. Under normal use, each digit will only consume a maximum ~120mA by having two LEDs lighting a digit with full white. You could run 8 digits without hitting the 1A cap.
		
  But some people might want to make the displays flash a color using all LEDs, as in the end of a countdown clock or IFTTT notifications. To protect your power supply (probably USB) from being overloaded, this default 5W cap is placed when lix.begin() is called. To change the maximum consumption allowed, enter your power supply specs into this new function:

    lix.max_power(volts,milliamps);

Be sure to leave some modest headroom for other things like the microcontroller doing all the work!

**- Added color_balance(r_adj, g_adj, b_adj) function:**

By default, CRGB(255,255,255) on a WS2812B produces a very blue-shifted, cold white. I don't think you can call it white if it's light blue! I've added these default color balance values:

    output_r = input_r * 1.00;
    output_g = input_g * 0.94;
    output_b = input_b * 0.59;
    
This equates to a very nice - not hot, not cold - balanced white. But let's say you wanted to change this default to a warm white. Just call **lix.color_balance(1.0, 0.7, 0.3)**.

These values need to be a floating point number between 0 and 1.
