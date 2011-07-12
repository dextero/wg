uniform sampler2D tex;

uniform vec3 lpos;
uniform vec4 lcolor;
uniform vec2 uTexSize;
uniform float normalStrength;

void main()
{
    // obliczenie rozmiaru 1 texela
    float tx = 1.0 / uTexSize.x;
    float ty = 1.0 / uTexSize.y;
    
	vec4 pixel = texture2D(tex, gl_TexCoord[0].st);
	float grayscale = (pixel.r + pixel.g + pixel.b) * 0.333333333333 * 0.5;
	
    // Obliczenie filtra Sobel    
    vec4 tl = texture2D(tex, gl_TexCoord[0].st + vec2(-tx, -ty)); // top left
    vec4 l  = texture2D(tex, gl_TexCoord[0].st + vec2(-tx,0)); // left
    vec4 bl = texture2D(tex, gl_TexCoord[0].st + vec2(-tx,ty)); // bottom left
    vec4 t = texture2D(tex, gl_TexCoord[0].st + vec2(0,-ty)); // top center
    vec4 b = texture2D(tex, gl_TexCoord[0].st + vec2(0,ty)); // bottom center
    vec4 tr = texture2D(tex, gl_TexCoord[0].st + vec2(tx,-ty)); // top right
    vec4 r  = texture2D(tex, gl_TexCoord[0].st + vec2(tx,0)); // right
    vec4 br = texture2D(tex, gl_TexCoord[0].st + vec2(tx,ty)); // bottom right
    
    // Compute dx using Sobel:
    //           -1 0 1 
    //           -2 0 2
    //           -1 0 1
    vec4 dx = -3.0*tr - 10.0*r - 3.0*br + 3.0*tl + 10.0*l + 3.0*bl;
    dx.r = (dx.r + dx.g + dx.b) * 0.33333333333333;
    // Compute dy using Sobel:
    //           -1 -2 -1 
    //            0  0  0
    //            1  2  1
    vec4 dy = - 3*bl - 10.0*b - 3*br + 3.0*tl + 10.0*t + 3.0*tr;
    dy.r = (dy.r + dy.g + dy.b) * 0.33333333333333;
    
    // tworzenie normalnej:
    vec3 norm = normalize( vec3(dx.r, dy.r, 1.0 / normalStrength)) ;       
    //norm = (norm-0.5) * 2.0;
    
    // normalizacja swiatla (hmm moze by to wywalic...)    
    lpos = normalize(lpos);
    
    // obliczenie jak zmodyfikowac jasnosc (dotproduct zwykly ;p)
    float dotproduct = sqrt( max( dot(norm,lpos), 0.1 ) );
    
    // obliczenie wlasciwego koloru
    vec3 diffuse = dotproduct * lcolor * pixel + grayscale * 0.35;
    gl_FragColor = vec4( diffuse, pixel.a );
}