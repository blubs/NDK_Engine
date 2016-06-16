uniform mat4 mvp;
attribute vec4 vert_pos;

uniform vec4 color;

void main()
{
	vec4 pos;
	pos = vert_pos;
	pos.w = 1.0;
	gl_Position = mvp * pos;
}

