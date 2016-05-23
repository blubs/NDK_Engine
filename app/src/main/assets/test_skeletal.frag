//Fragment shader
precision mediump float;
varying vec4 vert_color;
varying vec2 dest_tex_coord;
uniform sampler2D tex;

void main()
{
	//gl_FragColor = vert_color;
	gl_FragColor = (vert_color * texture2D(tex,dest_tex_coord));
}