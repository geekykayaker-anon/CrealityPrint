/* finephongtex.vert */
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexcoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec3 viewDirection;
varying vec3 normal;
varying vec2 texcoord;

void main( void )
{
	mat4 modelview_matrix = viewMatrix * modelMatrix;
	vec4 worldPosition = modelview_matrix * vec4(vertexPosition, 1.0);
    gl_Position = projectionMatrix *  worldPosition;
	
	viewDirection  = normalize(vec3(-worldPosition));
    normal          = mat3(modelview_matrix) * vertexNormal;
	texcoord = vertexTexcoord;
}