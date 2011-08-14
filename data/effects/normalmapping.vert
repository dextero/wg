// Normalmapping - program wierzcholkow
// Przyjmuje pozycje 3 najmocniejszych swiatel w przestrzeni swiata

uniform mat4 invModelMatrix;

uniform vec3 lpos1;
uniform vec3 lpos2;
uniform vec3 lpos3;

varying vec3 ldir1;
varying vec3 ldir2;
varying vec3 ldir3;

varying float ldist1;
varying float ldist2;
varying float ldist3;

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	
	// Oblicz wektory od wierzcholka do swiatel w przestrzeni sprite'a/tekstury
	ldir1 = (invModelMatrix * vec4(lpos1,1.0)).xyz - gl_Vertex;
	ldir2 = (invModelMatrix * vec4(lpos2,1.0)).xyz - gl_Vertex;
	ldir3 = (invModelMatrix * vec4(lpos3,1.0)).xyz - gl_Vertex;	
	
	// Oblicz odleglosc wierzcholka od zrodel swiatla
	// Nie mozemy po prostu obliczyc dlugosci wektorow ldir1, ldir2... poniewaz one sa w przestrzeni sprite'a
	// a radius chcemy podawac w przestrzeni swiata, wiec skalowanie sprite'a pociagaloby skalowanie
	// radiusa (a co jesli sprite jest skalowany inaczej w pionie i poziomie?!)
	vec3 vertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	ldist1 = length(vertexPos - lpos1);
	ldist2 = length(vertexPos - lpos2);
	ldist3 = length(vertexPos - lpos3);
}