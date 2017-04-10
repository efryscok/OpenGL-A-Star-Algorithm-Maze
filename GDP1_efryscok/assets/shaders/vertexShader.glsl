#version 450

layout(location=0) in vec4 inPosition;
layout(location=1) in vec4 inNormal;
layout(location=2) in vec4 inTexture;

out vec4 exColour;
out vec3 exToEye;
out vec3 exNormal;
out vec4 exTexture;
out vec3 exCubeMapTexture;
out vec3 exEyePosition;
out vec3 exWorldPosition;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 EyePosition;
uniform mat4 ModelMatrix;
uniform vec4 ModelColour;

void main() {
	gl_Position = ModelMatrix * inPosition;
	exWorldPosition = gl_Position.xyz;
	gl_Position = ProjectionMatrix * ViewMatrix * gl_Position;
	
	exEyePosition = EyePosition.xyz;
	exToEye = normalize(exEyePosition - exWorldPosition);
	exNormal =  mat3(ModelMatrix) * inNormal.xyz;
	exColour = ModelColour;
	exTexture = inTexture;

	mat4 normalMatrix = inverse(transpose(ModelMatrix));
	exCubeMapTexture = mat3(normalMatrix) * normalize(inNormal.xyz);
}

