// Swiatlo perpixel - program wierzcholkow
// Przyjmuje pozycje najmocniejszych swiatel w przestrzeni swiata

#define LIGHTS_CNT 5

uniform vec3 lpos[LIGHTS_CNT];
varying float ldist[LIGHTS_CNT];

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	
	// Oblicz odleglosc wierzcholka od zrodel swiatla
	vec3 vertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	for (int i = 0; i < LIGHTS_CNT; i++)
		ldist[i] = length(vertexPos - lpos[i]);
}