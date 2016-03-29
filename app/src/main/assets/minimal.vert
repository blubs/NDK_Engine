//Vertex shader

attribute vec4 vert_pos;
attribute vec4 fill_color;

varying vec4 vert_color;
void main()
{
	vert_color = fill_color;
	gl_Position = vert_pos;
}