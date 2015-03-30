#pragma once
#include "inc.h"

#define strmap(type) map<const char *, type>
#define arrpair(type) pair<type, uint64_t>

class Hacks {

public:
	//not in the lua api:
	static void HookFrame();
	static void UnHookFrame();
	static void Reset();

	//in the lua api:
	//prepend all methods with "maple."
	//times are in milliseconds (1000 = 1 second),
	//unless otherwise noted

	//maple.Log prints to the debug log
	//maple.Message creates a messagebox with a string
	//maple.MessageInt is like Message for ints instead of strings
	//maple.Wait or maple.Sleep waits a certain amount of time
	
	//key stuff:
	//virtual key codes list is on
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731
	static void KeyDown(int32_t k); //virtual key code
	static void KeyUp(int32_t k); //virtual key code
	static void KeyPress(int32_t k); //virtual key code
	// use  KeyPressNoHook if the character is out of game (e.g. in Cash Shop)
	static void KeySpam(int32_t k); //virtual key code, spams presses
	static void KeyUnSpam(int32_t k); //virtual key code, stops spamming presses
	static void KeyHoldFor(int32_t k, int32_t delay); //first arg: virtual key code, second arg: hold delay
	static void ResetKeys(); //unspams and brings up all keys

	static void EnableAutoPortal();
	static void DisableAutoPortal();

	static void Teleport(int32_t x, int32_t y); //may not be stable
	static void KamiTeleport(int32_t x, int32_t y); //may not be stable

	//spawn point control:
	static void HookSP();
	static void UnHookSP();
	static void SetSP(int32_t x, int32_t y); //spawn point must be hooked

	static void WaitForBreath(); //waits for the character to regain breath, might want to include a delay after this

	//getters:
	static arrpair(strmap(int32_t) *) GetPortals();
	static int32_t GetMapID(); //returns the current map's id
	static int32_t GetMobCount(); //returns the number of mobs
	static POINT GetMobClosest(); //returns the closest mob as a point
	static strmap(double) GetChar();
		/* returns the character as a point (x and y)
		also has the attackCount
		and time until breath in milliseconds as breath 
		mp and hp
		exp as a decimal percentage value */
	static arrpair(POINT *) GetMobs(); //returns a table of mobs as points
	static arrpair(RECT *) GetRopes(); //returns a table of ropes as rectangles
	static RECT GetMap(); //returns the bounding box of the map as a rectangle

	static int32_t GetItemCount(); //returns the number of items on the map

	static arrpair(strmap(int32_t) *) * GetInventory();
		/* returns a 2D array where the first array has inventory tabs
		0 - eqp
		1 - use
		2 - setup
		3 - etc
		4 - cash
		the inner array is the item(s) at a slot (left to right, then top to bottom)
		every item(s) has an id and quantity */
	
	static void AutoHP(int32_t k, int32_t minhp);
	static void AutoMP(int32_t k, int32_t minmp);

	static bool SendPacket(const char * p); //send a packet

	//movement stuff:
	static void SetMoveDelay(int32_t delay); //sets the delay after a move
	static void SetMoveXOff(int32_t off);
		/* sets the x offset
		if the x offset is 25 and you call MoveX with 100 with your character at 0,
		MoveX will keep moving until it sees the character is at 75.
		if your character was at 200, MoveX will move until your character is at 125.
		this is necesarry because SetMove has a delay and so the offset beats the delay.
		characters with different movement speeds will probably have to adjust the x offset they use */
	static void SetRopePollDelay(int32_t delay); //sets the delay before the loop checks if the y value has changed
	
	static void SetTimeout(int32_t t); //sets the timeout for MoveX* and RopeY in seconds

	static void HookMove();
	static void UnHookMove();
		//the following methods require move to be hooked:
		static void SetMove(int32_t dirX, int32_t dirY);
			/* move must be hooked
			first arg: -1 for left, 1 for right;
			second arg: -1 for up, 1 for down;
			0 for nothing;
			example: (1, 0) moves right */
		static bool MoveX(int32_t targetX); //moves to an x value using the set x offset
		static bool MoveXOff(int32_t targetX, int32_t off); //first arg: target x, second arg: x offset, not affected by SetMoveXOff
		static bool MoveXOffNoStop(int32_t targetX, int32_t off); //moves without stopping afterwards
		
		static void Rope(int32_t dirY); //-1 for up, 1 for down
		static bool RopeY(int32_t targetY); //moves on a rope to a certain y value, not affected by SetRopePollDelay
		
		static void FaceLeft(); //moves left to face left
		static void FaceRight(); //moves right to face right
		
		static void MoveTowardsX(int32_t targetX); //calls SetMove with the direction towards the given x value
		static void MoveTowardsY(int32_t targetY); //calls SetMove with the direction towards the given y value

	/* if you call a function that returns a point, it's x and y values can be accessed using .x or .y
	example: maple.GetMobClosest().x */

	/* if you call a function that returns a rect(angle), it will have a table (aka array/list) with 2 points
	the first point in a rectangle is the bottom left point
	the second point in a rectangle is the top right point
	bottom y value of the first rope example: maple.GetRopes()[0][0].y */
};