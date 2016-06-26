uniform mat4 mvp;

uniform mat3 m_IT;

attribute vec4 vert_pos;
attribute vec3 vert_nor;

varying vec4 vert_color;

void main()
{
	vec3 normal = m_IT * vert_nor;
	vert_color = vec4((normal.x+1.0)*0.5,(normal.y+1.0)*0.5,(normal.z+1.0)*0.5,1.0);
	vec4 pos;
	pos = vert_pos;
	pos.w = 1.0;
	gl_Position = mvp * pos;
}

