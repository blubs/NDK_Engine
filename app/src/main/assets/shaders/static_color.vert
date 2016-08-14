uniform mat4 mvp;

uniform mat3 m_IT;

attribute vec4 vert_pos;
attribute vec2 vert_uv_1;
attribute vec2 vert_uv_2;
attribute vec3 vert_nor;

varying vec2 v_uv_1;
varying vec2 v_uv_2;
varying vec4 v_color;

void main()
{
	v_uv_1 = vert_uv_1;
	v_uv_2 = vert_uv_2;
	vec3 normal = m_IT * vert_nor;
	v_color = vec4((normal.x+1.0)*0.5,(normal.y+1.0)*0.5,(normal.z+1.0)*0.5,1.0);
	vec4 pos;
	pos = vert_pos;
	pos.w = 1.0;
	gl_Position = mvp * pos;
}