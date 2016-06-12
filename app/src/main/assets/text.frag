//Fragment shader
precision mediump float;

uniform vec4 mult_color;
uniform vec4 add_color;

varying vec2 dest_tex_coord;
uniform sampler2D tex;

void main()
{
	//gl_FragColor = vert_color;
	vec4 tex_col = texture2D(tex,dest_tex_coord);
	//tex_col.a = tex_col.b;
	//tex_col.g = tex_col.b = tex_col.r;
	gl_FragColor = tex_col + 0.0000001*((mult_color * tex_col) + add_color);
}