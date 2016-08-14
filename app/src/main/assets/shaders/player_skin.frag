precision mediump float;
varying vec2 v_uv;

varying vec3 cam_dir_tanspace;
varying vec3 dirlight_dir_tanspace;

uniform sampler2D tex_nor;

void main()
{
	//Getting fragment normal
	//Modulating the normal map strength (lerping between unmodified normal and normal specified by normal map by strength amount)
	const float normal_map_strength = 0.3;
	vec3 normal_dir = normalize(mix(vec3(0,0,1),(texture2D(tex_nor,v_uv).rgb * 2.0 - 1.0),normal_map_strength));

	//Light Calculation
	float diffuse = clamp(dot(normal_dir,dirlight_dir_tanspace),0.0,1.0);

	//Specular shading
	const float shininess = 10.0;//ranged 1-20
	float specular = diffuse * pow(clamp(dot(reflect(-dirlight_dir_tanspace,normal_dir),cam_dir_tanspace),0.0,1.0),shininess);

	//Rim light shading
	const float rim_power = 3.0;//ranged 0.1-10
	float rim = 1.0 - clamp(dot(cam_dir_tanspace,normal_dir),0.0,1.0);
	rim = pow(rim,rim_power);

	const float ambient_light = 0.1;
	float light_power = ambient_light + diffuse + specular + rim;

	vec3 color = vec3(0.5,0.5,0.5);
	gl_FragColor = vec4(color*light_power, 1.0);
}