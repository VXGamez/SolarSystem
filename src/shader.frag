#version 330

in vec3 v_color;
in vec2 v_uv;
in vec3 v_un;
in vec3 v_world_vertex_pos;
out vec4 fragColor;

uniform vec3 u_color;
uniform float u_ambient;
uniform vec3 u_light;
uniform vec3 u_light_color;
uniform vec3 u_cam_pos;
uniform float u_shiniess;
uniform sampler2D u_texture;
uniform sampler2D u_texture_night;
uniform sampler2D u_texture_spec;
uniform sampler2D u_texture_normal;

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv){
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x; 
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
	float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
	return mat3(T * invmax, B * invmax, N);
}

vec3 perturbNormal(vec3 N, vec3 V, vec2 texcoord, vec3 normal_pixel) {
	normal_pixel = normal_pixel * 2.0 - 1.0;
	mat3 TBN = cotangent_frame(N, V, texcoord);
	return normalize(TBN * normal_pixel);
}

void main(void){

	vec3 texture_normal = texture(u_texture_normal, v_uv).xyz;
	vec3 N = normalize(v_un);
	vec3 N_orig = N;
	N = perturbNormal(N, v_world_vertex_pos, v_uv, texture_normal);
	N = mix(N_orig, N, 1.0f);
	vec3 L = normalize(u_light);


	vec3 R = reflect(-L,N);
	vec3 E = normalize(u_cam_pos - v_world_vertex_pos);
	float NdotL = max(dot(N,L),0.0);

	vec3 texture_color = texture(u_texture,v_uv).xyz;

	vec3 ambient_color = texture_color * u_ambient;
	vec3 diffuse_color = texture_color * NdotL;

	float RdotE = max(0.0,dot(R,E));
	
	vec3 specular_color = texture_color.xyz * vec3(1.0,1.0,1.0) * pow(RdotE,30);
	
	vec3 final_color = ambient_color + diffuse_color + specular_color;


	fragColor = vec4(final_color,1.0);
	
	


}