/*
Lixie.cpp - Library for driving Lixie displays!
Created by Connor Nishijima, October 26th 2016.
Released under the GPLv3 license.
*/

#include "Lixie.h"

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

static constexpr byte Lixie::addresses[];

Lixie::Lixie(uint8_t pin, uint8_t nDigits):NumDigits(nDigits), NumLEDs(nDigits * 20){
	leds = new CRGB[NumLEDs];
	led_states = new byte[NumDigits * 3]; // 24 bits for 20 LED states
	colors = new CRGB[NumDigits];
	colors_off = new CRGB[NumDigits];
  
	build_controller(pin);
}

void Lixie::setBit(uint16_t pos, byte val){
	bitWrite(led_states[(pos/8)], pos % 8, val);
}

byte Lixie::getBit(uint16_t pos) const{
	return bitRead(led_states[(pos/8)], pos % 8);
}

void Lixie::begin() {
	controller->showLeds();
	max_power(5,1000); // 5V, 1000mA
	for(byte i = 0; i < NumDigits; i++){
		colors[i] = CRGB(255,255,255);
		colors_off[i] = CRGB(0,0,0);
	}
	color_balance(Tungsten100W);
	clear();
}

void Lixie::clear(bool show_change) {
	for (uint16_t i = 0; i < NumLEDs; i++) {
		setBit(i,0);
	}
	if(show_change == true){
		show();
	}
}

void Lixie::show(){
	for(uint16_t i = 0; i < NumLEDs; i++){
		if(getBit(i) == 1)
			leds[i] = colors[i/20];
		else
			leds[i] = colors_off[i/20];
	}
	controller->showLeds(bright);
}

// set all on color ------------------------------------
void Lixie::color(byte r, byte g, byte b){
	for(byte i = 0; i < NumDigits; i++){
		colors[i].r = r;
		colors[i].g = g;
		colors[i].b = b;
	}
}

void Lixie::color(CRGB c){
	for(byte i = 0; i < NumDigits; i++){
		colors[i] = c;
	}
}

// set index on color ------------------------------------
void Lixie::color(byte r, byte g, byte b, byte index){
	if(index < NumDigits){
		colors[index].r = r;
		colors[index].g = g;
		colors[index].b = b;
	}
}

void Lixie::color(CRGB c, byte index){
	if(index < NumDigits){
		colors[index] = c;
	}
}

// set all off color -------------------------------------
void Lixie::color_off(byte r, byte g, byte b){
	for(byte i = 0; i < NumDigits; i++){
		colors_off[i].r = r;
		colors_off[i].g = g;
		colors_off[i].b = b;
	}
}

void Lixie::color_off(CRGB c){
	for(byte i = 0; i < NumDigits; i++){
		colors_off[i] = c;
	}
}

// set index color off -----------------------------------
void Lixie::color_off(byte r, byte g, byte b, byte index){
	if(index < NumDigits){
		colors_off[index].r = r;
		colors_off[index].g = g;
		colors_off[index].b = b;
	}
}

void Lixie::color_off(CRGB c, byte index){
	if(index < NumDigits){
		colors_off[index] = c;
	}
}

byte Lixie::get_size(uint32_t input) const{
	byte places = 1;
	while(input > 9){
		places++;
		input = input / 10;
	}
	return places;
}

byte Lixie::char_to_number(char input) const{
	return byte(input-48); // convert ascii index to real number
}

bool Lixie::char_is_number(char input) const{
	if(input <= 57 && input >= 48) // if between ASCII '9' and '0'
		return true;
	else
		return false;
}

void Lixie::write(char* input){
	char temp[20] = "";
	byte index = 0;
	for(byte i = 0; i < 20; i++){
		if(char_is_number(input[i])){
			temp[index] = input[i];
			index++;
		}
	}
	uint32_t output = atol(temp);
	write(output);
}

void Lixie::write(uint32_t input){
	char t[20] = "";
	sprintf(t,"%lu",input);
	clear(false);
	if(input != 0){
		for(byte i = 0; i < get_size(input); i++){
			push_digit(char_to_number(t[i]));
		}
	}
	else{
		push_digit(0);
	}
	show();
}

void Lixie::write(int input){
	write(uint32_t(input));
}


void Lixie::write(long input){
	write(uint32_t(input));
}

void Lixie::write(int8_t input){
	write(uint32_t(input));
}

void Lixie::write(byte input){
	write(uint32_t(input));
}

void Lixie::write(float input){
	write(uint32_t(input));
}

void Lixie::write(double input){
	write(uint32_t(input));
}

void Lixie::write_digit(byte input, byte index){
	if(input > 9) return;
  
	uint16_t start = (index*20);

	for(uint16_t i = start; i < start+20; i++){
		setBit(i,0);
	}

	uint16_t L1 = start+addresses[input];
	uint16_t L2 = start+addresses[input] + 10;

	setBit(L1,1);
	setBit(L2,1);

	show();
}

void Lixie::push_digit(byte number) {
	if(number > 9) return;

	// If multiple digits, move all LED states forward one
	if (NumDigits > 1) {
		for (uint16_t i = NumLEDs - 1; i >= 20; i--) {
			setBit(i,getBit(i - 20));
		}
	}
 
	// Clear the LED states for the first digit
	for (uint16_t i = 0; i < 20; i++) {
		setBit(i,0);
	}

	uint16_t L1 = addresses[number];
	uint16_t L2 = addresses[number] + 10;

	setBit(L1,1);
	setBit(L2,1);
}

void Lixie::print_binary() {
	for (uint16_t i = 0; i < NumLEDs; i++) {
		Serial.print(getBit(i));
		if ((i + 1) % 20 == 0 && i != 0) {
			Serial.print('\t');
		}
	}
	Serial.println();
}

uint8_t Lixie::get_numdigits() const{
	return NumDigits;
}

bool Lixie::maxed_out(float input){
	if(input >= pow(10,NumDigits)){ // If input > number that can be displayed
		return false;
	}
	else{
		return true;
	}
}

void Lixie::build_controller(uint8_t DataPin){
	switch (DataPin){
		case 0:
			controller = &FastLED.addLeds<LED_TYPE, 0, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 1:
			controller = &FastLED.addLeds<LED_TYPE, 1, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 2:
			controller = &FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 3:
			controller = &FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 4:
			controller = &FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 5:
			controller = &FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 6:
			controller = &FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 7:
			controller = &FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 8:
			controller = &FastLED.addLeds<LED_TYPE, 8, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 9:
			controller = &FastLED.addLeds<LED_TYPE, 9, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 10:
			controller = &FastLED.addLeds<LED_TYPE, 10, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 11:
			controller = &FastLED.addLeds<LED_TYPE, 11, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 12:
			controller = &FastLED.addLeds<LED_TYPE, 12, COLOR_ORDER>(leds, NumLEDs);
			break;
		case 13:
			controller = &FastLED.addLeds<LED_TYPE, 13, COLOR_ORDER>(leds, NumLEDs);
			break;
	}
}

void Lixie::max_power(byte volts, uint16_t milliamps){
	FastLED.setMaxPowerInVoltsAndMilliamps(volts,milliamps); 
}

void Lixie::color_balance(CRGB c_adj){
	controller->setTemperature(c_adj);
}

void Lixie::brightness(byte newBright){
	bright = newBright;
}

