/*
    SOURCE(S):
		https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/4.2.lighting_maps_specular_map/4.2.lamp.fs
*/


#version 330 core
out vec4 FragColor;

uniform float intensity;

void main()
{
	FragColor = vec4(1.0) * intensity; // set alle 4 vector values to 1.0
}
