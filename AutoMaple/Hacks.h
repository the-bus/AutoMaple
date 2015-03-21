#pragma once
#include "inc.h"

class Hacks {

public:
	//not in the lua api:
	static void HookFrame();
	static void UnHookFrame();

	//in the lua api:
	//prepend all methods with "maple."
	//all times are in milliseconds (1000 = 1 second)

	//maple.Message creates a messagebox with a string
	//maple.MessageInt is like Message for ints instead of strings
	//maple.Wait or maple.Sleep waits a certain amount of time
	
	//key stuff:
	//virtual key codes list is on
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731
	static void KeyDown(int32_t); //virtual key code
	static void KeyUp(int32_t); //virtual key code
	static void KeyPress(int32_t); //virtual key code
		// use  KeyPressNoHook if the character is out of game (e.g. in Cash Shop)
	static void KeySpam(int32_t); //virtual key code, spams presses
	static void KeyUnSpam(int32_t); //virtual key code, stops spamming presses
	static void KeyHoldFor(int32_t, int32_t); //first arg: virtual key code, second arg: hold delay
	static void ResetKeys(); //unspams and brings up all keys

	static void EnableAutoPortal();
	static void DisableAutoPortal();

	static void Teleport(int32_t, int32_t); //x and y, not usually stable

	//spawn point control, not working right now:
	static void HookSP();
	static void UnHookSP();
	static void SetSP(int32_t, int32_t); //x and y, spawn point must be hooked

	static void WaitForBreath(); //waits for the character to regain breath, might want to include a delay after this

	//getters:
	static int32_t GetMapID(); //returns the current map's id
	static int32_t GetMobCount(); //returns the number of mobs
	static POINT GetMobClosest(); //returns the closest mob as a point
	static POINT GetChar(); //returns the character as a point
	static pair<POINT *, uint64_t> GetMobs(); //returns a table of mobs as points
	static pair<RECT *, uint64_t> GetRopes(); //returns a table of ropes as rectangles
	static RECT GetMap(); //returns the bounding box of the map as a rectangle
	
	//movement stuff:
	static void SetMoveDelay(int32_t); //sets the delay after a move
	static void SetMoveXOff(int32_t);
		/* sets the x offset
		if the x offset is 25 and you call MoveX with 100 with your character at 0,
		MoveX will keep moving until it sees the character is at 75.
		if your character was at 200, MoveX will move until your character is at 125.
		this is necesarry because SetMove has a delay and so the offset beats the delay.
		characters with different movement speeds will probably have to adjust the x offset they use */
	static void SetRopePollDelay(int32_t); //sets the delay before the loop checks if the y value has changed
	static void HookMove();
	static void UnHookMove();
		//the following methods require move to be hooked:
		static void SetMove(int32_t, int32_t);
			/* move must be hooked
			first arg: -1 for left, 1 for right;
			second arg: -1 for up, 1 for down;
			0 for nothing;
			example: (1, 0) moves right */
		static void MoveX(int32_t); //moves to an x value using the set x offset
		static void MoveXOff(int32_t, int32_t); //first arg: target x, second arg: x offset, not affected by SetMoveXOff
		static void MoveXOffNoStop(int32_t targetX, int32_t off); //moves without stopping afterwards
		static void Rope(int32_t); //-1 for up, 1 for down
		static void RopeY(int32_t); //moves on a rope to a certain y value, not affected by SetRopePollDelay
		static void FaceLeft(); //moves left to face left, requires SetFaceDelay
		static void FaceRight(); //moves right to face right, requires SetFaceDelay

	/* if you call a function that returns a point, it's x and y values can be accessed using .x or .y
	example: maple.GetChar().x */

	/* if you call a function that returns a rect(angle), it will have a table (aka array/list) with 2 points
	the first point in a rectangle is the bottom left point
	the second point in a rectangle is the top right point
	bottom y value of a rope example: maple.GetRopes()[0][0].y */
};