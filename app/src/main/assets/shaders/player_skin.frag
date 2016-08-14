precision mediump float;
varying vec4 v_color;
varying vec2 v_uv;

//varying vec3 cam_dir_tanspace;
varying vec3 dirlight_dir_tanspace;

uniform sampler2D tex_nor;

void main()
{
	//gl_FragColor = v_color * 0.01 + vec4((v_nor+1.0)*0.5, 1.0)*0.01 + vec4(v_uv,0.5,1.0);

	//Getting fragment normal
	//Modulating the normal map strength (lerping between unmodified normal and normal specified by normal map by strength amount)
	const float normal_strength = 0.3;
	vec3 tex_nor = normalize(mix(vec3(0,0,-1),(texture2D(tex_nor,v_uv).rgb * 2.0 - 1.0),normal_strength));

	float light_color = clamp(dot(tex_nor,dirlight_dir_tanspace),0.0,1.0);

	//gl_FragColor = vec4(tex_col.rgb,1.0);

	gl_FragColor = v_color * vec4(light_color,light_color,light_color,1.0) + vec4(0.1,0.1,0.1,0.0);
	//The following lines are for testing specific parameters
	//gl_FragColor = vec4(light_color,1.0,0.0,1.0);
	//gl_FragColor = tex_col + vec4(dirlight_dir_tanspace,1.0)*0.01;
	//gl_FragColor = vec4(dirlight_dir_tanspace*0.5 + 0.5,1.0);
	//gl_FragColor = vec4(tex_col.rgb,1.0);
}