precision mediump float;

uniform samplerCube cube_map;
varying vec3 tex_coords;

void main()
{
	gl_FragColor = textureCube(cube_map,tex_coords);
}