uniform float radius;

void main()
{
    float multi = 1.0 - (length(gl_TexCoord[0].xy) / radius);

    multi = sqrt(multi);

    gl_FragColor = gl_Color * multi;
}

