precision mediump float;
varying vec4 v_color;
varying vec2 v_uv;

//varying vec3 cam_dir_tanspace;
varying vec3 light_dir_tanspace;

uniform sampler2D tex_nor;

void main()
{
	//gl_FragColor = v_color * 0.01 + vec4((v_nor+1.0)*0.5, 1.0)*0.01 + vec4(v_uv,0.5,1.0);

	//Getting fragment normal
	vec4 tex_col = texture2D(tex_nor,v_uv);
	vec3 tex_nor = normalize(texture2D(tex_nor,v_uv).rgb * 2.0 - 1.0);

	float light_color = clamp(dot(tex_nor,light_dir_tanspace),0.0,1.0);

	gl_FragColor = tex_col * vec4(light_color,light_color,light_color,1.0);
	//The following lines are for testing specific parameters
	//gl_FragColor = vec4(light_color,1.0,0.0,1.0);
	//gl_FragColor = tex_col + vec4(light_dir_tanspace,1.0)*0.01;
	//gl_FragColor = vec4(light_dir_tanspace*0.5 + 0.5,1.0);
}