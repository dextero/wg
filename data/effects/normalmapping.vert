// Normalmapping - program wierzcholkow
// Przyjmuje pozycje najmocniejszych swiatel w przestrzeni swiata

#define LIGHTS_CNT 3

uniform mat4 invModelMatrix;

uniform vec3 lpos[LIGHTS_CNT];
varying vec3 ldir[LIGHTS_CNT];
varying float ldist[LIGHTS_CNT];

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	
	// Oblicz wektory od wierzcholka do swiatel w przestrzeni sprite'a/tekstury
	for (int i = 0; i < LIGHTS_CNT; i++)
		ldir[i] = (invModelMatrix * vec4(lpos[i],1.0) - gl_Vertex).xyz;
	
	// Oblicz odleglosc wierzcholka od zrodel swiatla
	// Nie mozemy po prostu obliczyc dlugosci wektorow ldir1, ldir2... poniewaz one sa w przestrzeni sprite'a
	// a radius chcemy podawac w przestrzeni swiata, wiec skalowanie sprite'a pociagaloby skalowanie
	// radiusa (a co jesli sprite jest skalowany inaczej w pionie i poziomie?!)
	vec3 vertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	for (int i = 0; i < LIGHTS_CNT; i++)
		ldist[i] = length(vertexPos - lpos[i]);
}