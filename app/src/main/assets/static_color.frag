precision mediump float;
uniform vec4 color;

varying vec2 v_uv_1;
varying vec2 v_uv_2;
varying vec4 v_color;

void main()
{
	gl_FragColor = color*0.01 + v_color*0.01 + vec4( v_uv_1.x , v_uv_1.y , v_uv_2.x , v_uv_2.y );
}