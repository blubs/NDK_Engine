precision mediump float;
varying vec4 vert_color;
varying vec3 normal;

void main()
{
	gl_FragColor = vert_color * 0.01 + vec4((normal.x+1.0)*0.5, (normal.y+1.0)*0.5, (normal.z+1.0)*0.5, 1.0);
}