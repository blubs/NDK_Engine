precision mediump float;
uniform vec4 color;

varying vec4 vert_color;

void main()
{
	gl_FragColor = color*0.01 + vert_color;
}