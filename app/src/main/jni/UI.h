//
// Created by F1 on 6/7/2016.
//

#ifndef ENGINE_UI_H
#define ENGINE_UI_H

class UI
{
public:
	//Need to create  methods and variables to handle the following features of the UI class
	//	Draw textures to the UI
	//	Draw text to the UI
	//		Alter the color of the text (utilizing material and a shader property)
	//how to handle UI animations?

	//Beginning with drawing text to the screen
	//	Need to load characters texture and initialize the gl side of it
	//	Need to create a method that parses a string and
	//	Need to create arrays that hold information about character spacing/size/where it is on the charset
	//	Need to create a method for drawing a generic quad to the screen

	//every time draw_text() is called, we execute the above methods outlined
	int init()
	{
		return 1;
	}
	void term()
	{
	}
	int init_gl()
	{
		return 1;
	}
	void term_gl()
	{
	}
};

#endif //ENGINE_UI_H
