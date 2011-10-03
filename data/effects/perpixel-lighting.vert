// Swiatlo perpixel - program wierzcholkow

varying vec3 vertexPos;

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
	vertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
}