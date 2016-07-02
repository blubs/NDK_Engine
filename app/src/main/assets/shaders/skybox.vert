uniform mat4 mvp;

attribute vec4 vert_pos;

varying vec3 tex_coords;
void main()
{
	tex_coords = normalize(vert_pos.xyz);
	gl_Position = mvp * vec4(vert_pos.xyz,1.0);
	//Placing the z-coord at maximum depth value 1.0
	gl_Position = gl_Position.xyww;
}

