// Swiatlo perpixel - program wierzcholkow
// Przyjmuje pozycje 3 najmocniejszych swiatel w przestrzeni swiata

uniform vec3 lpos1;
uniform vec3 lpos2;
uniform vec3 lpos3;

varying float ldist1;
varying float ldist2;
varying float ldist3;

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	
	// Oblicz odleglosc wierzcholka od zrodel swiatla
	vec3 vertexPos = gl_ModelViewMatrix * gl_Vertex;
	ldist1 = length(vertexPos - lpos1);
	ldist2 = length(vertexPos - lpos2);
	ldist3 = length(vertexPos - lpos3);
}