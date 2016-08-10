precision mediump float;
varying vec4 v_color;
varying vec2 v_uv;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_binor;

uniform sampler2D tex_nor;

void main()
{
	gl_FragColor = v_color * 0.01 + vec4((v_nor.x+1.0)*0.5, (v_nor.y+1.0)*0.5, (v_nor.z+1.0)*0.5, 1.0)*0.01 + vec4(v_uv.x,v_uv.y,0.5,1.0);
}