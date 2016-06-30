precision mediump float;
varying vec4 v_color;
varying vec2 v_uv;
varying vec3 v_normal;

void main()
{
	gl_FragColor = v_color * 0.01 + vec4((v_normal.x+1.0)*0.5, (v_normal.y+1.0)*0.5, (v_normal.z+1.0)*0.5, 1.0)*0.01 + vec4(v_uv.x,v_uv.y,0.5,1.0);
}