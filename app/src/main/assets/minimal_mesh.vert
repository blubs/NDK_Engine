uniform mat4 mvp;
attribute vec4 vert_pos;

varying vec4 vert_color;

void main()
{
	vert_color = vec4(1.0,1.0,1.0,1.0);
	gl_Position = mvp * vec4(vert_pos.x,vert_pos.y,vert_pos.z,1.0);
}