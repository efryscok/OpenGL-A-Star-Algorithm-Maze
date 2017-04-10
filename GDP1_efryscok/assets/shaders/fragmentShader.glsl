#version 450

in vec4 exColour;
in vec3 exToEye;
in vec3 exNormal;
in vec4 exTexture;
in vec3 exCubeMapTexture;
in vec3 exEyePosition;
in vec3 exWorldPosition;

uniform bool isCubeMap;
uniform bool hasTexture;
uniform sampler2D texSampler;
uniform samplerCube cubeMapSampler;

out vec4 out_Colour;

void main() {
	if (isCubeMap) {
		out_Colour = texture(cubeMapSampler, exCubeMapTexture.xyz);
	}
	else {
		if (hasTexture) {
			out_Colour = texture(texSampler, exTexture.xy);
			float dot = dot(normalize(exToEye), normalize(exNormal)) * 0.7 + 0.3;
			out_Colour.x *= dot;
			out_Colour.y *= dot;
			out_Colour.z *= dot;
			out_Colour = vec4(1.f, 1.f, 1.f, 1.f);
		}
		else {
			out_Colour = exColour;
			float dot = dot(normalize(exToEye), normalize(exNormal)) * 0.7 + 0.3;
			out_Colour.x *= dot;
			out_Colour.y *= dot;
			out_Colour.z *= dot;
		}
	}

	out_Colour.w = 1.0;
}