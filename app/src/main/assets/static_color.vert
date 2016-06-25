uniform mat4 mvp;
attribute vec4 vert_pos;
attribute vec3 vert_nor;

uniform vec4 color;
varying vec4 vert_color;

void main()
{
	vert_color = vec4((vert_nor.x+1.0)*0.5,(vert_nor.y+1.0)*0.5,(vert_nor.z+1.0)*0.5,1.0);
	vec4 pos;
	pos = vert_pos;
	pos.w = 1.0;
	gl_Position = mvp * pos;
}

