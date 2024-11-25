/* 
 * JKSButton library for Arduino 
 * Copyright (C) 2017 Jesus Ruben Santa Anna Zamudio.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Author website: http://www.geekfactory.mx
 * Author e-mail: ruben at geekfactory dot mx
 */

#include "JKSButton.h"

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BR_SHIELD false
#define USE_PHYSICAL_BUTTONS false
#define PORTRAIT true


#if BR_SHIELD
	const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
	const int TS_LEFT = 101, TS_RT = 910, TS_TOP = 76, TS_BOT = 886; // portrait
#else
	//const int XP = 6, XM = A12, YP = A11, YM = 7; //ID=0x9341
	const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
	//const int TS_LEFT = 145, TS_RT = 887, TS_TOP = 934, TS_BOT = 158; // portrait
	const int TS_LEFT = 300, TS_RT = 860, TS_TOP = 944, TS_BOT = 148; // portrait
#endif
    

int ports[10] = {A1, A2, A3, A4, A5, A6, A7, A8, A9, A10};
int index = 0;


JKSButton::JKSButton()
{   
  // button FSM and timers initial state
	_state = E_BTN_WAIT_PRESS;
	_debounceTimer = 0;
	_clicksTimer = 0;

	// pre-set configuration fields
	_holdTimeSetting = 5000;
	_debounceTimeSetting = 300;

	// pre-set external read-only fields
	_pressCount = 0;
	_holdTime = 0;
	_clicks = 0;

	// Clear callback pointers
	cb_onPress = 0;
	cb_onHold = 0;
	cb_onRelease = 0;
	cb_onClick = 0;

	// Status flags
	_flags = 0;
	_flags = 0;
}

void JKSButton::init(
  MCUFRIEND_kbv *tft,
  TouchScreen *ts,
  int16_t x, int16_t y, 
  uint8_t w, uint8_t h, 
  uint16_t outline, uint16_t fill, 
  uint16_t textcolor,
  char *label, uint8_t textsize)
{
    #if USE_PHYSICAL_BUTTONS
        _port = ports[index];
        index++;
    #endif
        
    _tft = tft;
    _ts = ts;
    _gfxButton.initButton(_tft, x, y, w, h, outline, fill, textcolor, label, textsize);
    _gfxButton.drawButton(false);
}

void JKSButton::setHoldTime(uint16_t time)
{
	_holdTimeSetting = time;
}

void JKSButton::setDebounceTime(uint16_t time)
{
	_debounceTimeSetting = time;
}

void JKSButton::setPressHandler(JKSButtonEventHandler_t handler)
{
	cb_onPress = handler;
}

void JKSButton::setHoldHandler(JKSButtonEventHandler_t handler)
{
	cb_onHold = handler;
}

void JKSButton::setReleaseHandler(JKSButtonEventHandler_t handler)
{
	cb_onRelease = handler;
}

void JKSButton::setClicksHandler(JKSButtonEventHandler_t handler)
{
	cb_onClick = handler;
}

uint16_t JKSButton::getPressCount()
{
	return _pressCount;
}

uint16_t JKSButton::getHoldTime()
{
	return _holdTime;
}

uint16_t JKSButton::getClicks()
{
	return _clicks;
}

bool JKSButton::isPressed()
{
	process();
	// If FSM reached the E_BNT_WAIT_RELEASE state
	// we have the button pressed and return ture, otherwise return false
	return(_state == E_BTN_WAIT_RELEASE) ? true : false;
}

bool JKSButton::wasPressed()
{
	process();
	if (_flags & JKSBUTTON_FLAG_WASPRESSED) {
		// Clear the flag
		_flags = _flags & ~JKSBUTTON_FLAG_WASPRESSED;
		return true;
	}
	return false;
}

bool JKSButton::wasLongPress()
{
	process();
	if (_flags & JKSBUTTON_FLAG_LONGPRESS) {
		// Clear the flag
		_flags = _flags & ~JKSBUTTON_FLAG_LONGPRESS;
		return true;
	}
	return false;
}

bool JKSButton::isFirstHold()
{
	return(_flags & JKSBUTTON_FLAG_ISFIRSTHOLD) ? true : false;
}

void JKSButton::process()
{
	switch (_state) {
	case E_BTN_WAIT_PRESS:
		// Check if time expired for multiple clicks
		if (_clicks != 0 && millis() - _clicksTimer >= 1000l) {
			_clicks = 0;
		}
		// Wait for user to press the button
		if (readButtonPin()) {
//			_debounceTimer = millis();
//			_state = E_BTN_DEBOUNCE;
      _gfxButton.drawButton(true);
      _debounceTimer = millis();
        
        // Clear held time
        _holdTime = 0;
        // Clear long press flag on each new button press
        _flags = _flags & ~JKSBUTTON_FLAG_LONGPRESS;
        // Set pressed flag for polling API
        _flags = _flags | JKSBUTTON_FLAG_WASPRESSED;
        // Set the "first hold" flag for the button hold event callback
        _flags = _flags | JKSBUTTON_FLAG_ISFIRSTHOLD;
        // Start time counter for multiple clicks
        if (_clicks == 0)
          _clicksTimer = millis();
        // Increase button press counter
        _pressCount++;
        _clicks++;
        // Fire the onPress event
        if (cb_onPress) {
          cb_onPress(*this);
        }
        // Fire the multiple click event
        if (_clicks >= 2) {
          if (cb_onClick) {
            cb_onClick(*this);
          }
        }
        // Wait for button release
        _state = E_BTN_WAIT_RELEASE;
		}
		break;
	case E_BTN_DEBOUNCE:
		// Wait for debounce time...
//		if (millis() - _debounceTimer >= (unsigned long) _debounceTimeSetting) {
//			// and check if button is still pressed
//			if (readButtonPin()) {
//        _gfxButton.drawButton(true);
//        
//				// Clear held time
//				_holdTime = 0;
//				// Clear long press flag on each new button press
//				_flags = _flags & ~JKSBUTTON_FLAG_LONGPRESS;
//				// Set pressed flag for polling API
//				_flags = _flags | JKSBUTTON_FLAG_WASPRESSED;
//				// Set the "first hold" flag for the button hold event callback
//				_flags = _flags | JKSBUTTON_FLAG_ISFIRSTHOLD;
//				// Start time counter for multiple clicks
//				if (_clicks == 0)
//					_clicksTimer = millis();
//				// Increase button press counter
//				_pressCount++;
//				_clicks++;
//				// Fire the onPress event
//				if (cb_onPress) {
//					cb_onPress(*this);
//				}
//				// Fire the multiple click event
//				if (_clicks >= 2) {
//					if (cb_onClick) {
//						cb_onClick(*this);
//					}
//				}
//				// Wait for button release
//				_state = E_BTN_WAIT_RELEASE;
//			} else {
//				// False trigger (noise), wait for other pulse
//				_state = E_BTN_WAIT_PRESS;
//			}
//		}
		break;
	case E_BTN_WAIT_RELEASE:
		// Check if button is being held
    if (readButtonPin()) {
      _debounceTimer = millis();
    }
    if (millis() - _debounceTimer >= (unsigned long) _debounceTimeSetting) {
      if (cb_onRelease) {
        cb_onRelease(*this);
      }
      _gfxButton.drawButton(false);
      _state = E_BTN_WAIT_PRESS;
    }
//		if (readButtonPin()) {
//			if (millis() - _debounceTimer >= (unsigned long) _holdTimeSetting) {
//				_holdTime += millis() - _debounceTimer;
//				_debounceTimer = millis();
//				// Fire the onHold event
//				if (cb_onHold) {
//					cb_onHold(*this);
//				}
//				// Clear the first long press flag
//				_flags = _flags & ~JKSBUTTON_FLAG_ISFIRSTHOLD;
//				_flags = _flags | JKSBUTTON_FLAG_LONGPRESS;
//			}
//		} else {
//			// Button release, wait for a new button press
//			if (cb_onRelease) {
//				cb_onRelease(*this);
//			}
//      _gfxButton.drawButton(false);
//			_state = E_BTN_WAIT_PRESS;
//		}
		break;
	default:
		_state = E_BTN_WAIT_PRESS;
		break;
	}
}

bool JKSButton::readButtonPin()
{
    #if USE_PHYSICAL_BUTTONS
    
        return digitalRead(_port) == LOW;
        
    #else
    
	   TSPoint p = _ts->getPoint();
        pinMode(YP, OUTPUT);      //restore shared pins
        pinMode(XM, OUTPUT);
        digitalWrite(YP, HIGH);   //because TFT control pins
        digitalWrite(XM, HIGH);
        bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
		//Serial.println(pressed);

        int pixel_x = -100;
        int pixel_y = -100;
        if (pressed) {
			//Serial.println(p.y);
        	#if PORTRAIT
            	pixel_x = map(p.x, TS_LEFT, TS_RT, 0, _tft->width());
            	pixel_y = map(p.y, TS_TOP, TS_BOT, 0, _tft->height());
            #else
    	        pixel_x = map(p.y, TS_TOP, TS_BOT, 0, _tft->width());
    	        pixel_y = map(p.x, TS_LEFT, TS_RT, _tft->height(), 0);
    	    #endif
        }

        return pressed && _gfxButton.contains(pixel_x, pixel_y);
        
    #endif
}
